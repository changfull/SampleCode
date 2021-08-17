#include <iostream>
#include <fstream>
#include <codecvt>
#include "CsvSheet.h"
#include "Util.h"

CsvSheet::CsvSheet()
{
}

CsvSheet::~CsvSheet()
{
	_sheet.clear();
}

bool CsvSheet::LoadFromCsvFile(std::wstring file_name)
{
	std::wifstream fs(file_name, std::ios::in);

	if (!fs.is_open())
	{
		//LOG_ERROR("%s file open fail.", file_name.c_str());

		return false;
	}

	// utf8 형식 읽기 c++17에서 지원 안함
	//fs.imbue(std::locale(fs.getloc(), new std::codecvt_utf8<wchar_t, 0x10ffff, (std::codecvt_mode)(std::consume_header)>)/*자동 해제됨*/);
	// ansi
	//fs.imbue(std::locale("kor"));
	// utf-8
	fs.imbue(std::locale("kor.UTF-8"));

	std::wstring line;
	getline(fs, line);
	
	std::vector<std::wstring> column_name_vec;
	StringUtil::Split<wchar_t>(line, L",", column_name_vec);

	int column_name_index = 0;

	while (getline(fs, line))
	{
		CsvRow row(column_name_vec.size());

		std::vector<std::wstring> tokens;
		StringUtil::Split<wchar_t>(line, L",", tokens);

		for (auto iter = tokens.begin(); iter != tokens.end(); ++iter)
		{
			row.Add(column_name_vec[column_name_index++], *iter);
		}

		column_name_index = 0;
		_sheet.emplace_back(row);
	}

	column_name_vec.clear();
	fs.close();

	return true;
}

CsvRow* CsvSheet::Begin()
{
	if (_sheet.empty())
		return nullptr;

	_iter_sheet = _sheet.begin();

	return &(*_iter_sheet);
}

CsvRow* CsvSheet::Next()
{
	++_iter_sheet;

	if (_iter_sheet == _sheet.end())
		return nullptr;

	return &(*_iter_sheet);
}