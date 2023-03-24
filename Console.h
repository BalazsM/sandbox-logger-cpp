#ifndef CONSOLE_H
#define CONSOLE_H

class Console
{
public:
	class VT100
	{
	public:
		static const char* CursorOn()
		{
			return "\x1B[?25h";
		}

		static const char* CursorOff()
		{
			return "\x1B[?25l";
		}

		static std::string CursorMoveTo(int column, int row)
		{
			std::string result = "\x1B[";
			result += column;
			result += ';';
			result += row;
			result += 'H';
			return result;
		}

		static const char* CursorMoveToHome()
		{
			return "\x1B[1;1H";
		}

		static const char* TextColorDefault()
		{
			return "\x1B[39m";
		}

		static const char* TextColorBlack()
		{
			return "\x1B[30m";
		}

		static const char* TextColorRed()
		{
			return "\x1B[31m";
		}

		static const char* TextColorGreen()
		{
			return "\x1B[32m";
		}

		static const char* TextColorYellow()
		{
			return "\x1B[33m";
		}

		static const char* TextColorBlue()
		{
			return "\x1B[34m";
		}

		static const char* TextColorMagenta()
		{
			return "\x1B[35m";
		}

		static const char* TextColorCyan()
		{
			return "\x1B[36m";
		}

		static const char* TextColorWhite()
		{
			return "\x1B[37m";
		}

		static const char* TextColorGray()
		{
			return "\x1B[90m";
		}

		static const char* TextColorBrightRed()
		{
			return "\x1B[91m";
		}

		static const char* TextColorBrightGreen()
		{
			return "\x1B[92m";
		}

		static const char* TextColorBrightYellow()
		{
			return "\x1B[93m";
		}

		static const char* TextColorBrightBlue()
		{
			return "\x1B[94m";
		}

		static const char* TextColorBrightMagenta()
		{
			return "\x1B[95m";
		}

		static const char* TextColorBrightCyan()
		{
			return "\x1B[96m";
		}

		static const char* TextColorBrightWhite()
		{
			return "\x1B[97m";
		}
	};

	enum class Color : unsigned char
	{
		Black = 0,
		Red = 1,
		Green = 2,
		Yellow = 3,
		Blue = 4,
		Magenta = 5,
		Cyan = 6,
		White = 7,

		Gray = 8,
		BrightRed = 9,
		BrightGreen = 10,
		BrightYellow = 11,
		BrightBlue = 12,
		BrightMagenta = 13,
		BrightCyan = 14,
		BrightWhite = 15,
	};

	void Start();
	void Stop();
};

#ifdef CONSOLE_CPP
Console console;
#else
extern Console console;
#endif

#endif