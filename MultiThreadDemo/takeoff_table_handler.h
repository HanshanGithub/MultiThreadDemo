#pragma once

#include "table_handler.h"
#include "takeoff.h"
#include "sql_generator.h"
#include <QByteArray>

// Takeoff 表的处理器
class TakeoffTableHandler : public TableHandler<Takeoff> {
public:
	TakeoffTableHandler(sqlite3* db) : TableHandler<Takeoff>(db) {}

	QString getTableName() const override {
		return Takeoff::getTableName();
	}

	QString getCreateTableSQL() const override {
		// 自动从结构体定义生成 CREATE TABLE SQL
		return SQLGenerator::generateCreateTableSQL(
			Takeoff::getTableName(),
			Takeoff::getFieldMetadata()
		);
	}

	bool insert(const Takeoff& record) override;
	std::vector<Takeoff> getAll() override;
	Takeoff getById(int id) override;
	bool update(const Takeoff& record) override;
	bool remove(int id) override;
};

