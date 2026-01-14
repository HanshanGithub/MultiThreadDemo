#include "database_manager.h"
#include <iostream>
#include <cstring>

extern "C" {
#include "sqlite3.h"
}

DatabaseManager::DatabaseManager() {
}

DatabaseManager::~DatabaseManager() {
	close();
}

void DatabaseManager::initTableHandlers() {
	if (m_db && m_handlers.empty()) {
		// 从注册表自动创建所有已注册的 handler
		m_handlers = TableHandlerRegistry::instance().createAllHandlers(m_db);
		std::cout << "[DEBUG] Initialized " << m_handlers.size() << " table handlers from registry" << std::endl;
	}
}

bool DatabaseManager::open(const std::string& dbPath, const std::string& key) {
	if (m_isOpen) {
		std::cerr << "[WARNING] Database already opened" << std::endl;
		return true;
	}

	m_dbPath = dbPath;

	// 使用原生 SQLite C API 打开数据库
	int rc = sqlite3_open(dbPath.c_str(), &m_db);
	if (rc != SQLITE_OK) {
		std::cerr << "[ERROR] Failed to open database: " << sqlite3_errmsg(m_db) << std::endl;
		sqlite3_close(m_db);
		m_db = nullptr;
		return false;
	}

	// 设置 SQLCipher 密钥
	rc = sqlite3_key(m_db, key.c_str(), static_cast<int>(key.size()));
	if (rc != SQLITE_OK) {
		std::cerr << "[ERROR] Failed to set SQLCipher key: " << sqlite3_errmsg(m_db) << std::endl;
		sqlite3_close(m_db);
		m_db = nullptr;
		return false;
	}

	// 验证密钥是否正确
	sqlite3_stmt* stmt = nullptr;
	rc = sqlite3_prepare_v2(m_db, "SELECT COUNT(*) FROM sqlite_master", -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << "[ERROR] Invalid SQLCipher key or database corrupted: " << sqlite3_errmsg(m_db) << std::endl;
		sqlite3_close(m_db);
		m_db = nullptr;
		return false;
	}
	sqlite3_finalize(stmt);

	m_isOpen = true;
	initTableHandlers();
	std::cout << "[DEBUG] Database opened successfully: " << dbPath << std::endl;
	return true;
}

void DatabaseManager::close() {
	if (m_isOpen && m_db) {
		// 清理所有 handler（自动处理，无需逐个列出）
		m_handlerCache.clear();
		m_handlers.clear();
		
		sqlite3_close(m_db);
		m_db = nullptr;
		m_isOpen = false;
		std::cout << "[DEBUG] Database closed" << std::endl;
	}
}

bool DatabaseManager::syncSchema() {
	if (!m_isOpen || !m_db) {
		std::cerr << "[ERROR] Database not opened" << std::endl;
		return false;
	}

	initTableHandlers();

	// 自动遍历所有已注册的 handler 创建表
	for (const auto& handler : m_handlers) {
		if (!handler->createTable(m_db)) {
			std::cerr << "[ERROR] Failed to create table: " << handler->getTableName() << std::endl;
			return false;
		}
		std::cout << "[DEBUG] Created table: " << handler->getTableName() << std::endl;
	}

	std::cout << "[DEBUG] Schema synced successfully, total tables: " << m_handlers.size() << std::endl;
	return true;
}
