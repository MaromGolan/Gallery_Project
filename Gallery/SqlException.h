#pragma once
#include "MyException.h"

class SqlException : public MyException
{
public:
	SqlException(const char* msg) : MyException("SQL ERROR: " + std::string(msg)) {};
	SqlException(std::string msg) : MyException("SQL ERROR: " + msg) {};

};
