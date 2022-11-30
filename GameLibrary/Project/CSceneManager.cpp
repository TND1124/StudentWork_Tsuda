#include "CSceneManager.h"
#include <Windows.h>

using namespace std;

CSceneManager* CSceneManager::m_pInstance = 0;

void CSceneManager::Create(CScene* pScene)
{
	if (m_pInstance != NULL)
	{
		MessageBox(NULL, L"シーンマネージャークラス作成失敗", L"エラー", MB_OK);
		return;
	}

	m_pInstance = new CSceneManager(pScene);
}

CSceneManager* CSceneManager::GetInstance()
{
	if (m_pInstance == NULL)
	{
		MessageBox(NULL, L"シーンマネージャークラスが生成されていません。", L"エラー", MB_OK);
		return NULL;
	}

	return m_pInstance;
}

void CSceneManager::Destory()
{
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

CSceneManager::CSceneManager(CScene* pScene)
{
	m_pScene = pScene;
}

CSceneManager::~CSceneManager()
{
	if (m_pScene != NULL)
	{
		delete m_pScene;
		m_pScene = NULL;
	}
}

void CSceneManager::Update()
{
	CScene* pSceneNext = m_pScene->Update();
	if (pSceneNext != m_pScene)
	{
		delete m_pScene;
		m_pScene = pSceneNext;
	}
}
