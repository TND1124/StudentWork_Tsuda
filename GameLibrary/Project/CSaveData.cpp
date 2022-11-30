#include "CSaveData.h"
#include <Windows.h>
#include <stdio.h>

#define SAVEFILENAME (L"SaveData")

CSaveData* CSaveData::m_pInstance = 0;

void CSaveData::Create()
{
	if (m_pInstance != NULL)
	{
		MessageBox(NULL, L"�Z�[�u�f�[�^�N���X�쐬���s", L"�G���[", MB_OK);
		return;
	}

	m_pInstance = new CSaveData;
}

CSaveData* CSaveData::GetInstance()
{
	if (m_pInstance == NULL)
	{
		MessageBox(NULL, L"�Z�[�u�f�[�^�N���X����������Ă��܂���B", L"�G���[", MB_OK);
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

// ������
void CSaveData::Init(void* pData, int nDataSize)
{
	m_pData = pData;
	m_nDataSize = nDataSize;
}

// �f�[�^���Z�[�u
void CSaveData::Save()
{
	FILE* file;
	_wfopen_s(&file, SAVEFILENAME, L"wb");

	fwrite(m_pData, m_nDataSize, 1, file);

	fclose(file);
}

// �f�[�^�����[�h
void CSaveData::Load()
{
	FILE* file;
	if (_wfopen_s(&file, SAVEFILENAME, L"rb") != 0)
		return;

	fread(m_pData, m_nDataSize, 1, file);

	fclose(file);
}
