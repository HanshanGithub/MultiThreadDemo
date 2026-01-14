#include "Calculate.h"
#include <QThread>
#include <QThreadPool>
#include <QMutex>
#include <QMutexLocker>
#include <QDebug>

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
		QString dbPath = QDir::current().absoluteFilePath("wlx-sqlcipher.db");

		// 1. 创建数据库管理器（ORM 封装）
		DatabaseManager dbManager;

		// 2. 打开数据库并设置 SQLCipher 密钥
		if (!dbManager.open(dbPath.toStdString(), "123456")) {
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

		// 4. 创建数据对象容器，用于收集多线程计算结果
		std::vector<Takeoff> records;
		QMutex recordsMutex; // 保护 records 的互斥锁

		m_ThreadPool = new QThreadPool();
		m_ThreadPool->setMaxThreadCount(aInput.threadMaxCount); // 设置线程池的最大线程数

		// 存储所有 runnable 指针，用于收集结果（不能设置 autoDelete）
		std::vector<CalculateRunnable*> runnables;
		runnables.reserve(aInput.calculateCount);

		for (int i = 0; i < aInput.calculateCount; i++)
		{
			CalculateRunnable* runnable = new CalculateRunnable(aInput.calculateCount - i);
			runnable->setAutoDelete(false); // 不自动删除，需要手动收集结果后删除
			runnables.push_back(runnable);

			// 信号用于更新进度条
			connect(runnable, &CalculateRunnable::runnableFinishedSignal, this, [this, runnable, &records, &recordsMutex]() {
				emit updateProssorbarSignal();

				// 收集计算结果到 records（线程安全）
				QMutexLocker locker(&recordsMutex);
				records.push_back(runnable->getResult());
				},
				Qt::DirectConnection
			);

			m_ThreadPool->start(runnable);
		}
		m_ThreadPool->waitForDone(); // 等待所有任务完成

		// 5. 批量插入（使用 TableHandler）- 一次性插入所有结果
		if (!takeoffHandler || !takeoffHandler->insertBatch(records)) {
			qCritical() << "Failed to insert takeoff records";
		}

		// 手动删除所有 runnable
		for (auto* runnable : runnables) {
			delete runnable;
		}
		runnables.clear();

		emit calculateFinishedSignal();

		delete m_ThreadPool;
		m_ThreadPool = nullptr;

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
