#pragma once

#include "table_handler.h"
#include "table_handler_registry.h"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <iostream>

// 前向声明
struct sqlite3;

/**
 * 数据库管理器类
 * 
 * 使用注册表模式，符合开闭原则（OCP）：
 * - 新增表时只需在对应的 TableHandler.cpp 添加 REGISTER_TABLE_HANDLER 宏
 * - DatabaseManager 无需修改
 * 
 * 使用示例：
 *   DatabaseManager db;
 *   db.open("test.db", "password");
 *   
 *   // 获取特定类型的 handler
 *   auto* takeoffHandler = db.getHandler<TakeoffTableHandler>();
 *   if (takeoffHandler) {
 *       takeoffHandler->insert(record);
 *   }
 */
class DatabaseManager {
public:
	DatabaseManager();
	~DatabaseManager();

	// 打开数据库连接（带 SQLCipher 密钥）
	bool open(const std::string& dbPath, const std::string& key = "123456");

	// 关闭数据库连接
	void close();

	// 同步数据库架构（自动创建所有已注册的表）
	bool syncSchema();

	// 获取指定类型的 TableHandler（类型安全）
	template<typename HandlerType>
	HandlerType* getHandler() {
		if (!m_isOpen || !m_db) {
			std::cerr << "[ERROR] Database not opened" << std::endl;
			return nullptr;
		}

		std::type_index typeIdx(typeid(HandlerType));
		
		// 先在缓存中查找
		auto it = m_handlerCache.find(typeIdx);
		if (it != m_handlerCache.end()) {
			return dynamic_cast<HandlerType*>(it->second);
		}

		// 缓存中没有，从 handlers 列表中查找
		for (auto& handler : m_handlers) {
			if (auto* typed = dynamic_cast<HandlerType*>(handler.get())) {
				m_handlerCache[typeIdx] = typed;
				return typed;
			}
		}

		std::cerr << "[WARNING] Handler not found for type: " << typeid(HandlerType).name() << std::endl;
		return nullptr;
	}

	// 检查数据库是否打开
	bool isOpen() const { return m_db != nullptr && m_isOpen; }

	// 获取底层数据库连接（供高级用法）
	sqlite3* getDatabase() const { return m_db; }

	// 获取已注册的表数量
	size_t getTableCount() const { return m_handlers.size(); }

private:
	sqlite3* m_db = nullptr;
	std::string m_dbPath;
	bool m_isOpen = false;

	// 所有 handler 的统一存储
	std::vector<std::unique_ptr<ITableHandler>> m_handlers;
	
	// 类型到 handler 的缓存（加速 getHandler 查找）
	std::unordered_map<std::type_index, ITableHandler*> m_handlerCache;

	// 初始化所有已注册的 handler
	void initTableHandlers();
};
