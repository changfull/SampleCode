#pragma once
#include <vector>
#include <map>
#include "CsvRow.h"
#include "Serialization.h"

class CsvSheet
{
public:
	std::vector<CsvRow> _sheet;
	std::vector<CsvRow>::iterator _iter_sheet;
	SERIALIZE(_sheet);

public:
	CsvSheet();
	~CsvSheet();

	bool LoadFromCsvFile(std::wstring file_name);

	CsvRow* Begin();
	CsvRow* Next();
};

