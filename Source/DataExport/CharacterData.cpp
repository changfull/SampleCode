#include "CharacterData.h"

CharacterData::CharacterData()
	: DataInfoBase()
{
}

CharacterData::~CharacterData()
{
}

bool CharacterData::Load(CsvRow* row)
{
	if (false == __super::Load(row))
		return false;

	_level = stoi(row->GetData(L"level"));
	_hp = stoi(row->GetData(L"hp"));
	_speed = stoi(row->GetData(L"speed"));

	return true;
}
