#ifndef LOGGER_H
#define LOGGER_H

#include <list>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <windows.h>

#include "console.h"

class Logger
{ 
public:
	enum class Level : unsigned char
	{
		Debug = 0,
		Info = 1,
		Warning = 2,
		Error = 3,
		Fatal = 4
	};

	class Entry
	{
	public:
		Entry(Logger& logger_, Level level_, const char* tag_)
			:
			logger(logger_),
			level(level_),
			file(nullptr),
			line(0),
			tag(tag_)
		{
			std::time(&time);
		}

		~Entry();

		Level GetLevel() const
		{
			return level;
		}

		Entry& SetFileLine(const char* file_, int line_)
		{
			file = file_;
			line = line_;
			return *this;
		}

		Entry& operator<<(char data)
		{
			message.push_back(std::string(1, data));
			return *this;
		}

		Entry& operator<<(const char* data)
		{
			message.push_back(data);
			return *this;
		}

		Entry& operator<<(const std::string& data)
		{
			message.push_back(data);
			return *this;
		}

		Entry& operator<<(int data)
		{
			message.push_back(std::to_string(data));
			return *this;
		}

		template<typename Formatter>
		std::string Format() const;

	private:
		Logger& logger;
		std::time_t time;
		Level level;
		const char* file;
		int line;
		const char* tag;
		std::list<std::string> message;
	};

	template<Logger::Level level>
	class EntryFactory
	{
	public:
		EntryFactory(Logger& logger_)
			:
			logger(logger_)
		{}

		Logger::Entry operator[] (const char* tag)
		{
			return Logger::Entry(logger, level, tag);
		}

		Logger::Entry operator() (const char* file, int line, const char* tag)
		{
			return Logger::Entry(logger, level, tag);
		}

	private:
		Logger& logger;
	};

	class Writer
	{
	public:
		virtual void Start() = 0;
		virtual void Stop() = 0;

		virtual void Write(const Entry& entry) = 0;
	};

	class DebugWriter : public Writer
	{
	public:
		void Start();
		void Stop();

		void Write(const Entry& entry);
	};

	class TextFormatter
	{
	public:
		static void AddSeparator(std::stringstream& stream)
		{
			stream << '\t';
		};

		static void AddEntrySeparator(std::stringstream& stream)
		{
			stream << "\r\n";
		}

		static void AddTime(std::stringstream& stream)
		{
			stream << "DateTime";
		};

		static void AddDateTime(std::stringstream& stream)
		{
			stream << "DateTime";
		};

		static void AddFileLine(std::stringstream& stream, const char* file, int line)
		{
			stream << file << '(' << line << ')';
		}

		static void AddLevel(std::stringstream& stream, Level level)
		{
			switch (level)
			{
			case Level::Debug:
				stream << "Debug";
				break;
			case Level::Info:
				stream << "Info";
				break;
			case Level::Warning:
				stream << "Warning";
				break;
			case Level::Error:
				stream << "Error";
				break;
			case Level::Fatal:
				stream << "Fatal";
				break;
			}
		}

		static void AddTag(std::stringstream& stream, const char* tag)
		{
			stream << tag;
		}

		static void AddTag(std::stringstream& stream, const std::string& tag)
		{
			stream << tag;
		}

		static void AddMessage(std::stringstream& stream, const char* message)
		{
			stream << message;
		}

		static void AddMessage(std::stringstream& stream, const std::string& message)
		{
			stream << message;
		}
	};

	class ConsoleFormatter : public TextFormatter
	{
	public:
		enum Style
		{
			Primary,
			Secondary,
			Success,
			Danger,
			Warning,
			Muted,
		};

		static void AddDateTime(std::stringstream& stream)
		{
			SetStyle(stream, Style::Muted);
			TextFormatter::AddTime(stream);
		};

		static void AddFileLine(std::stringstream& stream, const char* file, int line)
		{
			SetStyle(stream, Style::Primary);
			TextFormatter::AddFileLine(stream, file, line);
		}

		static void AddLevel(std::stringstream& stream, Level level)
		{
			switch (level)
			{
			case Level::Debug:
				SetStyle(stream, Style::Muted);
				break;
			case Level::Info:
				SetStyle(stream, Style::Secondary);
				break;
			case Level::Warning:
				SetStyle(stream, Style::Warning);
				break;
			case Level::Error:
				SetStyle(stream, Style::Danger);
				break;
			case Level::Fatal:
				SetStyle(stream, Style::Danger);
				break;
			}
			TextFormatter::AddLevel(stream, level);
		}

		static void AddTag(std::stringstream& stream, const char* tag)
		{
			SetStyle(stream, Style::Primary);
			TextFormatter::AddTag(stream, tag);
		}

