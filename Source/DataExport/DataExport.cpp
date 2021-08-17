#include "CsvFileManager.h"
#include "DataManager.h"
#include "CharacterData.h"

int wmain(int /*argc*/, wchar_t* /*argv[]*/)
{
	CsvFileManager file_manager;

	// csv 데이터 형식 파일 로드
	if (false == file_manager.LoadFromCsvFile(L"test.csv"))
		return 0;

	// 바이너리로 저장
	file_manager.SaveBinary(L"test.data");

	DataManager* data_manager = GetDataMgr();
	
	// 바이너리를 로드해서 사용자 정의 형식 데이터로 변환
	data_manager->Load(L"test.data");
	
	// 데이터 출력
	for (std::size_t i = 1; i < data_manager->GetDataInfoSize(DataType::CharacterInfo); ++i)
	{
		CharacterData* data = dynamic_cast<CharacterData*>(data_manager->FindDataInfoBase(DataType::CharacterInfo, i));
		if (data)
		{
			printf("key: %d level: %d hp: %d speed: %d\n", data->_key, data->_level, data->_hp, data->_speed);
		}
	}

	printf("complete.. \n");
	return getchar();
}
