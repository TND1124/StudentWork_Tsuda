#include "CEnemy.h"

#include "CDevice3D.h"
#include "CShader.h"
#include "CSound.h"

#include "CPlayer.h"
#include "Cyuka.h"
#include "CSky.h"
#include "CSword.h"
#include "CModelManager.h"

// ジャンプするタイミング
#define JUMP_TIMING (150)

CEnemy::CEnemy(const D3DXVECTOR3& vPosition)
{
	CModelManager* pModelManager = CModelManager::GetInstance();
	m_pModel = pModelManager->GetModel(L"JumpEnemy.nx");

	m_ColObb = *(m_pModel->GetObb());

	m_vPosition = vPosition;
	m_Velocity = D3DXVECTOR3(0.f, 0.f, 0.f);
	m_MoveSpeed = 0.05f;
	m_jump_time = 0;

	m_vGravity = D3DXVECTOR3(0.f, 0.f, 0.f);

	m_isDefeat = false;
	m_DefeatRadian = 0.f;

	// プレイヤー情報を取得
	CTaskSystem* pTaskSystem = CTaskSystem::GetInstance();
	std::vector<std::shared_ptr<CObj>> vecPlayer;
	pTaskSystem->ObjsInfomation(typeid(CPlayer), &vecPlayer);
	CPlayer* pPlayer = (CPlayer*)(vecPlayer.begin())->get();
	std::vector<std::shared_ptr<CObj>> swap;
	vecPlayer.swap(swap);

	// プレイヤーの位置情報を取得
	m_pvPlayerPosition = pPlayer->GetTranslate();

	// 当たり判定更新
	m_ColObb.Update(m_vScale, m_qRotate, m_vPosition);
}

void CEnemy::Update()
{
	// 倒されたら
	if (m_isDefeat == true)
	{
		// 倒れるアニメーションを作成
		m_DefeatRadian += 0.05f;
		D3DXVECTOR3 axis(1.f, 0.f, 0.f);
		D3DXQUATERNION q;
		D3DXQuaternionRotationAxis(&q, &axis, 0.05f);

		D3DXQuaternionMultiply(&m_qRotate, &q, &m_qRotate);

		if (m_DefeatRadian * 180.f / 3.14 > 90.f)
		{
			ObjDelete();
		}
	}
	else
	{
		// 移動処理
		Move();

		// 当たり判定処理
		Collsion();
	}

	// ワールド行列作成
	CreateMatWorld();
}

void CEnemy::Draw()
{
	CShader* pShader = CShader::GetInstance();
	pShader->ModelDraw(m_pModel, m_matWorld, false);

	// 倒されたときは、当たり判定表示を切る)
	if (m_isDefeat == false)
	{
		Collision::DrawDebugOBB(m_ColObb);
	}

	// 影を付ける
	pShader->ModelShadowDraw(m_pModel, m_matWorld);
}

// 移動処理
void CEnemy::Move()
{
	// ジャンプさせる 
	if (m_jump_time == JUMP_TIMING)
	{
		m_Velocity.y = 50.f;
		m_jump_time = 0;

		CSound* pSound = CSound::GetInstance();
		pSound->PlayWave2D(5);
	}
	else
	{
		m_jump_time++;
	}

	// プレイヤーを追いかける
	float radian = atan2f(m_pvPlayerPosition->z - m_vPosition.z, m_pvPlayerPosition->x - m_vPosition.x);
	
	m_Velocity.x = cos(radian);
	m_Velocity.z = sin(radian);

	D3DXVECTOR3 vAxis = D3DXVECTOR3(0.f, 1.f, 0.f);

	// プレイヤーの方向を向く
	D3DXQuaternionRotationAxis(&m_qRotate, &vAxis, -radian + 270.f / 180.f * 3.14f);

	D3DXVECTOR3 MoveOffset = -m_Velocity;

	// 移動ができるかどうか
	m_ColSegment.m_vStartPosition = m_vPosition + D3DXVECTOR3(0.f, 0.5f, 0.f) + MoveOffset;
	m_ColSegment.m_vEndPosition = m_vPosition + D3DXVECTOR3(0.f, 0.5f, 0.f) + m_Velocity * m_MoveSpeed;

	CTaskSystem* pTaskSystem = CTaskSystem::GetInstance();
	std::vector<std::shared_ptr<CObj>> sky;
	pTaskSystem->ObjsInfomation(typeid(CSky), &sky);
	CSky* pSky = (CSky*)(sky.begin())->get();
	std::vector<std::shared_ptr<CObj>> swap;
	sky.swap(swap);

	// 壁にぶつかっていないかどうかを調べる
	D3DXVECTOR3 out;
	if (Collision::HitSegmentAndModel(m_ColSegment, *pSky->GetModel(), *pSky->GetMatWorld(), &out) == true)
	{
		// 衝突
		// ぶつかった場所まで移動
		m_vPosition += out + MoveOffset;
	}
	else
	{
		// 衝突してない
		// 移動
		m_vPosition += m_Velocity * m_MoveSpeed;
	}
	m_Velocity = D3DXVECTOR3(0.f, 0.f, 0.f);
}

// 当たり判定系の処理
void CEnemy::Collsion()
{
	// 重力の値更新
	m_vGravity.y += -0.0098f;

	// 当たり判定更新
	m_ColObb.Update(m_vScale, m_qRotate, m_vPosition);

	D3DXVECTOR3 vGravityOffset = D3DXVECTOR3(0.f, 1.f, 0.f);

	// 重力で床との衝突判定に必要な線分を作成
	m_ColSegment.m_vStartPosition = m_vPosition + vGravityOffset;
	m_ColSegment.m_vEndPosition = m_vPosition + m_vGravity;

	// 当たり判定を調べたい床オブジェクト情報取得
	CTaskSystem* pTaskSystem = CTaskSystem::GetInstance();
	std::vector<std::shared_ptr<CObj>> ObjData;
	pTaskSystem->ObjsInfomation(typeid(Cyuka), &ObjData);
	Cyuka* Yuka = (Cyuka*)(ObjData.begin())->get();
	std::vector<std::shared_ptr<CObj>> swap;
	ObjData.swap(swap);

	// 重力ベクトルと床との衝突判定
	D3DXVECTOR3 out;
	if (Collision::HitSegmentAndModel(m_ColSegment, *Yuka->GetModel(),
		*Yuka->GetMatWorld(), &out) == true)
	{
		// 当たっていれば
		m_vPosition += out + vGravityOffset;

		m_vGravity.y = 0.f;
	}
	else
	{
		// 重力をそのままかける
		m_vPosition += m_vGravity;
	}
}
