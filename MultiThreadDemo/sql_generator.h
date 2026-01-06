#pragma once

#include "field_metadata.h"
#include <QString>
#include <QStringList>

// SQL 生成器：根据字段元数据自动生成各种 SQL 语句
class SQLGenerator {
public:
	// 根据表名和字段元数据生成 CREATE TABLE SQL
	static QString generateCreateTableSQL(const QString& tableName, const FieldMetadataList& fields) {
		QString sql = "CREATE TABLE IF NOT EXISTS " + tableName + " (";
		
		QStringList fieldDefs;
		for (const auto& field : fields) {
			fieldDefs << field.toSQL();
		}
		
		sql += fieldDefs.join(", ");
		sql += ");";
		
		return sql;
	}

	// 生成 INSERT SQL（排除自增主键）
	static QString generateInsertSQL(const QString& tableName, const FieldMetadataList& fields) {
		QStringList fieldNames;
		QStringList placeholders;
		
		for (const auto& field : fields) {
			// 跳过自增主键
			if (field.autoIncrement) {
				continue;
			}
			fieldNames << field.name;
			placeholders << "?";
		}
		
		return QString("INSERT INTO %1 (%2) VALUES (%3)")
			.arg(tableName)
			.arg(fieldNames.join(", "))
			.arg(placeholders.join(", "));
	}

	// 生成 SELECT SQL（查询所有字段）
	static QString generateSelectSQL(const QString& tableName, const FieldMetadataList& fields) {
		QStringList fieldNames;
		for (const auto& field : fields) {
			fieldNames << field.name;
		}
		
		return QString("SELECT %1 FROM %2 ORDER BY id")
			.arg(fieldNames.join(", "))
			.arg(tableName);
	}

	// 生成 SELECT BY ID SQL
	static QString generateSelectByIdSQL(const QString& tableName, const FieldMetadataList& fields) {
		QStringList fieldNames;
		for (const auto& field : fields) {
			fieldNames << field.name;
		}
		
		return QString("SELECT %1 FROM %2 WHERE id = ?")
			.arg(fieldNames.join(", "))
			.arg(tableName);
	}

	// 生成 UPDATE SQL（排除主键）
	static QString generateUpdateSQL(const QString& tableName, const FieldMetadataList& fields) {
		QStringList setClauses;
		
		for (const auto& field : fields) {
			// 跳过主键
			if (field.primaryKey) {
				continue;
			}
			setClauses << QString("%1 = ?").arg(field.name);
		}
		
		return QString("UPDATE %1 SET %2 WHERE id = ?")
			.arg(tableName)
			.arg(setClauses.join(", "));
	}

	// 生成 DELETE SQL
	static QString generateDeleteSQL(const QString& tableName) {
		return QString("DELETE FROM %1 WHERE id = ?").arg(tableName);
	}

	// 获取非自增字段列表（用于 INSERT）
	static FieldMetadataList getInsertFields(const FieldMetadataList& allFields) {
		FieldMetadataList result;
		for (const auto& field : allFields) {
			if (!field.autoIncrement) {
				result.append(field);
			}
		}
		return result;
	}

	// 获取非主键字段列表（用于 UPDATE）
	static FieldMetadataList getUpdateFields(const FieldMetadataList& allFields) {
		FieldMetadataList result;
		for (const auto& field : allFields) {
			if (!field.primaryKey) {
				result.append(field);
			}
		}
		return result;
	}
};

