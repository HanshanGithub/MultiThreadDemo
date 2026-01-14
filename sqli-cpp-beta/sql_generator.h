#pragma once

#include "field_metadata.h"
#include <string>
#include <vector>
#include <sstream>

// SQL 生成器：根据字段元数据自动生成各种 SQL 语句
class SQLGenerator {
public:
	// 根据表名和字段元数据生成 CREATE TABLE SQL
	static std::string generateCreateTableSQL(const std::string& tableName, const FieldMetadataList& fields) {
		std::ostringstream sql;
		sql << "CREATE TABLE IF NOT EXISTS " << tableName << " (";
		
		bool first = true;
		for (const auto& field : fields) {
			if (!first) sql << ", ";
			sql << field.toSQL();
			first = false;
		}
		
		sql << ");";
		return sql.str();
	}

	// 生成 INSERT SQL（排除自增主键）
	static std::string generateInsertSQL(const std::string& tableName, const FieldMetadataList& fields) {
		std::vector<std::string> fieldNames;
		std::vector<std::string> placeholders;
		
		for (const auto& field : fields) {
			// 跳过自增主键
			if (field.autoIncrement) {
				continue;
			}
			fieldNames.push_back(field.name);
			placeholders.push_back("?");
		}
		
		std::ostringstream sql;
		sql << "INSERT INTO " << tableName << " (";
		for (size_t i = 0; i < fieldNames.size(); ++i) {
			if (i > 0) sql << ", ";
			sql << fieldNames[i];
		}
		sql << ") VALUES (";
		for (size_t i = 0; i < placeholders.size(); ++i) {
			if (i > 0) sql << ", ";
			sql << placeholders[i];
		}
		sql << ")";
		
		return sql.str();
	}

	// 生成 SELECT SQL（查询所有字段）
	static std::string generateSelectSQL(const std::string& tableName, const FieldMetadataList& fields) {
		std::ostringstream sql;
		sql << "SELECT ";
		for (size_t i = 0; i < fields.size(); ++i) {
			if (i > 0) sql << ", ";
			sql << fields[i].name;
		}
		sql << " FROM " << tableName << " ORDER BY id";
		
		return sql.str();
	}

	// 生成 SELECT BY ID SQL
	static std::string generateSelectByIdSQL(const std::string& tableName, const FieldMetadataList& fields) {
		std::ostringstream sql;
		sql << "SELECT ";
		for (size_t i = 0; i < fields.size(); ++i) {
			if (i > 0) sql << ", ";
			sql << fields[i].name;
		}
		sql << " FROM " << tableName << " WHERE id = ?";
		
		return sql.str();
	}

	// 生成 UPDATE SQL（排除主键）
	static std::string generateUpdateSQL(const std::string& tableName, const FieldMetadataList& fields) {
		std::vector<std::string> setClauses;
		
		for (const auto& field : fields) {
			// 跳过主键
			if (field.primaryKey) {
				continue;
			}
			setClauses.push_back(field.name + " = ?");
		}
		
		std::ostringstream sql;
		sql << "UPDATE " << tableName << " SET ";
		for (size_t i = 0; i < setClauses.size(); ++i) {
			if (i > 0) sql << ", ";
			sql << setClauses[i];
		}
		sql << " WHERE id = ?";
		
		return sql.str();
	}

	// 生成 DELETE SQL
	static std::string generateDeleteSQL(const std::string& tableName) {
		return "DELETE FROM " + tableName + " WHERE id = ?";
	}

	// 获取非自增字段列表（用于 INSERT）
	static FieldMetadataList getInsertFields(const FieldMetadataList& allFields) {
		FieldMetadataList result;
		for (const auto& field : allFields) {
			if (!field.autoIncrement) {
				result.push_back(field);
			}
		}
		return result;
	}

	// 获取非主键字段列表（用于 UPDATE）
	static FieldMetadataList getUpdateFields(const FieldMetadataList& allFields) {
		FieldMetadataList result;
		for (const auto& field : allFields) {
			if (!field.primaryKey) {
				result.push_back(field);
			}
		}
		return result;
	}
};
