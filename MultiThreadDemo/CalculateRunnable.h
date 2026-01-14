#pragma once
#include <QRunnable>
#include <QObject>

#include "takeoff.h"
#include "climb.h"

namespace ThreadDemo
{

	/*
	 *	QRunnable继承QObject可实现信号槽
	 */
	class CalculateRunnable : public QObject, public QRunnable
	{
		Q_OBJECT
	public:
		explicit CalculateRunnable(const int aVectorSize, QObject* parent = nullptr);
		~CalculateRunnable();

		// 获取计算结果
		Takeoff getTakeResult() const { return m_TakeResult; }
		Climb getClimbResult() const { return m_ClimbResult; }

	signals:
		void runnableFinishedSignal();

	protected:
		void run() override;

	private:
		int m_VectorSize;
		Takeoff m_TakeResult;
		Climb m_ClimbResult;
	};

}
