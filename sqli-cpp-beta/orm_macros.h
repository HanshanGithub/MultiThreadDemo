#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
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
		sqlite3_bind_text(stmt, idx, val.c_str(), static_cast<int>(val.size()), SQLITE_TRANSIENT); \
	} while(0)

#define ORM_BIND_FIELD(type, name, sqlType, defaultVal, notNull) \
	if (fieldName == #name) { \
		ORM_BIND_##type(stmt, index, record.name); \
		return; \
	}

#define ORM_BIND_LIST_FIELD(type, name, sqlType, defaultVal, notNull) \
	if (fieldName == #name) { \
		std::string _str = serializeList(record.name); \
		sqlite3_bind_text(stmt, index, _str.empty() ? nullptr : _str.c_str(), static_cast<int>(_str.size()), SQLITE_TRANSIENT); \
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
			record.name = deserializeList(std::string(reinterpret_cast<const char*>(_text))); \
		} \
		return; \
	}

// ============== FieldAccessor 生成宏 ==============

#define ORM_FIELD_ACCESSOR(RecordType, FIELDS) \
	struct FieldAccessor { \
		static void bindField(sqlite3_stmt* stmt, int index, const RecordType& record, const std::string& fieldName) { \
			FIELDS(ORM_BIND_FIELD, ORM_BIND_LIST_FIELD) \
		} \
		static void readField(sqlite3_stmt* stmt, int index, RecordType& record, const std::string& fieldName) { \
			if (fieldName == "id") { record.id = sqlite3_column_int(stmt, index); return; } \
			FIELDS(ORM_READ_FIELD, ORM_READ_LIST_FIELD) \
		} \
	};

// ============== 辅助方法宏 ==============

#define ORM_LIST_HELPERS() \
	static std::string serializeList(const std::vector<double>& list) { \
		if (list.empty()) return ""; \
		std::ostringstream oss; \
		oss << "["; \
		for (size_t i = 0; i < list.size(); ++i) { \
			if (i > 0) oss << ","; \
			oss << std::fixed << std::setprecision(15) << list[i]; \
		} \
		oss << "]"; \
		return oss.str(); \
	} \
	static std::vector<double> deserializeList(const std::string& str) { \
		std::vector<double> result; \
		if (str.empty()) return result; \
		std::string trimmed = str; \
		trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r")); \
		trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1); \
		if (trimmed.empty()) return result; \
		if (trimmed.front() == '[' && trimmed.back() == ']') { \
			trimmed = trimmed.substr(1, trimmed.length() - 2); \
		} \
		if (trimmed.empty()) return result; \
		std::istringstream iss(trimmed); \
		std::string token; \
		while (std::getline(iss, token, ',')) { \
			token.erase(0, token.find_first_not_of(" \t\n\r")); \
			token.erase(token.find_last_not_of(" \t\n\r") + 1); \
			if (!token.empty()) { \
				try { \
					double value = std::stod(token); \
					result.push_back(value); \
				} catch (...) { \
				} \
			} \
		} \
		return result; \
	}