		static void AddMessage(std::stringstream& stream, const char* message)
		{
			SetStyle(stream, Style::Primary);
			TextFormatter::AddMessage(stream, message);
		}

		static void AddMessage(std::stringstream& stream, const std::string& message)
		{
			SetStyle(stream, Style::Primary);
			TextFormatter::AddMessage(stream, message);
		}

		static void SetStyle(std::stringstream& stream, Style style)
		{
			switch(style)
			{
			case Primary:
				stream << Console::VT100::TextColorWhite();
				break;
			case Secondary:
				stream << Console::VT100::TextColorBrightWhite();
				break;
			case Success:
				stream << Console::VT100::TextColorGreen();
				break;
			case Danger:
				stream << Console::VT100::TextColorRed();
				break;
			case Warning:
				stream << Console::VT100::TextColorYellow();
				break;
			case Muted:
				stream << Console::VT100::TextColorGray();
				break;
			}
		}
	};

	class ConsoleWriter : public Writer
	{
	public:
		void Start();
		void Stop();

		void Write(const Entry& entry);
	};

	class FileWriter : public Writer
	{
	public:
		FileWriter()
		{
			SetPathFormat("{env:temp}/logger-{now:yyyy-MM-dd}.log");
		}

		void Start();
		void Stop();

		void Write(const Entry& entry);

		void SetPathFormat(std::string filePathFormat_)
		{
			filePathFormat = filePathFormat_;
		}

	private:
		std::string filePathFormat;
	};

	class SocketWriter : public Writer
	{
	public:
		void Start();
		void Stop();

		void Write(const Entry& entry);
	};

	class Rule
	{
	public:
		Rule(Level minLevel_, Writer* writer_)
			:
			minLevel(minLevel_),
			writer(writer_)
		{
		}

		void Apply(const Entry& entry) const;

		Level minLevel;
		Writer* writer;
	};

	EntryFactory<Level::Debug> debug;
	EntryFactory<Level::Info> info;
	EntryFactory<Level::Warning> warning;
	EntryFactory<Level::Error> error;
	EntryFactory<Level::Fatal> fatal;

	std::list<Rule> rules;

	Logger()
		:
		debug(*this),
		info(*this),
		warning(*this),
		error(*this),
		fatal(*this)
	{}

private:
};

// ________________________________________________________________  entry  __

Logger::Entry::~Entry()
{
//	writerThread.PushEntry();
	// TODO: push into writer thread's queue
	for (Logger::Rule& rule : logger.rules)
	{
		rule.Apply(*this);
	}
}

template<typename Formatter>
std::string Logger::Entry::Format() const
{
	std::stringstream result;

	std::tm t;
	localtime_s(&t, &time);
	//result << std::put_time(&t, "%Y-%m-%d %OH:%OM:%OS");
	Formatter::AddDateTime(result);
	Formatter::AddSeparator(result);

	if (file != nullptr) {
		Formatter::AddFileLine(result, file, line);
		Formatter::AddSeparator(result);
	}

	Formatter::AddLevel(result, level);
	Formatter::AddSeparator(result);

	Formatter::AddTag(result, tag);
	Formatter::AddSeparator(result);

	for (auto m : message)
	{
		Formatter::AddMessage(result, m);
	}
	Formatter::AddEntrySeparator(result);

	return result.str();
}

// _________________________________________________________________  rule  __

void Logger::Rule::Apply(const Entry& entry) const
{
	if (entry.GetLevel() < minLevel)
		return;
	writer->Write(entry);
}

// _________________________________________________________  debug writer  __

void Logger::DebugWriter::Start() 
{
}

void Logger::DebugWriter::Stop() 
{
}

void Logger::DebugWriter::Write(const Entry& entry)
{
	OutputDebugStringA(entry.Format<TextFormatter>().c_str());
}

// _______________________________________________________  console writer  __

void Logger::ConsoleWriter::Start()
{
}

void Logger::ConsoleWriter::Stop()
{
}

void Logger::ConsoleWriter::Write(const Entry& entry)
{
	std::cout << entry.Format<ConsoleFormatter>();
}

// __________________________________________________________  file writer  __

void Logger::FileWriter::Start()
{
}

void Logger::FileWriter::Stop()
{
}

void Logger::FileWriter::Write(const Entry& entry)
{
	// TODO: parse filaPathFormat and create real filePath
	std::string filePath = "logger-20230101.log";

	std::ofstream file;
	file.open(filePath, std::ios_base::app);
	file << entry.Format<TextFormatter>();
	file.close();
}

// _________________________________________________________  socket writer  __

void Logger::SocketWriter::Start()
{
}

void Logger::SocketWriter::Stop()
{
}

void Logger::SocketWriter::Write(const Entry& entry)
{
}

#endif