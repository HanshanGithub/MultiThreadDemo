#include "CalculateRunnable.h"
#include <QThread>
#include <QDateTime>

namespace ThreadDemo
{

	CalculateRunnable::CalculateRunnable(const int aVectorSize, QObject* parent /*= nullptr*/)
	{
		if (aVectorSize <= 0)
			m_VectorSize = 1;
		else
			m_VectorSize = aVectorSize;
	}

	CalculateRunnable::~CalculateRunnable()
	{
		auto test_thread_id = QThread::currentThreadId();
		int i = 1;
	}

	void CalculateRunnable::run()
	{
		// 在工作线程中执行计算任务
		m_TakeResult.height = 1000.0;
		m_TakeResult.weight = 4500.0;
		m_TakeResult.windSpeed = 0.0;
		m_TakeResult.temperature = 15.0;
		m_TakeResult.runwaySlope = 0.0;

		// 生成列表数据（10个时间点，0-9秒）
		m_TakeResult.timeList = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		m_TakeResult.groundSpeedList = { 0, 4, 5, 20, 34, 87, 129, 140, 189, 243 };
		m_TakeResult.machList = { 0.0, 0.001, 0.002, 0.006, 0.010, 0.026, 0.038, 0.041, 0.055, 0.071 };
		m_TakeResult.distList = { 0, 12, 31, 46, 76, 99, 145, 234, 436, 674 };
		m_TakeResult.heightList = { 0, 0, 0, 0, 0, 1, 2, 3, 5, 8 };
		m_TakeResult.angleOfAttackList = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		m_TakeResult.inclinationList = { 0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5 };
		m_TakeResult.accelerationList = { 2.0, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8, 2.9 };
		m_TakeResult.thrustList = { 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000 };
		m_TakeResult.frictionResistanceList = { 500, 500, 500, 500, 500, 500, 500, 500, 500, 500 };
		m_TakeResult.aerodynamicDragList = { 100, 120, 150, 200, 250, 300, 350, 400, 450, 500 };

		m_TakeResult.runDistance = 674.0;
		m_TakeResult.runTime = 9.0;
		m_TakeResult.obstacleCrossingDistance = 800.0;

		auto test_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
		m_TakeResult.config = test_time.toStdString();


		// 在工作线程中执行爬升计算任务
		m_ClimbResult.indicatedSpeed = 250.0;
		m_ClimbResult.weight = 4500.0;
		m_ClimbResult.temperature = 15.0;
		m_ClimbResult.startHeight = 0.0;
		m_ClimbResult.endHeight = 10000.0;

		// 生成列表数据（10个时间点，0-9秒）
		m_ClimbResult.timeList = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		m_ClimbResult.speedList = { 200, 210, 220, 230, 240, 250, 260, 270, 280, 290 };
		m_ClimbResult.heightList = { 0, 500, 1000, 2000, 3000, 4500, 6000, 7500, 8500, 10000 };
		m_ClimbResult.horizontalDistanceList = { 0, 1200, 2500, 4000, 5800, 7800, 10000, 12500, 15000, 18000 };
		m_ClimbResult.angleOfAttackList = { 5, 5.5, 6, 6.5, 7, 7.5, 8, 8.5, 9, 9.5 };
		m_ClimbResult.climbAngleList = { 10, 12, 14, 16, 18, 20, 22, 24, 26, 28 };
		m_ClimbResult.massList = { 4500, 4480, 4460, 4440, 4420, 4400, 4380, 4360, 4340, 4320 };
		m_ClimbResult.machList = { 0.3, 0.32, 0.34, 0.36, 0.38, 0.40, 0.42, 0.44, 0.46, 0.48 };
		m_ClimbResult.climbRateList = { 500, 550, 600, 650, 700, 750, 800, 850, 900, 950 };
		m_ClimbResult.dynamicPressureList = { 5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500 };

		m_ClimbResult.climbRate = 750.0;
		m_ClimbResult.climbTime = 9.0;
		m_ClimbResult.fuelConsumption = 180.0;
		m_ClimbResult.horizontalDistance = 18000.0;

		auto test_time_climb = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
		m_ClimbResult.config = test_time_climb.toStdString();

		emit runnableFinishedSignal();
	}

}
