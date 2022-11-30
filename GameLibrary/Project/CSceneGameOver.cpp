#include "CSceneGameOver.h"
#include "CSceneTitle.h"

#include "CInput.h"
#include "CSound.h"

#include "CGameOverBackGround.h"

CSceneGameOver::CSceneGameOver()
{
	CTaskSystem* pTaskSystem = CTaskSystem::GetInstance();

	// �Q�[���I�[�o�[�w�i�I�u�W�F�N�g�쐬
	CGameOverBackGround* pGameOverBackGounrd = new CGameOverBackGround;
	pTaskSystem->AddObj(pGameOverBackGounrd);

	// ����� �ǂݍ���
	CSound* pSound = CSound::GetInstance();

	// �Q�[���I�[�o�[BGM
	pSound->LoadWave2D(L"Asset\\Audio\\GameOver_BGM.wav", 0);
	pSound->PlayLoodWave2D(0);
	
	// ���艹
	pSound->LoadWave2D(L"Asset\\Audio\\Intermediate_SE.wav", 1);
}

CScene* CSceneGameOver::Update()
{
	CScene* pSceneNext = this;
	CInput* pInput = CInput::GetInstance();
	if (pInput->GetKeyDown('Z') == true)
	{
		CTaskSystem* pTaskSystem = CTaskSystem::GetInstance();
		pTaskSystem->ObjAllDelete();

		// ���艹�𗬂�
		CSound* pSound = CSound::GetInstance();
		pSound->PlayWave2D(1);

		// BGM���~�߂�
		pSound->StopWave2D(0);

		// �����폜
		pSound->Delete();

		// �^�C�g����ʂɖ߂�
		pSceneNext = new CSceneTitle;
	}

	return pSceneNext;
}
