#pragma once

#include <QString>
#include <QByteArray>
#include <QList>
#include <string>
#include "field_metadata.h"

extern "C" {
#include "sqlite3.h"
}

// 类型别名，解决宏中 std::string 的问题
using std_string = std::string;

// ============== 字段声明宏 ==============

#define ORM_DECLARE_FIELD(type, name, sqlType, defaultVal, notNull) \
	type name{};

#define ORM_DECLARE_LIST_FIELD(type, name, sqlType, defaultVal, notNull) \
	type name;

#define ORM_DECLARE_FIELDS(FIELDS) \
	FIELDS(ORM_DECLARE_FIELD, ORM_DECLARE_LIST_FIELD)

// ============== 元数据生成宏 ==============

#define ORM_MAKE_FIELD_METADATA(type, name, sqlType, defaultVal, notNull) \
	{#name, sqlType, defaultVal, notNull, false, false},

#define ORM_MAKE_LIST_METADATA(type, name, sqlType, defaultVal, notNull) \
	{#name, sqlType, defaultVal, notNull, false, false},

#define ORM_FIELD_METADATA(RecordType, FIELDS) \
	static FieldMetadataList getFieldMetadata() { \
		return { \
			{"id", "INTEGER", "", true, true, true}, \
			FIELDS(ORM_MAKE_FIELD_METADATA, ORM_MAKE_LIST_METADATA) \
		}; \
	}

// ============== 字段绑定宏 ==============

#define ORM_BIND_double(stmt, idx, val) \
	sqlite3_bind_double(stmt, idx, val)

#define ORM_BIND_int(stmt, idx, val) \
	sqlite3_bind_int(stmt, idx, val)

#define ORM_BIND_std_string(stmt, idx, val) \
	do { \
		QByteArray _bytes = QString::fromStdString(val).toUtf8(); \
		sqlite3_bind_text(stmt, idx, _bytes.constData(), _bytes.size(), SQLITE_TRANSIENT); \
	} while(0)

#define ORM_BIND_FIELD(type, name, sqlType, defaultVal, notNull) \
	if (fieldName == #name) { \
		ORM_BIND_##type(stmt, index, record.name); \
		return; \
	}

#define ORM_BIND_LIST_FIELD(type, name, sqlType, defaultVal, notNull) \
	if (fieldName == #name) { \
		QString _str = serializeList(record.name); \
		QByteArray _bytes = _str.toUtf8(); \
		sqlite3_bind_text(stmt, index, _bytes.isEmpty() ? nullptr : _bytes.constData(), _bytes.size(), SQLITE_TRANSIENT); \
		return; \
	}

// ============== 字段读取宏 ==============

#define ORM_READ_double(stmt, idx, field) \
	field = sqlite3_column_double(stmt, idx)

#define ORM_READ_int(stmt, idx, field) \
	field = sqlite3_column_int(stmt, idx)

#define ORM_READ_std_string(stmt, idx, field) \
	do { \
		const unsigned char* _text = sqlite3_column_text(stmt, idx); \
		if (_text) field = reinterpret_cast<const char*>(_text); \
	} while(0)

#define ORM_READ_FIELD(type, name, sqlType, defaultVal, notNull) \
	if (fieldName == #name) { \
		ORM_READ_##type(stmt, index, record.name); \
		return; \
	}

#define ORM_READ_LIST_FIELD(type, name, sqlType, defaultVal, notNull) \
	if (fieldName == #name) { \
		const unsigned char* _text = sqlite3_column_text(stmt, index); \
		if (_text) { \
			record.name = deserializeList(QString::fromUtf8(reinterpret_cast<const char*>(_text))); \
		} \
		return; \
	}

// ============== FieldAccessor 生成宏 ==============

#define ORM_FIELD_ACCESSOR(RecordType, FIELDS) \
	struct FieldAccessor { \
		static void bindField(sqlite3_stmt* stmt, int index, const RecordType& record, const QString& fieldName) { \
			FIELDS(ORM_BIND_FIELD, ORM_BIND_LIST_FIELD) \
		} \
		static void readField(sqlite3_stmt* stmt, int index, RecordType& record, const QString& fieldName) { \
			if (fieldName == "id") { record.id = sqlite3_column_int(stmt, index); return; } \
			FIELDS(ORM_READ_FIELD, ORM_READ_LIST_FIELD) \
		} \
	};

// ============== 辅助方法宏 ==============

#define ORM_LIST_HELPERS() \
	static QString serializeList(const QList<double>& list) { \
		if (list.isEmpty()) return ""; \
		QStringList parts; \
		for (double value : list) { \
			parts << QString::number(value, 'g', 15); \
		} \
		return QString("[%1]").arg(parts.join(",")); \
	} \
	static QList<double> deserializeList(const QString& str) { \
		QList<double> result; \
		if (str.isEmpty()) return result; \
		QString trimmed = str.trimmed(); \
		if (trimmed.startsWith('[') && trimmed.endsWith(']')) { \
			trimmed = trimmed.mid(1, trimmed.length() - 2); \
		} \
		if (trimmed.isEmpty()) return result; \
		QStringList parts = trimmed.split(',', Qt::SkipEmptyParts); \
		for (const QString& part : parts) { \
			QString trimmedPart = part.trimmed(); \
			if (!trimmedPart.isEmpty()) { \
				bool ok = false; \
				double value = trimmedPart.toDouble(&ok); \
				if (ok) result.append(value); \
			} \
		} \
		return result; \
	}

