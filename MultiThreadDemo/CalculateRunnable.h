#pragma once
#include <QRunnable>
#include <QObject>

namespace ThreadDemo
{

	/*
	 *	QRunnable继承QObect可实现信号槽
	 */
	class CalculateRunnable : public QObject, public QRunnable
	{
		Q_OBJECT
	public:
		explicit CalculateRunnable(const int aVectorSize, QObject *parent = nullptr);
		~CalculateRunnable();

	signals:
		void runnableFinishedSignal();

	protected:
		void run() override;

	private:
		int m_VectorSize; 
	};

}