#include "CTaskSystem.h"

#include <Windows.h>

using namespace std;

CTaskSystem* CTaskSystem::m_pInstance = 0;

CTaskSystem::CTaskSystem()
{
	m_TaskList = new list<shared_ptr<CObj>>();
}

CTaskSystem::~CTaskSystem()
{
	if (m_TaskList)
	{
		m_TaskList->clear();
		delete m_TaskList;
		m_TaskList = NULL;
	}
}

void CTaskSystem::Create()
{
	// 複数生成を防ぐ
	if (m_pInstance != nullptr)
	{
		MessageBox(NULL, L"CTaskSystemが複数生成されました。",
			L"エラー", MB_OK);
		return;
	}

	m_pInstance = new CTaskSystem;
}

CTaskSystem* CTaskSystem::GetInstance()
{
	if (m_pInstance == NULL)
	{
		MessageBox(NULL, L"CTaskSystemクラスが生成されていません", L"エラー", MB_OK);
		return NULL;
	}
	return m_pInstance;
}

void CTaskSystem::Destory()
{
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

void CTaskSystem::AddObj(CObj* object)
{
	//シェアポインタにオブジェクトのアドレスを格納
	shared_ptr< CObj > sp;
	sp.reset(object);

	m_TaskList->push_back(sp);
}

// 全てのオブジェクトを更新
void CTaskSystem::Update()
{
	for (auto ip = m_TaskList->begin(); ip != m_TaskList->end();)
	{
		// 削除フラグがたっていたら、オブジェクトを削除
		if (ip->get()->GetDeleteFlag() == true)
		{
			ip = m_TaskList->erase(ip);
		}
		else
		{
			ip->get()->Update();
			ip++;
		}
	}
}

// 全てのオブジェクトを描画
void CTaskSystem::Draw()
{
	for (auto ip = m_TaskList->begin(); ip != m_TaskList->end(); ip++)
	{
		ip->get()->Draw();
	}
}

// オブジェクト情報取得
// 引数1 const type_info& : typeid(クラス名)と指定する
// 引数2 vector<shared_ptr<Co>>
void CTaskSystem::ObjsInfomation(const type_info& Type, vector<shared_ptr<CObj>>* pObj)
{
	// リストの先端から末端まで検索
	for (auto ip = m_TaskList->begin(); ip != m_TaskList->end(); ip++)
	{
		// 型チェック
		if (Type == typeid(*ip->get()))
		{
			// オブジェクトが削除されていないか
			if (ip->get()->GetDeleteFlag() == true) continue;

			// 同じ型であれば、objにアドレスを入れる
			pObj->push_back(*ip);
		}
	}
}

// オブジェクトが削除されているまたは、存在していないか
bool CTaskSystem::ObjDeleteCheck(CObj* object)
{
	if (object == NULL) return true;

	// リストの先端から末端まで検索
	for (auto ip = m_TaskList->begin(); ip != m_TaskList->end(); ip++)
	{
		if (ip->get() == object)
		{
			return false;
		}
	}

	return true;
}

// 全てのオブジェクトを削除する関数
void CTaskSystem::ObjAllDelete()
{
	m_TaskList->clear();
}

// オブジェクトの数を返す関数
// 引数1 const type_info& : 調べたいクラス typeid(クラス名)と指定する
// 戻り値 int			  : オブジェクトの数 
int CTaskSystem::ObjectNum(const type_info& Type)
{
	// オブジェクトを記録する変数
	int nCount = 0;

	// リストの先端から末端まで検索
	for (auto ip = m_TaskList->begin(); ip != m_TaskList->end(); ip++)
	{
		// 型チェック
		if (Type == typeid(*ip->get()))
		{
			nCount++;
		}
	}

	return nCount;
}
