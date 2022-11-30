#pragma once

#include "CModel.h"
#include "Collision.h"

#include <unordered_map>

// ���f���Ǘ��N���X(�V���O���g��)
class CModelManager
{
private:
	CModelManager() {}
	CModelManager(const CModelManager&) {}
	~CModelManager();

public:
	static void Create();
	static CModelManager* GetInstance();
	static void Destory();
	bool CreateModel(const wchar_t* pFileName); // ���f���쐬�Ɠo�^
	CModel* GetModel(const wchar_t* pFileName); // �t�@�C�������烂�f���������Ă���

private:
	static CModelManager* m_instance;

	// ���f���f�[�^���Ǘ�����ϐ�
	std::unordered_map<std::wstring, CModel*> m_ModelMap;
};
