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
		m_Result.height = 1000.0;
		m_Result.weight = 4500.0;
		m_Result.windSpeed = 0.0;
		m_Result.temperature = 15.0;
		m_Result.runwaySlope = 0.0;

		// 生成列表数据（10个时间点，0-9秒）
		m_Result.timeList = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		m_Result.groundSpeedList = { 0, 4, 5, 20, 34, 87, 129, 140, 189, 243 };
		m_Result.machList = { 0.0, 0.001, 0.002, 0.006, 0.010, 0.026, 0.038, 0.041, 0.055, 0.071 };
		m_Result.distList = { 0, 12, 31, 46, 76, 99, 145, 234, 436, 674 };
		m_Result.heightList = { 0, 0, 0, 0, 0, 1, 2, 3, 5, 8 };
		m_Result.angleOfAttackList = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		m_Result.inclinationList = { 0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5 };
		m_Result.accelerationList = { 2.0, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8, 2.9 };
		m_Result.thrustList = { 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000 };
		m_Result.frictionResistanceList = { 500, 500, 500, 500, 500, 500, 500, 500, 500, 500 };
		m_Result.aerodynamicDragList = { 100, 120, 150, 200, 250, 300, 350, 400, 450, 500 };

		m_Result.runDistance = 674.0;
		m_Result.runTime = 9.0;
		m_Result.obstacleCrossingDistance = 800.0;

		auto test_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
		m_Result.config = test_time.toStdString();

		emit runnableFinishedSignal();
	}

}
