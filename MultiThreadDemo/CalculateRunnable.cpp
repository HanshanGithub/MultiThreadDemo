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
		m_Result.height = 300.0;
		m_Result.weight = 20000.0;
		m_Result.windSpeed = 10.0;
		m_Result.temperature = 0.0;
		m_Result.runwayLength = 3000.0;

		// 生成列表数据（10个时间点，0-9秒）
		m_Result.slipTimeList = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		m_Result.liftOffHeightList = { 0, 0, 0, 0, 0, 1, 2, 3, 5, 8 };
		m_Result.slipDistanceList = { 0, 12, 31, 46, 76, 99, 145, 234, 436, 674 };
		m_Result.airspeedList = { 0, 4, 5, 20, 34, 87, 129, 140, 189, 243 };

		m_Result.frontWheelLiftOffSpeed = 87.0;
		m_Result.rearWheelLiftOffSpeed = 243.0;

		auto test_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
		m_Result.engineState = test_time.toStdString();

		emit runnableFinishedSignal();
	}

}