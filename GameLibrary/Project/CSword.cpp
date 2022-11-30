#include "CSword.h"

#include "CShader.h"
#include "CInput.h"
#include "CModelManager.h"

#include "CEnemy.h"

CSword::CSword()
{
	CModelManager* pModelManager = CModelManager::GetInstance();
	m_Model = pModelManager->GetModel(L"sword.nx");
	m_ColObb = *(m_Model->GetObb());
	
	m_vScale = D3DXVECTOR3(0.2f, 0.2f, 0.2f);
	m_isHitBoxDraw = false;
}

void CSword::Draw()
{
	if (m_isHitBoxDraw == true)
	{
		// 当たり判定表示
		Collision::DrawDebugOBB(m_ColObb);
		m_isHitBoxDraw = false;
	}

	CShader* pShader = CShader::GetInstance();
	pShader->ModelDraw(m_Model, m_matWorld, false);

	// 影を付ける
	pShader->ModelShadowDraw(m_Model, m_matWorld);
}

// 当たり判定系の処理
void CSword::Collision()
{
	// 当たり判定表示フラグを立てる
	m_isHitBoxDraw = true;

	// 当たり判定の大きさ調整
	D3DXVECTOR3 vObbColScale = D3DXVECTOR3(m_vScale.x, m_vScale.y, m_vScale.z);

	D3DXQUATERNION qRotate;
	D3DXMATRIX matWorld;

	// スケール情報と回転情報を掛け合わしたデータから回転情報を取り出す
	matWorld._11 = m_matWorld._11 / m_vScale.x;
	matWorld._12 = m_matWorld._12 / m_vScale.y;
	matWorld._13 = m_matWorld._13 / m_vScale.z;
	matWorld._14 = 0.f;
	matWorld._21 = m_matWorld._21 / m_vScale.x;
	matWorld._22 = m_matWorld._22 / m_vScale.y;
	matWorld._23 = m_matWorld._23 / m_vScale.z;
	matWorld._24 = 0.f;
	matWorld._31 = m_matWorld._31 / m_vScale.x;
	matWorld._32 = m_matWorld._32 / m_vScale.y;
	matWorld._33 = m_matWorld._33 / m_vScale.z;
	matWorld._34 = 0.f;
	matWorld._41 = 0.f;
	matWorld._42 = 0.f;
	matWorld._43 = 0.f;
	matWorld._44 = 1.f;

	D3DXQuaternionRotationMatrix(&qRotate, &matWorld);
	D3DXQuaternionMultiply(&qRotate, &m_qRotate, &qRotate);

	D3DXVECTOR3 vTranslate = D3DXVECTOR3(m_matWorld._41, m_matWorld._42, m_matWorld._43);

	m_ColObb.Update(vObbColScale, qRotate, vTranslate);

	std::vector<std::shared_ptr<CObj>> EnemysData;

	// すべての敵の情報を取得
	CTaskSystem* pTaskSystem = CTaskSystem::GetInstance();
	pTaskSystem->ObjsInfomation(typeid(CEnemy), &EnemysData);

	for (auto ip = EnemysData.begin(); ip != EnemysData.end(); ip++)
	{
		CEnemy* pEnemy = (CEnemy*)ip->get();
		if (Collision::HitObb3D(m_ColObb, *pEnemy->GetObb()) == true)
		{
			// 敵の削除フラグを立てる
			pEnemy->SetDefeatFlag(true);
		}
	}
}

void CSword::Move(const D3DXMATRIX& matWorld)
{
	// このオブジェクトのワールド行列作成
	CreateMatWorld();

	// プレイヤーのボーンに合わせる
	m_matWorld = m_matWorld * matWorld;
}
