#pragma once

#include <vector>
#include <memory>
#include <list>
#include <algorithm>
#include <typeinfo.h>

#pragma warning(push)
#pragma warning(disable : 4005)
#include <d3dx10.h>
#pragma warning(pop)

// オブジェクト継承用
class CObj
{
public:
	CObj()
	{
		m_isDelete = false;
	}
	virtual ~CObj() {};
	virtual void Update() = 0;
	virtual void Draw() = 0;

	void ObjDelete() { m_isDelete = true; }
	bool GetDeleteFlag() { return m_isDelete; }

private:
	bool m_isDelete; // 削除フラグ
};

// 3Dオブジェクト継承用
class CObj3D : public CObj
{
public:
	CObj3D()
	{
		m_vScale	= D3DXVECTOR3(1.f, 1.f, 1.f);
		m_qRotate	= D3DXQUATERNION(0.f, 0.f, 0.f, 1.f);
		m_vPosition = D3DXVECTOR3(0.f, 0.f, 0.f);
		D3DXMatrixIdentity(&m_matWorld);
	}
	D3DXVECTOR3* GetScale()		{ return &m_vScale; }
	D3DXQUATERNION* GetRotate() { return &m_qRotate; }
	D3DXVECTOR3* GetTranslate() { return &m_vPosition; }
	D3DXMATRIX* GetMatWorld()	{ return &m_matWorld; }
	// m_vScaleなどのオブジェクトメンバ情報を使って、ワールド行列を作成して、m_matWorldに入れます。
	void CreateMatWorld() 
	{ 
		D3DXMATRIX matScale, matRotate, matTranslate;
		D3DXMatrixScaling(&matScale, m_vScale.x, m_vScale.y, m_vScale.z);
		D3DXMatrixRotationQuaternion(&matRotate, &m_qRotate);
		D3DXMatrixTranslation(&matTranslate, m_vPosition.x, m_vPosition.y, m_vPosition.z);
		m_matWorld = matScale * matRotate * matTranslate;
	}

protected:
	D3DXVECTOR3	   m_vScale;	// 大きさ情報
	D3DXQUATERNION m_qRotate;	// 回転情報
	D3DXVECTOR3	   m_vPosition;	// 位置情報
	D3DXMATRIX	   m_matWorld;	// ワールド行列
};

// タクスを管理するクラス
class CTaskSystem
{
private:
	CTaskSystem();
	CTaskSystem(const CTaskSystem&) {}
	~CTaskSystem();

public:
	static void Create();
	static CTaskSystem* GetInstance();
	static void Destory();

	// オブジェクト追加
	void AddObj(CObj * object);
	// オブジェクト更新
	void Update();
	// オブジェクト描画
	void Draw();
	// オブジェクト情報取得
	// 引数1 const type_info& : typeid(クラス名)と指定する
	void ObjsInfomation(const type_info& type, std::vector<std::shared_ptr<CObj>>* obj);
	// クラスオブジェクトの数
	// 引数1 const type_info& : typeid(クラス名)と指定する
	int ObjectNum(const type_info& type);
	
	// オブジェクトが削除されているまたは、存在していないか
	bool ObjDeleteCheck(CObj* object);
	// 全てのオブジェクトを削除する関数
	void ObjAllDelete();

public:
	static CTaskSystem* m_pInstance;

	std::list<std::shared_ptr<CObj>>* m_TaskList; // ゲームオブジェクトリスト
};
