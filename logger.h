#ifndef LOGGER_H
#define LOGGER_H

#include <list>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <windows.h>

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

				std::string Format() const
				{
					std::stringstream result;

					std::tm t;
					localtime_s(&t, &time);
					result << std::put_time(&t, "%Y-%m-%d %OH:%OM:%OS") << '\t';

					if (file != nullptr)
						result << file << '(' << line << ")\t";

					switch (level)
					{
						case Level::Debug:
							result << "Debug";
							break;
						case Level::Info:
							result << "Info";
							break;
						case Level::Warning:
							result << "Warning";
							break;
						case Level::Error:
							result << "Error";
							break;
						case Level::Fatal:
							result << "Fatal";
							break;
					}
					result << '\t';

					result << tag << '\t';

					for (auto m : message)
					{
						result << m;
					}
					result << std::endl;

					return result.str();
				}

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
	// TODO: start writer thread if not started yet
	// writerThread.Start();
}

void Logger::DebugWriter::Stop() 
{
	// writerThread.Stop();
}

void Logger::DebugWriter::Write(const Entry& entry)
{
	OutputDebugStringA(entry.Format().c_str());
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
	std::cout << entry.Format();
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
	file << entry.Format();
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