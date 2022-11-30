#include "CSaveData.h"
#include <Windows.h>
#include <stdio.h>

#define SAVEFILENAME (L"SaveData")

CSaveData* CSaveData::m_pInstance = 0;

void CSaveData::Create()
{
	if (m_pInstance != NULL)
	{
		MessageBox(NULL, L"セーブデータクラス作成失敗", L"エラー", MB_OK);
		return;
	}

	m_pInstance = new CSaveData;
}

CSaveData* CSaveData::GetInstance()
{
	if (m_pInstance == NULL)
	{
		MessageBox(NULL, L"セーブデータクラスが生成されていません。", L"エラー", MB_OK);
		return NULL;
	}

	return m_pInstance;
}

void CSaveData::Destory()
{
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

// 初期化
void CSaveData::Init(void* pData, int nDataSize)
{
	m_pData = pData;
	m_nDataSize = nDataSize;
}

// データをセーブ
void CSaveData::Save()
{
	FILE* file;
	_wfopen_s(&file, SAVEFILENAME, L"wb");

	fwrite(m_pData, m_nDataSize, 1, file);

	fclose(file);
}

// データをロード
void CSaveData::Load()
{
	FILE* file;
	if (_wfopen_s(&file, SAVEFILENAME, L"rb") != 0)
		return;

	fread(m_pData, m_nDataSize, 1, file);

	fclose(file);
}
