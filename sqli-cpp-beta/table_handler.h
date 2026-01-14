#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>

// 前向声明
struct sqlite3;
struct sqlite3_stmt;

extern "C" {
#include "sqlite3.h"
}

// 简单的日志宏（替代Qt的qDebug等）
#define DB_LOG_DEBUG(msg) std::cout << "[DEBUG] " << msg << std::endl
#define DB_LOG_WARNING(msg) std::cerr << "[WARNING] " << msg << std::endl
#define DB_LOG_ERROR(msg) std::cerr << "[ERROR] " << msg << std::endl

// 通用表处理器基类接口
class ITableHandler {
public:
	virtual ~ITableHandler() = default;
	virtual std::string getTableName() const = 0;
	virtual std::string getCreateTableSQL() const = 0;
	virtual bool createTable(sqlite3* db) const = 0;
};

// 通用表处理器模板类
// T: 记录类型（如 AirspeedRecord、TakeoffRecord）
template<typename T>
class TableHandler : public ITableHandler {
public:
	TableHandler(sqlite3* db) : m_db(db) {}

	virtual std::string getTableName() const override = 0;
	virtual std::string getCreateTableSQL() const override = 0;

	bool createTable(sqlite3* db) const override {
		std::string sql = getCreateTableSQL();
		int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
		if (rc != SQLITE_OK) {
			DB_LOG_ERROR("Failed to create table " + getTableName() + ": " + sqlite3_errmsg(db));
			return false;
		}
		return true;
	}

	// 插入单条记录（需要子类实现）
	virtual bool insert(const T& record) = 0;

	// 插入多条记录（默认实现使用事务）
	virtual bool insertBatch(const std::vector<T>& records) {
		if (!m_db) return false;

		int rc = sqlite3_exec(m_db, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);
		if (rc != SQLITE_OK) {
			DB_LOG_ERROR("Failed to begin transaction: " + std::string(sqlite3_errmsg(m_db)));
			return false;
		}

		bool success = true;
		for (const auto& record : records) {
			if (!insert(record)) {
				success = false;
				break;
			}
		}

		if (success) {
			rc = sqlite3_exec(m_db, "COMMIT", nullptr, nullptr, nullptr);
			if (rc != SQLITE_OK) {
				DB_LOG_ERROR("Failed to commit transaction: " + std::string(sqlite3_errmsg(m_db)));
				success = false;
			}
		} else {
			sqlite3_exec(m_db, "ROLLBACK", nullptr, nullptr, nullptr);
		}

		return success;
	}

	// 查询所有记录（需要子类实现）
	virtual std::vector<T> getAll() = 0;

	// 根据 ID 查询（需要子类实现）
	virtual T getById(int id) = 0;

	// 更新记录（需要子类实现）
	virtual bool update(const T& record) = 0;

	// 删除记录（需要子类实现）
	virtual bool remove(int id) = 0;

	// 获取记录总数（通用实现）
	virtual int count() {
		if (!m_db) return 0;

		std::string sql = "SELECT COUNT(*) FROM " + getTableName();

		sqlite3_stmt* stmt = nullptr;
		int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
		if (rc != SQLITE_OK) {
			DB_LOG_ERROR("Failed to prepare count query: " + std::string(sqlite3_errmsg(m_db)));
			return 0;
		}

		int result = 0;
		if ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
			result = sqlite3_column_int(stmt, 0);
		}

		sqlite3_finalize(stmt);
		return result;
	}

protected:
	sqlite3* m_db = nullptr;
};
