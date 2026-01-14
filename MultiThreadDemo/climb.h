#pragma once

#include "orm_macros.h"

/**
 * ClimbRecord - 爬升记录数据模型
 * 
 * 修改字段只需要改 CLIMB_FIELDS 宏一处！
 */

// =====================================================
// 字段定义（所有字段信息集中在这里！）
// =====================================================
#define CLIMB_FIELDS(X, LIST) \
	X(double, indicatedSpeed, "REAL", "(0)", true) \
	X(double, weight, "REAL", "(4500)", true) \
	X(double, temperature, "REAL", "(15)", true) \
	X(std_string, config, "TEXT", "''", true) \
	X(double, startHeight, "REAL", "(0)", true) \
	X(double, endHeight, "REAL", "(10000)", true) \
	LIST(std::vector<double>, timeList, "TEXT", "", false) \
	LIST(std::vector<double>, speedList, "TEXT", "", false) \
	LIST(std::vector<double>, heightList, "TEXT", "", false) \
	LIST(std::vector<double>, horizontalDistanceList, "TEXT", "", false) \
	LIST(std::vector<double>, angleOfAttackList, "TEXT", "", false) \
	LIST(std::vector<double>, climbAngleList, "TEXT", "", false) \
	LIST(std::vector<double>, massList, "TEXT", "", false) \
	LIST(std::vector<double>, machList, "TEXT", "", false) \
	LIST(std::vector<double>, climbRateList, "TEXT", "", false) \
	LIST(std::vector<double>, dynamicPressureList, "TEXT", "", false) \
    X(double, climbRate, "REAL", "(0)", true) \
	X(double, climbTime, "REAL", "(0)", true) \
	X(double, fuelConsumption, "REAL", "(0)", true) \
	X(double, horizontalDistance, "REAL", "(0)", true)

// =====================================================
// 结构体定义
// =====================================================
struct Climb {
	int id = 0;
	
	ORM_DECLARE_FIELDS(CLIMB_FIELDS)
	ORM_LIST_HELPERS()
	
	static std::string getTableName() {
		return "Climb";
	}
	
	ORM_FIELD_METADATA(Climb, CLIMB_FIELDS)
	ORM_FIELD_ACCESSOR(Climb, CLIMB_FIELDS)
};
