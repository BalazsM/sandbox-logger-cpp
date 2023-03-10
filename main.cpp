// TODO: log into a socket
// TODO: create logger sandbox git repo
// TODO: filter by tag

#include <conio.h>
#include <stdlib.h>
#include "logger.h"

Logger logger;

template <typename T>
	class AutoStop : public T
{
	public:
		~AutoStop()
		{
			this->Stop();
		}
};

const char* GetTag(int index)
{
	/*
	Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor
	incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud
	exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor
	in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur
	sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id
	est laborum.
	*/

	const char* tags[] = {
		"lo", "rem", "ip", "sum", "do", "lor", "sit", "am", "et", "con", 
		"sec", "te", "tur", "el", "it", "sed", "tem", "por", "in", "ci", 
		"di", "du", "nt", "su", "cul", "pa", "ex", "is", "ris", "re", "non", 
		"off", "la", "bor", "id"
	};

	return tags[index % (sizeof(tags) / sizeof(const char*))];
}

std::string GenerateMessage(int seed)
{
	std::string result;

	std::srand(seed);

	const int s = 1 + rand() % 3;
	for (int k = 0; k < s; k++)
	{
		std::string firstTag = GetTag(rand());
		firstTag[0] = toupper(firstTag[0]);
		result.append(firstTag);

		const int l = 5 + rand() % 10;
		for (int i = 0; i < l; i++)
		{
			const int wl = 1 + rand() % 3;
			for (int j = 0; j < wl; j++)
				result.append(GetTag(rand()));

			if (i < l - 1)
				result.append(rand() % 100 < 10 ? ", " : " ");
			else
				result.append(". ");
		}
	}

	return result;
}

int main()
{
	AutoStop<Logger::DebugWriter> debugWriter;
	debugWriter.Start();
	logger.rules.push_back(Logger::Rule(Logger::Level::Debug, &debugWriter));
	logger.debug[__func__] << "first line of log";

	AutoStop<Logger::ConsoleWriter> consoleWriter;
	consoleWriter.Start();
	logger.rules.push_back(Logger::Rule(Logger::Level::Debug, &consoleWriter));

	AutoStop<Logger::FileWriter> fileWriter;
	fileWriter.Start();
	logger.rules.push_back(Logger::Rule(Logger::Level::Info, &fileWriter));

	AutoStop<Logger::SocketWriter> socketWriter;
	socketWriter.Start();
	logger.rules.push_back(Logger::Rule(Logger::Level::Info, &socketWriter));

	logger.debug[__func__] << "first line of log";
	logger.info[__func__] << "second " << "line of log";
	logger.warning[__func__] << "third " << "line of log " << 2;
	logger.error[__func__] << 4 << "th line of log";
	logger.fatal[__func__] << 5 << "th line of log";

	int seed = 1;
	bool exitSignal = false;
	while (!exitSignal)
	{
		std::srand(seed++);
		std::string message = GenerateMessage(std::rand());

		logger.debug[__func__].SetFileLine(__FILE__, __LINE__) << message;

		std::this_thread::sleep_for(std::chrono::seconds(1));

		if (_kbhit())
		{
			exitSignal = true;
		}
	}
}