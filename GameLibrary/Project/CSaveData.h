#pragma once

// �Z�[�u�f�[�^��ۑ�����N���X
class CSaveData
{
private:
	CSaveData() {}
	CSaveData(const CSaveData&) {}
	~CSaveData() {}

public:
	static void Create();
	static CSaveData* GetInstance();
	static void Destory();

	void Init(void* data, int data_size); // �����o������
	void Save();						  // �f�[�^�̕ۑ�
	void Load();						  // �f�[�^�̃��[�h
	void* GetData() { return m_pData; }	  // �Z�[�u�f�[�^��Ԃ�

private:
	static CSaveData* m_pInstance;

	void* m_pData;   // �Z�[�u�����f�[�^
	int m_nDataSize; // �Z�[�u�����f�[�^�̃T�C�Y
};
