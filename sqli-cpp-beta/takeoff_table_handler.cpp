#include "takeoff_table_handler.h"
#include "sql_generator.h"
#include "table_handler_registry.h"
#include <iostream>
#include <string>

extern "C" {
#include "sqlite3.h"
}

// 自动注册到全局注册表
REGISTER_TABLE_HANDLER(TakeoffTableHandler)

bool TakeoffTableHandler::insert(const Takeoff& record) {
	if (!m_db) return false;

	// 自动生成 INSERT SQL（基于字段元数据）
	std::string insertSql = SQLGenerator::generateInsertSQL(
		Takeoff::getTableName(),
		Takeoff::getFieldMetadata()
	);

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_db, insertSql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		DB_LOG_ERROR("Failed to prepare insert statement: " + std::string(sqlite3_errmsg(m_db)));
		return false;
	}

	// 根据字段元数据自动绑定字段值
	FieldMetadataList fields = SQLGenerator::getInsertFields(Takeoff::getFieldMetadata());
	int index = 1;
	for (const auto& field : fields) {
		Takeoff::FieldAccessor::bindField(stmt, index++, record, field.name);
	}

	rc = sqlite3_step(stmt);
	bool success = (rc == SQLITE_DONE);
	if (!success) {
		DB_LOG_ERROR("Failed to insert takeoff record: " + std::string(sqlite3_errmsg(m_db)));
	}

	sqlite3_finalize(stmt);
	return success;
}

std::vector<Takeoff> TakeoffTableHandler::getAll() {
	std::vector<Takeoff> records;
	if (!m_db) return records;

	// 自动生成 SELECT SQL（基于字段元数据）
	std::string sql = SQLGenerator::generateSelectSQL(
		Takeoff::getTableName(),
		Takeoff::getFieldMetadata()
	);

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		DB_LOG_ERROR("Failed to prepare query: " + std::string(sqlite3_errmsg(m_db)));
		return records;
	}

	FieldMetadataList fields = Takeoff::getFieldMetadata();
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		Takeoff record;
		int index = 0;
		for (const auto& field : fields) {
			Takeoff::FieldAccessor::readField(stmt, index++, record, field.name);
		}
		records.push_back(record);
	}

	sqlite3_finalize(stmt);
	return records;
}

Takeoff TakeoffTableHandler::getById(int id) {
	Takeoff record;
	if (!m_db) return record;

	// 自动生成 SELECT BY ID SQL（基于字段元数据）
	std::string sql = SQLGenerator::generateSelectByIdSQL(
		Takeoff::getTableName(),
		Takeoff::getFieldMetadata()
	);

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		DB_LOG_ERROR("Failed to prepare query: " + std::string(sqlite3_errmsg(m_db)));
		return record;
	}

	sqlite3_bind_int(stmt, 1, id);

	FieldMetadataList fields = Takeoff::getFieldMetadata();
	if ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		int index = 0;
		for (const auto& field : fields) {
			Takeoff::FieldAccessor::readField(stmt, index++, record, field.name);
		}
	}

	sqlite3_finalize(stmt);
	return record;
}

bool TakeoffTableHandler::update(const Takeoff& record) {
	if (!m_db) return false;

	// 自动生成 UPDATE SQL（基于字段元数据）
	std::string updateSql = SQLGenerator::generateUpdateSQL(
		Takeoff::getTableName(),
		Takeoff::getFieldMetadata()
	);

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_db, updateSql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		DB_LOG_ERROR("Failed to prepare update statement: " + std::string(sqlite3_errmsg(m_db)));
		return false;
	}

	// 根据字段元数据自动绑定字段值（排除主键）
	FieldMetadataList fields = SQLGenerator::getUpdateFields(Takeoff::getFieldMetadata());
	int index = 1;
	for (const auto& field : fields) {
		Takeoff::FieldAccessor::bindField(stmt, index++, record, field.name);
	}
	// 最后绑定 id
	sqlite3_bind_int(stmt, index, record.id);

	rc = sqlite3_step(stmt);
	bool success = (rc == SQLITE_DONE);
	if (!success) {
		DB_LOG_ERROR("Failed to update takeoff record: " + std::string(sqlite3_errmsg(m_db)));
	}

	sqlite3_finalize(stmt);
	return success;
}

bool TakeoffTableHandler::remove(int id) {
	if (!m_db) return false;

	// 自动生成 DELETE SQL
	std::string deleteSql = SQLGenerator::generateDeleteSQL(Takeoff::getTableName());

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_db, deleteSql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		DB_LOG_ERROR("Failed to prepare delete statement: " + std::string(sqlite3_errmsg(m_db)));
		return false;
	}

	sqlite3_bind_int(stmt, 1, id);

	rc = sqlite3_step(stmt);
	bool success = (rc == SQLITE_DONE);
	if (!success) {
		DB_LOG_ERROR("Failed to delete takeoff record: " + std::string(sqlite3_errmsg(m_db)));
	}

	sqlite3_finalize(stmt);
	return success;
}
