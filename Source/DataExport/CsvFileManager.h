#pragma once
#include <vector>
#include <string>
#include <fstream>
#include "Serialization.h"
#include "CsvSheet.h"

#define MAX_BINARY_SIZE 1024 * 128

class CsvFileManager
{
public:
	std::vector<CsvSheet> _sheet_vec;
	SERIALIZE(_sheet_vec);

public:
	CsvFileManager();
	~CsvFileManager();

	bool LoadFromCsvFile(std::wstring file_name);
	CsvSheet& GetSheet(int index);

	bool SaveBinary(std::wstring file_name);
	bool LoadBinary(std::wstring file_name);
};

