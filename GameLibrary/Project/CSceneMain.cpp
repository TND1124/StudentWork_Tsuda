#include "CSceneMain.h"

#include "CPlayer.h"
#include "Cyuka.h"
#include "CSky.h"
#include "CEnemy.h"
#include "CHpGauge.h"
#include "CCamera.h"
#include "CSound.h"

#include "CTaskSystem.h"
#include "CSceneGameOver.h"
#include "CSceneGameClear.h"

extern CCamera* g_pCamera;

CSceneMain::CSceneMain()
{
	g_pCamera->SetIsTps(true);

	CTaskSystem* pTaskSystem = CTaskSystem::GetInstance();

	// プレイヤーオブジェクト作成
	m_pPlayer = new CPlayer;
	// タスクに追加
	pTaskSystem->AddObj(m_pPlayer);

	// 床オブジェクト作成
	Cyuka*   pYuka = new Cyuka;
	pTaskSystem->AddObj(pYuka);

	// 空(そら)オブジェクト作成
	CSky*    pSky = new CSky;
	pTaskSystem->AddObj(pSky);

	m_nEnemyAnnihilation = 0;

	for (int i = -2; i <= 2; i++)
	{
		// 敵オブジェクト作成
		CEnemy* pEnemy = new CEnemy(D3DXVECTOR3(10.f * i, 0.f, 10.f));
		pTaskSystem->AddObj(pEnemy);
	}

	// HPゲージオブジェクト作成
	CHpGauge* pHpGauge = new CHpGauge(m_pPlayer->GetpHp(), m_pPlayer->GetHpMax());
	pTaskSystem->AddObj(pHpGauge);

	CSound* pSound = CSound::GetInstance();

	// ゲーム音
	pSound->LoadWave2D(L"Asset\\Audio\\Game_BGM.wav", 0);
	// ゲーム音を流す
	pSound->PlayLoodWave2D(0);

	// ダメージ音
	pSound->LoadWave2D(L"Asset\\Audio\\Damage_SE.wav", 1);

	// プレイヤー攻撃音
	pSound->LoadWave2D(L"Asset\\Audio\\HeroAttack_SE.wav", 2);

	// プレイヤーの走る音
	pSound->LoadWave2D(L"Asset\\Audio\\Run_SE", 3);

	// プレイヤーの歩く音
	pSound->LoadWave2D(L"Asset\\Audio\\Walk_SE.wav", 4);

	// 敵のジャンプ音
	pSound->LoadWave2D(L"Asset\\Audio\\EnemyJump_SE.wav", 5);
}

CScene* CSceneMain::Update()
{
	CSound* pSound = CSound::GetInstance();

	CScene* pSceneNext = this;

	CTaskSystem* pTaskSystem = CTaskSystem::GetInstance();

	// プレイヤーのHPが0になったら、ゲームオーバー画面へ
	if (*m_pPlayer->GetpHp() <= 0)
	{
		// 現在あるオブジェクトを全て削除
		pTaskSystem->ObjAllDelete();

		pSound->StopWave2D(0);

		// 音情報削除
		pSound->Delete();

		return new CSceneGameOver;
	}

	if (pTaskSystem->ObjectNum(typeid(CEnemy)) == 0)
	{
		// 敵を決まったウェーブ数倒すとゲームクリア画面へ
		if (m_nEnemyAnnihilation == 2)
		{
			// 現在あるオブジェクトをすべて削除
			pTaskSystem->ObjAllDelete();

			pSound->StopWave2D(0);

			// 音情報削除
			pSound->Delete();
			return new CSceneGameClear;
		}

		if (m_nEnemyAnnihilation % 2 == 0)
		{
			for (int i = -3; i <= 3; i++)
			{
				// 敵オブジェクト作成
				CEnemy* pEnemy = new CEnemy(D3DXVECTOR3(10.f * i, 50.f, 10.f));
				pTaskSystem->AddObj(pEnemy);
			}
		}
		else
		{
			for (int i = -3; i <= 3; i++)
			{
				// 敵オブジェクト作成
				CEnemy* pEnemy = new CEnemy(D3DXVECTOR3(10.f * i, 0.f, 30.f));
				pTaskSystem->AddObj(pEnemy);
			}
		}

		m_nEnemyAnnihilation++;
	}

	return pSceneNext;
}
