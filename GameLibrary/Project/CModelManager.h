#pragma once

#include "CModel.h"
#include "Collision.h"

#include <unordered_map>

// モデル管理クラス(シングルトン)
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
	bool CreateModel(const wchar_t* pFileName); // モデル作成と登録
	CModel* GetModel(const wchar_t* pFileName); // ファイル名からモデルを持ってくる

private:
	static CModelManager* m_instance;

	// モデルデータを管理する変数
	std::unordered_map<std::wstring, CModel*> m_ModelMap;
};
