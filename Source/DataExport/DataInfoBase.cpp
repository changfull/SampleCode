#include "DataInfoBase.h"

DataInfoBase::DataInfoBase()
{
}

DataInfoBase::~DataInfoBase()
{
}

bool DataInfoBase::Load(CsvRow* row)
{
	_key = stoi(row->GetData(L"Key"));
	return true;
}