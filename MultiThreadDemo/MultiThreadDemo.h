#pragma once

#include <QtWidgets/QMainWindow>
#include "CalculateInputStruct.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MultiThreadDemoClass; };
QT_END_NAMESPACE

class QThread;

namespace ThreadDemo
{
	class Calculate;

	class MultiThreadDemo : public QMainWindow
	{
		Q_OBJECT
			using StringLists = QVector<QStringList>;
	public:
		MultiThreadDemo(QWidget* parent = nullptr);
		~MultiThreadDemo();

	signals:
		void startCalculateSignal(const CalculateInputStruct aInput);      //!< 让计算类开始计算
		void cancleCalculateSignal();                                      //!< 取消计算

	private slots:
		void btn_calculate_slot();
		void btn_cancel_calculate_slot();
		void calculate_finished_slot();

		void update_prossorbar_slot();

	private:
		Ui::MultiThreadDemoClass* ui;

		QThread*   m_Thread;          //!< 计算类执行的线程
		Calculate* m_Calculate;       //!< 计算类，放在mThread线程里面执行，避免界面卡死
		QString    m_StartTime;       //!< 记录开始计算时间
		bool       m_CalculateStatus; //!< 计算状态
	};
}
