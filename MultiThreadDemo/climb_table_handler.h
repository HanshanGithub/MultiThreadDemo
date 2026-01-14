#pragma once

#include "table_handler.h"
#include "climb.h"
#include "sql_generator.h"

// 前向声明
struct sqlite3;

// Climb 表的处理器
class ClimbTableHandler : public TableHandler<Climb> {
public:
	ClimbTableHandler(sqlite3* db) : TableHandler<Climb>(db) {}

	std::string getTableName() const override {
		return Climb::getTableName();
	}

	std::string getCreateTableSQL() const override {
		// 自动从结构体定义生成 CREATE TABLE SQL
		return SQLGenerator::generateCreateTableSQL(
			Climb::getTableName(),
			Climb::getFieldMetadata()
		);
	}

	bool insert(const Climb& record) override;
	bool insertBatch(const std::vector<Climb>& records) override;
	std::vector<Climb> getAll() override;
	Climb getById(int id) override;
	bool update(const Climb& record) override;
	bool remove(int id) override;
};
