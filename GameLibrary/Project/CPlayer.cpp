#include "CPlayer.h"

#include "CShader.h"
#include "CInput.h"

#include "Cyuka.h"
#include "CSky.h"
#include "CCamera.h"
#include "CEnemy.h"
#include "CModelManager.h"

#include "CSound.h"

extern CCamera* g_pCamera;

CPlayer::CPlayer()
{
	CModelManager* pModelManger = CModelManager::GetInstance();
	m_Model = pModelManger->GetModel(L"player.nx");
	m_ColObb = *(m_Model->GetObb());

	m_Velocity = D3DXVECTOR3(0.f, 0.f, 0.f);
	m_MoveSpeed = 1.f;
	
	// キャラクターに持たせる剣
	m_Sword = new CSword;
	
	// タスクに追加
	CTaskSystem* pTaskSystem = CTaskSystem::GetInstance();
	pTaskSystem->AddObj(m_Sword);

	m_Hp = m_HpMax      = 100;
	m_SlashSwordFlag	= false;
	m_nInvincibleTime	= 0;
	m_DamageAniMoveFlag = false;

	m_fIdelAniFrame   = 0.f;
	m_fRunAniFrame	  = 0.f;
	m_fSwordAniFrame  = 0.f;
	m_fWalkAniFrame	  = 0.f;
}

void CPlayer::Update()
{
	// 無敵時間中なら、無敵有効時間を減らす
	if (m_nInvincibleTime != 0)
	{
		m_nInvincibleTime--;
	}
	
	CInput* pInput = CInput::GetInstance();
	// 剣を振るという入力を一度だけ取る
	if (pInput->GetKeyDown('Z') == true)
	{
		if (m_SlashSwordFlag == false)
		{
			CSound* pSound = CSound::GetInstance();
			pSound->PlayWave2D(2);
		}

		m_SlashSwordFlag = true;
	}

	// 移動処理
	if (m_SlashSwordFlag == false)
	{
		Move();
	}

	// 剣を振るという入力を受け取ったら、剣を振るアニメーションが終わるまで進める
	if (m_SlashSwordFlag == true)
	{
		// 剣を振る
		SwordSlash();
	}
	else
	{
		// 当たり判定処理
		Collsion();
	}

	// キャラクターをカメラの方向に向ける
	const D3DXVECTOR3* pRadian = g_pCamera->GetRadian();
	// pvRadian->xは現在のカメラの角度 3.14はキャラクターをカメラの初期向きに合わせる(180.f / 180.f * 3.14fのこと)
	D3DXQuaternionRotationYawPitchRoll(&m_qRotate, pRadian->x + 3.14f, 0.f, 0.f);

	// ワールド行列作成
	CreateMatWorld();


	// 剣の位置更新
	{
		D3DXMATRIX matSwordWorld;
		matSwordWorld = m_Model->GetBone(50)->m_matOffset;
		matSwordWorld._41 = -matSwordWorld._41;
		matSwordWorld._42 = -matSwordWorld._42;

		D3DXMATRIX matAniSwordWorld = m_Model->GetBone(50)->m_matAnimation;
		matSwordWorld = matSwordWorld * matAniSwordWorld * m_matWorld;

		// 剣を移動させる
		m_Sword->Move(matSwordWorld);
	}


	D3DXVECTOR3 vPlayerEye = m_vPosition;
	vPlayerEye.y += 2.00f;
	
	g_pCamera->SetEye(vPlayerEye);

	vPlayerEye.y -= 0.5f;
	g_pCamera->SetLookAt(vPlayerEye);

	// カメラ情報更新
	g_pCamera->Update();
}

void CPlayer::Draw()
{
	CShader* pShader = CShader::GetInstance();
	if (m_nInvincibleTime % 2 == 0)
	{
		pShader->ModelDraw(m_Model, m_matWorld, false);
	}

	// 剣を振っている間は、当たり判定を出さない
	if (m_SlashSwordFlag == false)
	{
		// 当たり判定描画
		Collision::DrawDebugOBB(m_ColObb);
	}

	// 影を付ける
	pShader->ModelShadowDraw(m_Model, m_matWorld);

	// 主人公の位置表示デバッグ用
#ifdef _DEBUG
	wchar_t str[256];
	swprintf_s(str, L"PlayerX : %f", m_vPosition.x);
	pShader->StrDraw(400.f, 0.f, 24.f, str, 0.f);
	swprintf_s(str, L"PlayerY : %f", m_vPosition.y);
	pShader->StrDraw(400.f, 24.f, 24.f, str, 0.f);
	swprintf_s(str, L"PlayerZ : %f", m_vPosition.z);
	pShader->StrDraw(400.f, 48.f, 24.f, str, 0.f);
#endif 

	pShader->StrDraw(0.f, 80.f, 32.f, L"Zキーで攻撃");
	pShader->StrDraw(0.f, 112.f, 32.f, L"Xキーでカメラ回転(左)");
	pShader->StrDraw(0.f, 144.f, 32.f, L"Cキーでカメラ回転(右)");
	pShader->StrDraw(0.f, 176.f, 32.f, L"矢印キーで移動");
	pShader->StrDraw(0.f, 208.f, 32.f, L"矢印キー + Shiftでダッシュ");
	
}

