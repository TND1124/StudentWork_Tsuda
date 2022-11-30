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

	// �v���C���[�I�u�W�F�N�g�쐬
	m_pPlayer = new CPlayer;
	// �^�X�N�ɒǉ�
	pTaskSystem->AddObj(m_pPlayer);

	// ���I�u�W�F�N�g�쐬
	Cyuka*   pYuka = new Cyuka;
	pTaskSystem->AddObj(pYuka);

	// ��(����)�I�u�W�F�N�g�쐬
	CSky*    pSky = new CSky;
	pTaskSystem->AddObj(pSky);

	m_nEnemyAnnihilation = 0;

	for (int i = -2; i <= 2; i++)
	{
		// �G�I�u�W�F�N�g�쐬
		CEnemy* pEnemy = new CEnemy(D3DXVECTOR3(10.f * i, 0.f, 10.f));
		pTaskSystem->AddObj(pEnemy);
	}

	// HP�Q�[�W�I�u�W�F�N�g�쐬
	CHpGauge* pHpGauge = new CHpGauge(m_pPlayer->GetpHp(), m_pPlayer->GetHpMax());
	pTaskSystem->AddObj(pHpGauge);

	CSound* pSound = CSound::GetInstance();

	// �Q�[����
	pSound->LoadWave2D(L"Asset\\Audio\\Game_BGM.wav", 0);
	// �Q�[�����𗬂�
	pSound->PlayLoodWave2D(0);

	// �_���[�W��
	pSound->LoadWave2D(L"Asset\\Audio\\Damage_SE.wav", 1);

	// �v���C���[�U����
	pSound->LoadWave2D(L"Asset\\Audio\\HeroAttack_SE.wav", 2);

	// �v���C���[�̑��鉹
	pSound->LoadWave2D(L"Asset\\Audio\\Run_SE", 3);

	// �v���C���[�̕�����
	pSound->LoadWave2D(L"Asset\\Audio\\Walk_SE.wav", 4);

	// �G�̃W�����v��
	pSound->LoadWave2D(L"Asset\\Audio\\EnemyJump_SE.wav", 5);
}

CScene* CSceneMain::Update()
{
	CSound* pSound = CSound::GetInstance();

	CScene* pSceneNext = this;

	CTaskSystem* pTaskSystem = CTaskSystem::GetInstance();

	// �v���C���[��HP��0�ɂȂ�����A�Q�[���I�[�o�[��ʂ�
	if (*m_pPlayer->GetpHp() <= 0)
	{
		// ���݂���I�u�W�F�N�g��S�č폜
		pTaskSystem->ObjAllDelete();

		pSound->StopWave2D(0);

		// �����폜
		pSound->Delete();

		return new CSceneGameOver;
	}

	if (pTaskSystem->ObjectNum(typeid(CEnemy)) == 0)
	{
		// �G�����܂����E�F�[�u���|���ƃQ�[���N���A��ʂ�
		if (m_nEnemyAnnihilation == 2)
		{
			// ���݂���I�u�W�F�N�g�����ׂč폜
			pTaskSystem->ObjAllDelete();

			pSound->StopWave2D(0);

			// �����폜
			pSound->Delete();
			return new CSceneGameClear;
		}

		if (m_nEnemyAnnihilation % 2 == 0)
		{
			for (int i = -3; i <= 3; i++)
			{
				// �G�I�u�W�F�N�g�쐬
				CEnemy* pEnemy = new CEnemy(D3DXVECTOR3(10.f * i, 50.f, 10.f));
				pTaskSystem->AddObj(pEnemy);
			}
		}
		else
		{
			for (int i = -3; i <= 3; i++)
			{
				// �G�I�u�W�F�N�g�쐬
				CEnemy* pEnemy = new CEnemy(D3DXVECTOR3(10.f * i, 0.f, 30.f));
				pTaskSystem->AddObj(pEnemy);
			}
		}

		m_nEnemyAnnihilation++;
	}

	return pSceneNext;
}
