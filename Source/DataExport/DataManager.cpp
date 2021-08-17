#include "DataManager.h"

#include <iostream>
#include <fstream>
#include <cstring>

#include "DataInfoBase.h"
#include "Util.h"
#include "CsvFileManager.h"
#include "CsvSheet.h"
#include "CharacterData.h"
#include "SafeDelete.hpp"

DataManager::DataManager()
{
}

DataManager::~DataManager()
{
	for (int i = 0; i < DataType::Count; ++i)
	{
		for (auto iter = _data_map[i].begin(); iter != _data_map[i].end(); ++iter)
		{
			SafeDelete(iter->second);
		}

		_data_map[i].clear();
	}
}

bool DataManager::Load(std::wstring file_name)
{
	CsvFileManager file_manager;
	if (false == file_manager.LoadBinary(file_name))
		return false;

	for (int i = 0; i < DataType::Count; ++i)
	{
		CsvSheet& sheet = file_manager.GetSheet(i);
		for (CsvRow* row = sheet.Begin(); row != nullptr; row = sheet.Next())
		{
			DataInfoBase* data_info_base = CreateDataInfo((DataType)i);
			
			data_info_base->Load(row);

			_data_map[i].emplace(std::make_pair(data_info_base->GetKey(), data_info_base));
		}
	}

	return true;
}

DataInfoBase* DataManager::CreateDataInfo(DataType type)
{
	switch (type)
	{
	case DataType::CharacterInfo: return new CharacterData();
		break;
	}

	return nullptr;
}

DataInfoBase* DataManager::FindDataInfoBase(DataType type, int key)
{
	auto iter = _data_map[type].find(key);
	if (iter == _data_map[type].end())
		return nullptr;

	return iter->second;
}

std::size_t DataManager::GetDataInfoSize(DataType type)
{
	return _data_map[type].size();
}