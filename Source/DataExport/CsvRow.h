#pragma once
#include <unordered_map>
#include "Serialization.h"

class CsvRow
{
public:
	std::unordered_map<std::wstring, std::wstring> _data_map;
	SERIALIZE(_data_map);

public:
	CsvRow(std::size_t reserve_cnt = 1);
	~CsvRow();

	bool Add(std::wstring key, std::wstring data);
	std::wstring GetData(std::wstring key);
};

