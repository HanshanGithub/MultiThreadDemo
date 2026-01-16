// #include "MultiThreadDemo.h"
// #include <QtWidgets/QApplication>
// 
// int main(int argc, char* argv[])
// {
// 	QApplication a(argc, argv);
// 	ThreadDemo::MultiThreadDemo w;
// 	w.show();
// 	return a.exec();
// }

#include <iostream>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "database_manager.h"
#include "takeoff_table_handler.h"
#include "climb_table_handler.h"
#include "takeoff.h"
#include "climb.h"

// 创建示例Takeoff数据（参照CalculateRunnable::run()的实现）
Takeoff createSampleTakeoff(int index) {
	Takeoff record;

	// 基本参数
	record.height = 1000.0;
	record.weight = 4500.0;
	record.windSpeed = 0.0;
	record.temperature = 15.0;
	record.runwaySlope = 0.0;

	// 生成列表数据（10个时间点，0-9秒）
	record.timeList = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	record.groundSpeedList = { 0, 4, 5, 20, 34, 87, 129, 140, 189, 243 };
	record.machList = { 0.0, 0.001, 0.002, 0.006, 0.010, 0.026, 0.038, 0.041, 0.055, 0.071 };
	record.distList = { 0, 12, 31, 46, 76, 99, 145, 234, 436, 674 };
	record.heightList = { 0, 0, 0, 0, 0, 1, 2, 3, 5, 8 };
	record.angleOfAttackList = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	record.inclinationList = { 0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5 };
	record.accelerationList = { 2.0, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8, 2.9 };
	record.thrustList = { 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000 };
	record.frictionResistanceList = { 500, 500, 500, 500, 500, 500, 500, 500, 500, 500 };
	record.aerodynamicDragList = { 100, 120, 150, 200, 250, 300, 350, 400, 450, 500 };

	// 计算结果
	record.runDistance = 674.0;
	record.runTime = 9.0;
	record.obstacleCrossingDistance = 800.0;

	// 配置信息（包含时间戳和索引）
	record.config = " [Takeoff #" + std::to_string(index) + "]";

	return record;
}


// 创建示例Climb数据（参照CalculateRunnable::run()的实现）
Climb createSampleClimb(int index) {
	Climb record;

	// 基本参数
	record.indicatedSpeed = 250.0;
	record.weight = 4500.0;
	record.temperature = 15.0;
	record.startHeight = 0.0;
	record.endHeight = 10000.0;

	// 生成列表数据（10个时间点，0-9秒）
	record.timeList = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	record.speedList = { 200, 210, 220, 230, 240, 250, 260, 270, 280, 290 };
	record.heightList = { 0, 500, 1000, 2000, 3000, 4500, 6000, 7500, 8500, 10000 };
	record.horizontalDistanceList = { 0, 1200, 2500, 4000, 5800, 7800, 10000, 12500, 15000, 18000 };
	record.angleOfAttackList = { 5, 5.5, 6, 6.5, 7, 7.5, 8, 8.5, 9, 9.5 };
	record.climbAngleList = { 10, 12, 14, 16, 18, 20, 22, 24, 26, 28 };
	record.massList = { 4500, 4480, 4460, 4440, 4420, 4400, 4380, 4360, 4340, 4320 };
	record.machList = { 0.3, 0.32, 0.34, 0.36, 0.38, 0.40, 0.42, 0.44, 0.46, 0.48 };
	record.climbRateList = { 500, 550, 600, 650, 700, 750, 800, 850, 900, 950 };
	record.dynamicPressureList = { 5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500 };

	// 计算结果
	record.climbRate = 750.0;
	record.climbTime = 9.0;
	record.fuelConsumption = 180.0;
	record.horizontalDistance = 18000.0;

	// 配置信息（包含时间戳和索引）
	record.config = " [Climb #" + std::to_string(index) + "]";

	return record;
}

