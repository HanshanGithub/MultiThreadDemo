#pragma once

#include <QString>
#include <QStringList>
#include <QVariant>

// 字段元数据：描述数据库字段的属性
struct FieldMetadata {
	QString name;           // 字段名
	QString sqlType;       // SQL 类型（如 "REAL", "TEXT", "INTEGER"）
	QString defaultValue;  // 默认值（SQL 格式，如 "0", "'si'", "(1000)"）
	bool notNull;          // 是否 NOT NULL
	bool primaryKey;       // 是否主键
	bool autoIncrement;    // 是否自增

	// 生成字段定义的 SQL 片段
	QString toSQL() const {
		QString sql = name + " " + sqlType;
		
		if (primaryKey) {
			sql += " PRIMARY KEY";
		}
		if (autoIncrement) {
			sql += " AUTOINCREMENT";
		}
		if (notNull) {
			sql += " NOT NULL";
		}
		if (!defaultValue.isEmpty()) {
			sql += " DEFAULT " + defaultValue;
		}
		
		return sql;
	}
};

// 字段元数据列表类型
using FieldMetadataList = QList<FieldMetadata>;

