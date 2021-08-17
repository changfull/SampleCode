#pragma once
#include <string>
#include "CsvRow.h"

class DataInfoBase
{
public:
	int _key;

public:
	DataInfoBase();
	virtual ~DataInfoBase();
	virtual bool Load(CsvRow* row);

	int GetKey() { return _key; }
};

