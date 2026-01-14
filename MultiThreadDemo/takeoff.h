#pragma once

#include "orm_macros.h"

/**
 * TakeoffRecord - 起飞记录数据模型
 * 
 * 修改字段只需要改 TAKEOFF_FIELDS 宏一处！
 */

// =====================================================
// 字段定义（所有字段信息集中在这里！）
// =====================================================
#define TAKEOFF_FIELDS(X, LIST) \
	X(double, height, "REAL", "(1000)", true) \
	X(double, weight, "REAL", "(4500)", true) \
	X(double, windSpeed, "REAL", "(0)", true) \
	X(double, temperature, "REAL", "(15)", true) \
	X(double, runwaySlope, "REAL", "(0)", true) \
	X(std_string, config, "TEXT", "''", true) \
	LIST(std::vector<double>, timeList, "TEXT", "", false) \
	LIST(std::vector<double>, groundSpeedList, "TEXT", "", false) \
	LIST(std::vector<double>, machList, "TEXT", "", false) \
	LIST(std::vector<double>, distList, "TEXT", "", false) \
	LIST(std::vector<double>, heightList, "TEXT", "", false) \
	LIST(std::vector<double>, angleOfAttackList, "TEXT", "", false) \
	LIST(std::vector<double>, inclinationList, "TEXT", "", false) \
	LIST(std::vector<double>, accelerationList, "TEXT", "", false) \
	LIST(std::vector<double>, thrustList, "TEXT", "", false) \
	LIST(std::vector<double>, frictionResistanceList, "TEXT", "", false) \
	LIST(std::vector<double>, aerodynamicDragList, "TEXT", "", false) \
	X(double, runDistance, "REAL", "(0)", true) \
	X(double, runTime, "REAL", "(0)", true) \
	X(double, obstacleCrossingDistance, "REAL", "(0)", true)

// =====================================================
// 结构体定义
// =====================================================
struct Takeoff {
	int id = 0;
	
	ORM_DECLARE_FIELDS(TAKEOFF_FIELDS)
	ORM_LIST_HELPERS()
	
	static std::string getTableName() {
		return "Takeoff";
	}
	
	ORM_FIELD_METADATA(Takeoff, TAKEOFF_FIELDS)
	ORM_FIELD_ACCESSOR(Takeoff, TAKEOFF_FIELDS)
};
