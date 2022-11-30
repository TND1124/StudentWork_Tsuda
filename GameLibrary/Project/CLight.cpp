#include "CLight.h"

CLightManager* CLightManager::m_pInstance = nullptr;

void CLightManager::Create()
{
	// ����������h��
	if (m_pInstance != nullptr)
	{
		MessageBox(NULL, L"CLightManager��������������܂����B",
			L"�G���[", MB_OK);
		return;
	}

	m_pInstance = new CLightManager;
}

CLightManager* CLightManager::GetInstance()
{
	if (m_pInstance == NULL)
	{
		MessageBox(NULL, L"CLightManager�N���X����������Ă��܂���", L"�G���[", MB_OK);
		return NULL;
	}
	return m_pInstance;
}

void CLightManager::Destory()
{
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
