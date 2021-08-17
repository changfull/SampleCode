#include "CsvFileManager.h"
#include "CsvSheet.h"
#include "SafeDelete.hpp"

CsvFileManager::CsvFileManager()
{
}

CsvFileManager::~CsvFileManager()
{
}

bool CsvFileManager::LoadFromCsvFile(std::wstring file_name)
{
	CsvSheet sheet;
	if (false == sheet.LoadFromCsvFile(file_name))
		return false;

	_sheet_vec.emplace_back(sheet);

	return true;
}

CsvSheet& CsvFileManager::GetSheet(int index)
{
	return _sheet_vec[index];
}

bool CsvFileManager::SaveBinary(std::wstring file_name)
{
	Archive<MAX_BINARY_SIZE> ar;
	ar << *this;

	std::ofstream fs;

	fs.open(file_name, std::ios::binary);
	if ( ! fs.is_open())
	{
		//LOG_ERROR("%S file open fail.", file_name.c_str());

		return false;
	}

	fs.write(ar.GetBuf(), ar.GetSize());
	fs.close();

	return true;
}

bool CsvFileManager::LoadBinary(std::wstring file_name)
{
	std::ifstream fs;
	fs.open(file_name, std::ios::binary);
	if ( ! fs.is_open())
	{
		printf("%S file open fail.", file_name.c_str());

		return false;
	}

	fs.seekg(0, std::ios::end);
	std::size_t length = static_cast<std::size_t>(fs.tellg());
	fs.seekg(0, std::ios::beg);

	char* buf = new char[length];
	
	fs.read(buf, length);
	fs.close();

	Archive<0> ar(buf, length);
	ar >> *this;

	SafeArrayDelete(buf);

	return true;
}