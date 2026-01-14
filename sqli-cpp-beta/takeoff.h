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
	X(double, weight, "REAL", "(15000)", true) \
	X(double, windSpeed, "REAL", "(0)", true) \
	X(double, temperature, "REAL", "(15)", true) \
	X(double, runwayLength, "REAL", "(3000)", true) \
	LIST(std::vector<double>, slipTimeList, "TEXT", "", false) \
	LIST(std::vector<double>, liftOffHeightList, "TEXT", "", false) \
	LIST(std::vector<double>, slipDistanceList, "TEXT", "", false) \
	LIST(std::vector<double>, airspeedList, "TEXT", "", false) \
	X(double, frontWheelLiftOffSpeed, "REAL", "(0)", true) \
	X(double, rearWheelLiftOffSpeed, "REAL", "(0)", true) \
	X(std_string, engineState, "TEXT", "''", true)

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
