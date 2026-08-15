#pragma once
#include "Core/Common/CoreMinimal.h"

enum class LogLevel
{
	Info,
	Warning,
	Error,
	Debug // 3
};
struct LogEntry
{
	LogLevel level_;
	std::string message_;
};
class VICTORY_API Logger
{
public:
	static void Info(const std::string& message);
	static void Warning(const std::string& message);
	static void Error(const std::string& message);
	static void Debug(const std::string& message);

	static const std::vector<LogEntry>& GetLogEntries();
private:
	static std::vector<LogEntry> logEntries_;
};