int main(int argc, char* argv[])
{
	std::cout << "=== Standard C++ Database Example ===" << std::endl;
	std::cout << "Reference: CalculateRunnable::run() and Calculate::startCalculate()" << std::endl;
	std::cout << std::endl;

	// Database path
	std::string dbPath = "wlx-cpp.db";

	// 1. Create database manager (reference: Calculate::startCalculate() line 43)
	DatabaseManager dbManager;

	// 2. Open database and set SQLCipher key (reference: Calculate::startCalculate() line 46)
	std::cout << "Opening database: " << dbPath << std::endl;
	if (!dbManager.open(dbPath, "123456")) {
		std::cerr << "[ERROR] Failed to open database" << std::endl;
		return 1;
	}
	std::cout << "Database opened successfully" << std::endl;

	// 3. Sync database schema (create tables) (reference: Calculate::startCalculate() line 51)
	std::cout << "Syncing database schema..." << std::endl;
	if (!dbManager.syncSchema()) {
		std::cerr << "[ERROR] Failed to sync schema" << std::endl;
		dbManager.close();
		return 1;
	}
	std::cout << "Schema synced successfully" << std::endl;

	// 4. Get table handlers (reference: Calculate::startCalculate() lines 55-56)
	TakeoffTableHandler* takeoffHandler = dbManager.getHandler<TakeoffTableHandler>();
	ClimbTableHandler* climbHandler = dbManager.getHandler<ClimbTableHandler>();

	if (!takeoffHandler || !climbHandler) {
		std::cerr << "[ERROR] Failed to get table handlers" << std::endl;
		dbManager.close();
		return 1;
	}
	std::cout << "Table handlers obtained successfully" << std::endl;
	std::cout << std::endl;

	// 5. Create sample data containers (reference: Calculate::startCalculate() lines 64-65)
	std::vector<Takeoff> takeoffRecords;
	std::vector<Climb> climbRecords;

	// 6. Generate multiple sample records (simulate multi-threaded calculation results)
	const int recordCount = 5;  // Create 5 records as example
	std::cout << "Creating " << recordCount << " Takeoff records..." << std::endl;
	for (int i = 0; i < recordCount; i++) {
		Takeoff record = createSampleTakeoff(i + 1);
		takeoffRecords.push_back(record);
		std::cout << "  Created Takeoff record #" << (i + 1) << ": " << record.config << std::endl;
	}

	std::cout << "Creating " << recordCount << " Climb records..." << std::endl;
	for (int i = 0; i < recordCount; i++) {
		Climb record = createSampleClimb(i + 1);
		climbRecords.push_back(record);
		std::cout << "  Created Climb record #" << (i + 1) << ": " << record.config << std::endl;
	}
	std::cout << std::endl;

	// 7. Batch insert Takeoff data (reference: Calculate::startCalculate() line 102)
	std::cout << "Batch inserting Takeoff records..." << std::endl;
	if (!takeoffHandler->insertBatch(takeoffRecords)) {
		std::cerr << "[ERROR] Failed to insert Takeoff records" << std::endl;
	}
	else {
		std::cout << "Successfully inserted " << takeoffRecords.size() << " Takeoff records" << std::endl;
	}

	// 8. Batch insert Climb data (reference: Calculate::startCalculate() line 107)
	std::cout << "Batch inserting Climb records..." << std::endl;
	if (!climbHandler->insertBatch(climbRecords)) {
		std::cerr << "[ERROR] Failed to insert Climb records" << std::endl;
	}
	else {
		std::cout << "Successfully inserted " << climbRecords.size() << " Climb records" << std::endl;
	}
	std::cout << std::endl;

	// 9. Verify: Query and display inserted data
	std::cout << "Verifying inserted data..." << std::endl;
	std::vector<Takeoff> allTakeoffs = takeoffHandler->getAll();
	std::vector<Climb> allClimbs = climbHandler->getAll();

	std::cout << "Total Takeoff records in database: " << allTakeoffs.size() << std::endl;
	std::cout << "Total Climb records in database: " << allClimbs.size() << std::endl;
	std::cout << std::endl;

	// 10. Close database (reference: Calculate::startCalculate() line 122)
	std::cout << "Closing database..." << std::endl;
	dbManager.close();
	std::cout << "Database closed" << std::endl;

	std::cout << std::endl;
	std::cout << "=== Example program completed ===" << std::endl;

	return 0;
}
