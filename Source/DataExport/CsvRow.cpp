#include "CsvRow.h"
#include <assert.h>
#include "Debug.h"

CsvRow::CsvRow(std::size_t reserve_cnt)
{
	_data_map.reserve(reserve_cnt);
}

CsvRow::~CsvRow()
{
	_data_map.clear();
}

bool CsvRow::Add(std::wstring key, std::wstring data)
{
	auto iter = _data_map.emplace(std::make_pair(key, data));
	if (false == iter.second)
	{
		//LOG_ERROR("Duplicatekey: %S Data: %S", key.c_str(), data.c_str());
		Verify(false);

		return false;
	}

	return true;
}

std::wstring CsvRow::GetData(std::wstring key)
{
	auto iter = _data_map.find(key);
	if (iter != _data_map.end())
	{
		return iter->second;
	}

	return L"";
}