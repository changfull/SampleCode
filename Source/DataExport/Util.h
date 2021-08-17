#pragma once
#include <string>
#include <vector>
#include <assert.h>

class Util
{
public:

	static std::string GetCompileDateTime()
	{
		std::string out;
		out += __DATE__;
		out += " ";
		out += __TIME__;
		return out;
	}

	template <typename T>
	static T Clamp(T x, T low, T high)
	{
		using namespace std;
		assert(low <= high);
		return min(max(x, low), high);
	}

	static int Wrap(int x, int low, int high)
	{
		assert(low < high);
		const int n = (x - low) % (high - low);
		return (n >= 0 ? (n + low) : n + high);
	};

	static float Wrap(float x, float low, float high)
	{
		assert(low < high);
		const float n = std::fmod(x - low, high - low);
		return (n >= 0) ? (n + low) : (n + high);
	}

};

class StringUtil
{
public:
	template<typename T>
	static void Split(const std::basic_string<T>& line, const std::basic_string<T>& separators, std::vector<std::basic_string<T>>& tokens)
	{
		size_t start = line.find_first_not_of(separators);
		size_t end = line.length();

		while ((start >= 0) && (start < end))
		{
			size_t stop = line.find_first_of(separators, start);

			if ((stop < 0) || (stop > end))
			{
				stop = end;
			}

			tokens.emplace_back(line.substr(start, stop - start));

			start = line.find_first_not_of(separators, stop + 1);
		}
	}
};
