#pragma once
#include <string>
#include <map>
#include "CsvSheet.h"

class DataInfoBase;

enum DataType
{
	CharacterInfo,
	Count
};

class CharacterData;

class DataManager
{
private:
	std::map<int, DataInfoBase*> _data_map[DataType::Count];

public:
	DataManager();
	~DataManager();

	bool Load(std::wstring file_name);
	DataInfoBase* CreateDataInfo(DataType type);

	DataInfoBase* FindDataInfoBase(DataType type, int key);
	CharacterData* GetDataInfoHero(int key) { return (CharacterData*)FindDataInfoBase(CharacterInfo,key); }
	std::size_t GetDataInfoSize(DataType type);
};

#define GetDataMgr() Singleton<DataManager>::GetInstance()