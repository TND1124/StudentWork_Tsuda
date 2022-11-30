#include "CSceneTitle.h"
#include "CSceneMain.h"

#include "CInput.h"
#include "CSound.h"

#include "CTitleBackGround.h"

CSceneTitle::CSceneTitle()
{
	CTaskSystem* pTaskSystem = CTaskSystem::GetInstance();

	// �w�i�I�u�W�F�N�g��ǉ�
	CTitleBackGround* pTitleBackGounrd = new CTitleBackGround;
	pTaskSystem->AddObj(pTitleBackGounrd);

	CSound* pSound = CSound::GetInstance();
	// �^�C�g��BGM��
	pSound->LoadWave2D(L"Asset\\Audio\\Title_BGM.wav", 0);

	// BGM��炷
	pSound->PlayLoodWave2D(0);

	// ���艹
	pSound->LoadWave2D(L"Asset\\Audio\\Intermediate_SE.wav", 1);
}

CScene* CSceneTitle::Update()
{
	CScene* pSceneNext = this;
	CInput* pInput = CInput::GetInstance();

	if (pInput->GetKeyDown('Z') == true)
	{
		CTaskSystem* pTaskSystem = CTaskSystem::GetInstance();
		// ���݂���I�u�W�F�N�g�����ׂč폜
		pTaskSystem->ObjAllDelete();
		// �L�[���͏���������
		pInput->KeyInit();

		CSound* pSound = CSound::GetInstance();

		pSound->PlayWave2D(1);

		// BGM���~�߂�
		pSound->StopWave2D(0);

		// �����폜
		pSound->Delete();

		// �Q�[���V�[���ɐ؂�ւ���
		pSceneNext = new CSceneMain;
	}

	return pSceneNext;
}
