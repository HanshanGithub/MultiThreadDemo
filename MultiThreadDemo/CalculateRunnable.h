#pragma once
#include <QRunnable>
#include <QObject>
#include "takeoff.h"

namespace ThreadDemo
{

	/*
	 *	QRunnable继承QObject可实现信号槽
	 */
	class CalculateRunnable : public QObject, public QRunnable
	{
		Q_OBJECT
	public:
		explicit CalculateRunnable(const int aVectorSize, QObject *parent = nullptr);
		~CalculateRunnable();

		// 获取计算结果
		Takeoff getResult() const { return m_Result; }

	signals:
		void runnableFinishedSignal();

	protected:
		void run() override;

	private:
		int m_VectorSize;
		Takeoff m_Result;
	};

}