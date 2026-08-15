#include "Logger.h"

void Logger::Info(const std::string& message) 
{
	LogEntry entry;
	entry.level_ = LogLevel::Info;
	entry.message_ = "[Info] " + message;

	logEntries_.push_back(entry);
}
void Logger::Warning(const std::string& message) 
{
	LogEntry entry;
	entry.level_ = LogLevel::Warning;
	entry.message_ = "[Warning] " + message;

	logEntries_.push_back(entry);
}
void Logger::Error(const std::string& message) 
{
	LogEntry entry;
	entry.level_ = LogLevel::Error;
	entry.message_ = "[Error] " + message;

	logEntries_.push_back(entry);
}
void Logger::Debug(const std::string& message) 
{
	LogEntry entry;
	entry.level_ = LogLevel::Debug;
	entry.message_ = "[Debug] " + message;

	logEntries_.push_back(entry);
}

const std::vector<LogEntry>& Logger::GetLogEntries() 
{
	return logEntries_;
}