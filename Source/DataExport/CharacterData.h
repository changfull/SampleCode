#pragma once
#include <string>
#include "DataInfoBase.h"
#include "CsvRow.h"

class CharacterData : public DataInfoBase
{
public:
	int _level;
	int _hp;
	int _speed;

public:
	CharacterData();
	~CharacterData();
	
	virtual bool Load(CsvRow* row);
};

