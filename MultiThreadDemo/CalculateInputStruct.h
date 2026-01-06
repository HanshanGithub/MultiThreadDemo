#pragma once
#include <QString>

//! 传递给线程的输入结构体
typedef struct  
{
	QString msg;
	int threadMaxCount;
	int calculateCount;
}CalculateInputStruct; 