// 移動処理
void CPlayer::Move()
{
	// 入力処理------------------------------------------

	CInput* pInput = CInput::GetInstance();
	if (pInput->GetKey(VK_LEFT) == true)
	{
		m_Velocity.x -= 1.f;
	}
	if (pInput->GetKey(VK_DOWN) == true)
	{
		m_Velocity.z -= 1.f;
	}
	if (pInput->GetKey(VK_RIGHT) == true)
	{
		m_Velocity.x += 1.f;
	}
	if (pInput->GetKey(VK_UP) == true)
	{
		m_Velocity.z += 1.f;
	}

	// 移動入力されていれば
	if (m_Velocity.x != 0.f || m_Velocity.z != 0.f)
	{
		D3DXVECTOR3 vCamera;
		vCamera = *g_pCamera->GetLookAt() - *g_pCamera->GetEye();
		D3DXVec3Normalize(&vCamera, &vCamera);

		float cos_vec, sin_vec;
		float radian = (-90.f / 180.f) * 3.14f;
		cos_vec = m_Velocity.x * cosf(radian) - m_Velocity.z * sinf(radian);
		sin_vec = m_Velocity.z * cosf(radian) + m_Velocity.x * sinf(radian);

		m_Velocity.x = vCamera.x * cos_vec - vCamera.z * sin_vec;
		m_Velocity.z = vCamera.z * cos_vec + vCamera.x * sin_vec;

		D3DXVec3Normalize(&m_Velocity, &m_Velocity);

		// 走る
		if (pInput->GetKey(VK_SHIFT) == true)
		{
			m_MoveSpeed = 0.3f;

			// 走るアニメーション再生
			// スキンアニメーション管理------
			m_fRunAniFrame += 0.5f;
			if (m_fRunAniFrame >= m_Model->GetAnimationFrame((int)AnimationKey::Run))
			{
				m_fRunAniFrame = 0.f;
			}
			m_Model->SkinAnimation(m_fRunAniFrame, (int)AnimationKey::Run);
			// ------------------------------
		}
		else
		{
			m_MoveSpeed = 0.1f;

			// 移動アニメーション再生
			// スキンアニメーション管理------
			m_fWalkAniFrame += 0.5f;
			if (m_fWalkAniFrame >= m_Model->GetAnimationFrame((int)AnimationKey::Walk))
			{
				m_fWalkAniFrame = 0.f;
			}
			m_Model->SkinAnimation(m_fWalkAniFrame, (int)AnimationKey::Walk);
			// ------------------------------
		}

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
			m_vPosition.x += m_Velocity.x * m_MoveSpeed;
			m_vPosition.z += m_Velocity.z * m_MoveSpeed;
		}
		m_Velocity = D3DXVECTOR3(0.f, 0.f, 0.f);
	}
	else
	{
		m_fIdelAniFrame += 0.5f;
		if (m_fIdelAniFrame >= m_Model->GetAnimationFrame((int)AnimationKey::Idle))
		{
			m_fIdelAniFrame = 0.f;
		}
		m_Model->SkinAnimation(m_fIdelAniFrame, (int)AnimationKey::Idle);
	}

	// -------------------------------------------------
}

// 当たり判定系の処理
void CPlayer::Collsion()
{
	// 当たり判定の大きさ調整
	D3DXVECTOR3 vObbColScale = D3DXVECTOR3(m_vScale.x / 2.f, m_vScale.y, m_vScale.z);

	// 当たり判定更新
	m_ColObb.Update(vObbColScale, m_qRotate, m_vPosition);

	std::vector<std::shared_ptr<CObj>> ObjData;	// 欲しいデータを入れるベクター
	std::vector<std::shared_ptr<CObj>> swap;    // ベクターのメモリ解放用

	CTaskSystem* pTaskSystem = CTaskSystem::GetInstance();

	// 無敵中でなければ、
	if (m_nInvincibleTime == 0)
	{
		// 敵に当たるとダメージ
		pTaskSystem->ObjsInfomation(typeid(CEnemy), &ObjData);
		for (auto ip = ObjData.begin(); ip != ObjData.end(); ip++)
		{
			CEnemy* pEnemy = (CEnemy*)(ip->get());
			if (Collision::HitObb3D(m_ColObb, *pEnemy->GetObb()) == true)
			{
				// ダメージ音
				CSound* pSound = CSound::GetInstance();
				pSound->PlayWave2D(1);

				m_nInvincibleTime = 100;
				m_Hp -= 10;
				m_DamageAniMoveFlag = true;
				break;
			}
		}
		ObjData.swap(swap);
	}

	D3DXVECTOR3 vGravity = D3DXVECTOR3(0.f, -0.3f, 0.f);
	D3DXVECTOR3 GravityOffset = D3DXVECTOR3(0.f, 1.f, 0.f);

	// 重力で床との衝突判定に必要な線分を作成
	m_ColSegment.m_vStartPosition = m_vPosition + GravityOffset;
	m_ColSegment.m_vEndPosition = m_vPosition + vGravity;

	pTaskSystem->ObjsInfomation(typeid(Cyuka), &ObjData);
	Cyuka* pYuka = (Cyuka*)(ObjData.begin())->get();
	ObjData.swap(swap);

	// 重力ベクトルと床との衝突判定
	D3DXVECTOR3 out;
	if (Collision::HitSegmentAndModel(m_ColSegment, *pYuka->GetModel(), *pYuka->GetMatWorld(), &out) == true)
	{
		// 当たっていれば
		m_vPosition += out + GravityOffset;
	}
	else
	{
		// 重力をそのままかける
		m_vPosition += vGravity;
	}
}

// 剣を振る処理
void CPlayer::SwordSlash()
{
	// 剣を振るアニメーションを進める
	m_fSwordAniFrame += 0.5f;
	if (m_fSwordAniFrame >= m_Model->GetAnimationFrame((int)AnimationKey::Slash))
	{
		m_fSwordAniFrame = 0.f;
		m_SlashSwordFlag = false;
	}
	// アニメーションさせる
	m_Model->SkinAnimation(m_fSwordAniFrame, (int)AnimationKey::Slash);

	// 剣の当たり判定を有効にする
	m_Sword->Collision();
}
