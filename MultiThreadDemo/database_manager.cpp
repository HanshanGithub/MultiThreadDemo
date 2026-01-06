#include "database_manager.h"
#include <QDebug>
#include <QDir>
#include <QByteArray>

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
		qDebug() << "Initialized" << m_handlers.size() << "table handlers from registry";
	}
}

bool DatabaseManager::open(const QString& dbPath, const QString& key) {
	if (m_isOpen) {
		qWarning() << "Database already opened";
		return true;
	}

	m_dbPath = dbPath;

	// 使用原生 SQLite C API 打开数据库
	int rc = sqlite3_open(dbPath.toUtf8().constData(), &m_db);
	if (rc != SQLITE_OK) {
		qCritical() << "Failed to open database:" << sqlite3_errmsg(m_db);
		sqlite3_close(m_db);
		m_db = nullptr;
		return false;
	}

	// 设置 SQLCipher 密钥
	QByteArray keyBytes = key.toUtf8();
	rc = sqlite3_key(m_db, keyBytes.constData(), keyBytes.size());
	if (rc != SQLITE_OK) {
		qCritical() << "Failed to set SQLCipher key:" << sqlite3_errmsg(m_db);
		sqlite3_close(m_db);
		m_db = nullptr;
		return false;
	}

	// 验证密钥是否正确
	sqlite3_stmt* stmt = nullptr;
	rc = sqlite3_prepare_v2(m_db, "SELECT COUNT(*) FROM sqlite_master", -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		qCritical() << "Invalid SQLCipher key or database corrupted:" << sqlite3_errmsg(m_db);
		sqlite3_close(m_db);
		m_db = nullptr;
		return false;
	}
	sqlite3_finalize(stmt);

	m_isOpen = true;
	initTableHandlers();
	qDebug() << "Database opened successfully:" << dbPath;
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
		qDebug() << "Database closed";
	}
}

bool DatabaseManager::syncSchema() {
	if (!m_isOpen || !m_db) {
		qCritical() << "Database not opened";
		return false;
	}

	initTableHandlers();

	// 自动遍历所有已注册的 handler 创建表
	for (const auto& handler : m_handlers) {
		if (!handler->createTable(m_db)) {
			qCritical() << "Failed to create table:" << handler->getTableName();
			return false;
		}
		qDebug() << "Created table:" << handler->getTableName();
	}

	qDebug() << "Schema synced successfully, total tables:" << m_handlers.size();
	return true;
}

