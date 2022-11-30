#include "CModelManager.h"

// モデルデータが入っているディレクトリをここで指定
// (ここで指定することでわざわざ全てのディレクトリを書かなくてもよくなる)
#define MODEL_DIRECTORY (L"Asset\\Model\\")

using namespace std;

CModelManager* CModelManager::m_instance = 0;

void CModelManager::Create()
{
	if (m_instance != NULL)
	{
		MessageBox(NULL, L"モデル管理クラス作成失敗", L"エラー", MB_OK);
		return;
	}

	m_instance = new CModelManager;
}

CModelManager* CModelManager::GetInstance()
{
	if (m_instance == NULL)
	{
		MessageBox(NULL, L"モデル管理クラスが生成されていません。", L"エラー", MB_OK);
		return NULL;
	}

	return m_instance;
}

void CModelManager::Destory()
{
	if (m_instance != nullptr)
	{
		delete m_instance;
		m_instance = nullptr;
	}
}

CModelManager::~CModelManager()
{
	for(auto &itr : m_ModelMap)
	{
		delete itr.second;
	}

	m_ModelMap.clear();
}

// 画像作成
// 引数1 const wchar_t* : Asset\Modelに入っているファイル名
bool CModelManager::CreateModel(const wchar_t* wzFileName)
{
	CModel* pModel;	

	// ディレクトリとファイル名を合わせる
	wstring wsFileName(MODEL_DIRECTORY);
	wsFileName += wzFileName;

	// モデル作成
	pModel = new CModel;

	// モデルデータをロードします
	if (pModel->LoadNXModel(wsFileName.c_str()) == true)
	{
		m_ModelMap.insert(make_pair(wsFileName, pModel));

		return true;
	}

	// データがない場合
	wchar_t str[256];
	wcscpy_s(str, wsFileName.c_str());
	wcscat_s(str, L"が見つかりませんでした。");
	MessageBox(NULL, str, L"エラー", MB_OK);
	return false;
}

// 画像データ取得
CModel* CModelManager::GetModel(const wchar_t* FileName)
{
	unordered_map<wstring, CModel*>::iterator itr;

	// ディレクトリとファイル名を合わせる
	wstring wsFileName(MODEL_DIRECTORY);
	wsFileName += FileName;

	// 画像データを探す
	itr = m_ModelMap.find(wsFileName);

	// データが見つかった場合
	if (itr != m_ModelMap.end())
	{
		// 画像データを返す
		return itr->second;
	}

	// データがなかった場合
	wchar_t str[256];
	wcscpy_s(str, wsFileName.c_str());
	wcscat_s(str, L"が作成されていません。");
	MessageBox(NULL, str, L"エラー", MB_OK);
	return NULL;
}
