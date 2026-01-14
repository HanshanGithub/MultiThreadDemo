#include "MultiThreadDemo.h"
#include "ui_MultiThreadDemo.h"

#include <QThread>
#include <QDateTime>
#include <QRegExpValidator>
#include "Calculate.h"
#include "CalculateInputStruct.h"

namespace ThreadDemo
{

	MultiThreadDemo::MultiThreadDemo(QWidget* parent)
		: QMainWindow(parent)
		, ui(new Ui::MultiThreadDemoClass())
	{
		m_CalculateStatus = false;
		ui->setupUi(this);

		connect(ui->btn_calculate, &QPushButton::clicked, this, &MultiThreadDemo::btn_calculate_slot);
		connect(ui->btn_cancel_calculate, &QPushButton::clicked, this, &MultiThreadDemo::btn_cancel_calculate_slot);

		m_Thread = new QThread();
		m_Calculate = new Calculate();
		m_Calculate->moveToThread(m_Thread); // 改变mCalculate的线程依附关系，将计算类放在线程中执行

		//! 释放堆空间资源，避免内存泄露
		connect(m_Thread, &QThread::finished, m_Thread, &QObject::deleteLater);
		connect(m_Thread, &QThread::finished, m_Calculate, &QObject::deleteLater);

		//! 注意：在使用跨线程通信时，参数需要为元数据类型
		qRegisterMetaType<CalculateInputStruct>("CalculateInputStruct"); // 将结构体 CalculateInputStruct 注册为元数据类型

		//! 连接其他信号槽，用于触发线程执行槽函数里的任务    
		connect(this, &MultiThreadDemo::startCalculateSignal, m_Calculate, &Calculate::startCalculate, Qt::QueuedConnection); // 默认使用Qt::QueuedConnection，保证槽函数的执行顺序
		connect(this, &MultiThreadDemo::cancleCalculateSignal, m_Calculate, &Calculate::cancelCalculate, Qt::DirectConnection);

		connect(m_Calculate, &Calculate::calculateFinishedSignal, this, &MultiThreadDemo::calculate_finished_slot, Qt::QueuedConnection); // 计算完成显示信息
		connect(m_Calculate, &Calculate::updateProssorbarSignal, this, &MultiThreadDemo::update_prossorbar_slot, Qt::QueuedConnection); // 每计算一次完一次任务，更新界面进度条

		m_Thread->start(); // 启动线程，线程默认开启事件循环，并且线程正处于事件循环状态

		QRegExp regex("^[0-9.,:- ]*$");
		QRegExpValidator* validator = new QRegExpValidator(regex);
		ui->line_edit_calculate_count->setValidator(validator);
	}

	MultiThreadDemo::~MultiThreadDemo()
	{
		delete ui;
	}

	void MultiThreadDemo::btn_calculate_slot()
	{
		auto test_thread_id = QThread::currentThreadId(); // 查看当前线程的id和计算类的id是否相同

		//! 检查输入
		auto max_count_str = ui->line_edit_thread_max_count->text();
		if (max_count_str == "" || max_count_str.toInt() <= 0)
		{
			ui->calculate_message->append("计算最大线程数量需要大于0！");
			return;
		}

		auto calculate_count_str = ui->line_edit_calculate_count->text();
		if (calculate_count_str == "" || calculate_count_str.toInt() <= 0)
		{
			ui->calculate_message->append(u8"计算次数需要大于0！");
			return;
		}

		//! 判断是否在计算
		if (m_CalculateStatus)
		{
			ui->calculate_message->append(u8"正在计算，请稍后...");
			return;
		}
		m_CalculateStatus = true;

		//! 添加计算信息
		QString start_msg =
			QString(u8"本次计算次数 %1，单线程需要时间为 %3s，当前计算线程数量 %2\n")
			.arg(ui->line_edit_calculate_count->text())
			.arg(ui->line_edit_thread_max_count->text())
			.arg(ui->line_edit_calculate_count->text().toInt() * 0.5);
		start_msg += u8"开始计算，当前时间：" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
		ui->calculate_message->append(start_msg);

		//! 设定输入参数
		CalculateInputStruct input;
		input.msg = "start";
		input.threadMaxCount = max_count_str.toInt();
		input.calculateCount = calculate_count_str.toInt();

		ui->calculate_prossorbar->reset();
		ui->calculate_prossorbar->setRange(0, input.calculateCount);
		ui->calculate_prossorbar->setValue(0);

		emit startCalculateSignal(input);
		m_StartTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zz");
	}

	void MultiThreadDemo::btn_cancel_calculate_slot()
	{
		emit cancleCalculateSignal();
	}

	void MultiThreadDemo::calculate_finished_slot()
	{
		m_CalculateStatus = false;
		QString finished_msg = u8"计算完成，当前时间：" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
		ui->calculate_message->append(finished_msg);
		ui->calculate_message->append(u8"=========================");
		ui->calculate_message->append("");
		ui->calculate_message->append("");

		int i = 1;
	}

	void MultiThreadDemo::update_prossorbar_slot()
	{
		auto cur_num = ui->calculate_prossorbar->value();
		if (cur_num < ui->calculate_prossorbar->maximum())
		{
			ui->calculate_prossorbar->setValue(cur_num + 1);
		}
	}

}
