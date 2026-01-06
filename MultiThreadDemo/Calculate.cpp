#include "Calculate.h"
#include <QThread>
#include <QThreadPool>

#include "CalculateRunnable.h"

#include <QDir>
#include "database_manager.h"
#include "takeoff.h"
#include "takeoff_table_handler.h"
#include <QDateTime>

namespace ThreadDemo
{

	Calculate::Calculate(QObject* parent/*=nullptr*/)
	{
		m_ThreadPool = nullptr;
	}

	void Calculate::directDoSomeThing()
	{
		//!@ 测试外部调用接口与槽函数响应是否在同一线程中
		auto test_thread_id = QThread::currentThreadId(); 
		int i = 1;
	}

	Calculate::~Calculate()
	{

	}

	void Calculate::startCalculate(const CalculateInputStruct aInput)
	{
		QString dbPath = QDir::current().absoluteFilePath("wlx.db");

		// 1. 创建数据库管理器（ORM 封装）
		DatabaseManager dbManager;

		// 2. 打开数据库并设置 SQLCipher 密钥
		if (!dbManager.open(dbPath, "123456")) {
			qCritical() << "Failed to open database";
		}

		// 3. 同步数据库架构（创建表）
		if (!dbManager.syncSchema()) {
			qCritical() << "Failed to sync schema";
			dbManager.close();
		}
		TakeoffTableHandler* takeoffHandler = dbManager.getHandler<TakeoffTableHandler>();

// 		// 4. 创建数据对象（ORM 实体）- 基于实际数据示例
// 		std::vector<Takeoff> records;

		auto test_thread_id = QThread::currentThreadId(); // 查看当前线程的id和界面类、任务类的id是否相同

		m_ThreadPool = new QThreadPool();
		m_ThreadPool->setMaxThreadCount(aInput.threadMaxCount); // 设置线程池的最大线程数为1
		for (int i = 0; i < aInput.calculateCount; i++)
		{
			CalculateRunnable* runnable = new CalculateRunnable(aInput.calculateCount - i);
			runnable->setAutoDelete(true); // 设置任务结束后自动删除
			m_ThreadPool->start(runnable);

			//! 设置响应方式为消息队列时，会在emit CalculateFinished(); 后再响应，故这里使用直接响应
			connect(runnable, &CalculateRunnable::runnableFinishedSignal, this, [&]() {
				emit updateProssorbarSignal();

				Takeoff record;
				record.height = 300.0;
				record.weight = 20000.0;
				record.windSpeed = 10.0;
				record.temperature = 0.0;
				record.runwayLength = 3000.0;

				// 生成列表数据（10个时间点，0-9秒）
				// 直接使用 QList<double>，ORM 会自动序列化
				record.slipTimeList = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
				record.liftOffHeightList = { 0, 0, 0, 0, 0, 1, 2, 3, 5, 8 };
				record.slipDistanceList = { 0, 12, 31, 46, 76, 99, 145, 234, 436, 674 };
				record.airspeedList = { 0, 4, 5, 20, 34, 87, 129, 140, 189, 243 };

				record.frontWheelLiftOffSpeed = 87.0;
				record.rearWheelLiftOffSpeed = 243.0;

				auto test_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
				record.engineState = test_time.toStdString();

// 				records.push_back(record);

				if (!takeoffHandler || !takeoffHandler->insert(record)) {
					qCritical() << "Failed to insert takeoff records";
				}

				}, 
				Qt::DirectConnection
			);
		}
		m_ThreadPool->waitForDone(); // 等待所有任务完成
		emit calculateFinishedSignal();

		delete m_ThreadPool;
		m_ThreadPool = nullptr;


		// 5. 批量插入（使用 TableHandler）
// 		TakeoffTableHandler* takeoffHandler = dbManager.getHandler<TakeoffTableHandler>();
// 		if (!takeoffHandler || !takeoffHandler->insertBatch(records)) {
// 			qCritical() << "Failed to insert takeoff records";
// 			dbManager.close();
// 		}
// 
		dbManager.close();

	}

	void Calculate::cancelCalculate()
	{
		if (m_ThreadPool)
		{
			m_ThreadPool->clear(); // 清空线程池
// 		delete mThreadPool;
// 		mThreadPool = nullptr;
		}
	}
}