#include "CSceneGameClear.h"
#include "CSceneTitle.h"

#include "CInput.h"
#include "CSound.h"
#include "CTextureManager.h"

#include "CGameClearBackGround.h"

CSceneGameClear::CSceneGameClear()
{
	CTextureManager* pTextureManager = CTextureManager::GetInstance();
	pTextureManager->CreateTexture(L"GameClear.jpeg");

	CTaskSystem* pTaskSystem = CTaskSystem::GetInstance();

	// �Q�[���N���A�w�i�I�u�W�F�N�g�쐬
	CGameClearBackGround* pGameClearBackGounrd = new CGameClearBackGround;
	pTaskSystem->AddObj(pGameClearBackGounrd);

	// ���f�[�^�ǂݍ���
	CSound* pSound = CSound::GetInstance();

	// �Q�[���N���A��
	pSound->LoadWave2D(L"Asset\\Audio\\GameClear_BGM.wav", 0);
	// BGM�𗬂�
	pSound->PlayLoodWave2D(0);

	// ���艹
	pSound->LoadWave2D(L"Asset\\Audio\\Intermediate_SE.wav", 1);
}

CScene* CSceneGameClear::Update()
{
	CScene* pSceneNext = this;
	CInput* pInput = CInput::GetInstance();
	if (pInput->GetKeyDown('Z') == true)
	{
		CTaskSystem* pTaskSystem = CTaskSystem::GetInstance();
		pTaskSystem->ObjAllDelete();

		// ���f�[�^�ǂݍ���
		CSound* pSound = CSound::GetInstance();

		// ���艹�𗬂�
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
