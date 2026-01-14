#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <typeindex>
#include <iostream>
#include "table_handler.h"

// 前向声明
struct sqlite3;

/**
 * TableHandler 注册表（单例模式）
 * 
 * 使用方法：
 * 1. 在 XXXTableHandler.cpp 文件末尾添加: REGISTER_TABLE_HANDLER(XXXTableHandler)
 * 2. DatabaseManager 通过 registry 统一管理所有表
 * 
 * 优点：
 * - 新增表时无需修改 DatabaseManager
 * - 符合开闭原则（OCP）
 * - 解耦表处理器与数据库管理器
 */
class TableHandlerRegistry {
public:
	// Handler 工厂函数类型
	using HandlerFactory = std::function<std::unique_ptr<ITableHandler>(sqlite3*)>;

	// 获取单例实例
	static TableHandlerRegistry& instance() {
		static TableHandlerRegistry registry;
		return registry;
	}

	// 注册 handler 工厂（模板版本，自动推导类型）
	template<typename HandlerType>
	void registerHandler() {
		auto factory = [](sqlite3* db) -> std::unique_ptr<ITableHandler> {
			return std::make_unique<HandlerType>(db);
		};
		
		// 使用类型索引作为 key，确保每种类型只注册一次
		std::type_index typeIdx(typeid(HandlerType));
		if (m_typeIndexMap.find(typeIdx) == m_typeIndexMap.end()) {
			m_typeIndexMap[typeIdx] = m_factories.size();
			m_factories.push_back({factory, typeIdx});
			std::cout << "[DEBUG] Registered table handler: " << typeid(HandlerType).name() << std::endl;
		}
	}

	// 创建所有已注册的 handler 实例
	std::vector<std::unique_ptr<ITableHandler>> createAllHandlers(sqlite3* db) const {
		std::vector<std::unique_ptr<ITableHandler>> handlers;
		handlers.reserve(m_factories.size());
		for (const auto& entry : m_factories) {
			handlers.push_back(entry.factory(db));
		}
		return handlers;
	}

	// 获取已注册的 handler 数量
	size_t count() const { return m_factories.size(); }

	// 检查某类型是否已注册
	template<typename HandlerType>
	bool isRegistered() const {
		std::type_index typeIdx(typeid(HandlerType));
		return m_typeIndexMap.find(typeIdx) != m_typeIndexMap.end();
	}

private:
	TableHandlerRegistry() = default;
	TableHandlerRegistry(const TableHandlerRegistry&) = delete;
	TableHandlerRegistry& operator=(const TableHandlerRegistry&) = delete;

	struct FactoryEntry {
		HandlerFactory factory;
		std::type_index typeIndex;
	};

	std::vector<FactoryEntry> m_factories;
	std::unordered_map<std::type_index, size_t> m_typeIndexMap;
};

/**
 * 静态自注册辅助类
 * 在全局/静态初始化阶段自动注册 handler
 */
template<typename HandlerType>
class TableHandlerRegistrar {
public:
	TableHandlerRegistrar() {
		TableHandlerRegistry::instance().registerHandler<HandlerType>();
	}
};

/**
 * 自动注册宏
 * 用法：在每个 XXXTableHandler.cpp 文件末尾添加
 * REGISTER_TABLE_HANDLER(XXXTableHandler)
 */
#define REGISTER_TABLE_HANDLER(HandlerType) \
	static TableHandlerRegistrar<HandlerType> g_##HandlerType##_registrar;
