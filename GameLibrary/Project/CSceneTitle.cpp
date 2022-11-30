#include "CSceneTitle.h"
#include "CSceneMain.h"

#include "CInput.h"
#include "CSound.h"

#include "CTitleBackGround.h"

CSceneTitle::CSceneTitle()
{
	CTaskSystem* pTaskSystem = CTaskSystem::GetInstance();

	// 背景オブジェクトを追加
	CTitleBackGround* pTitleBackGounrd = new CTitleBackGround;
	pTaskSystem->AddObj(pTitleBackGounrd);

	CSound* pSound = CSound::GetInstance();
	// タイトルBGM音
	pSound->LoadWave2D(L"Asset\\Audio\\Title_BGM.wav", 0);

	// BGMを鳴らす
	pSound->PlayLoodWave2D(0);

	// 決定音
	pSound->LoadWave2D(L"Asset\\Audio\\Intermediate_SE.wav", 1);
}

CScene* CSceneTitle::Update()
{
	CScene* pSceneNext = this;
	CInput* pInput = CInput::GetInstance();

	if (pInput->GetKeyDown('Z') == true)
	{
		CTaskSystem* pTaskSystem = CTaskSystem::GetInstance();
		// 現在あるオブジェクトをすべて削除
		pTaskSystem->ObjAllDelete();
		// キー入力情報を初期化
		pInput->KeyInit();

		CSound* pSound = CSound::GetInstance();

		pSound->PlayWave2D(1);

		// BGMを止める
		pSound->StopWave2D(0);

		// 音情報削除
		pSound->Delete();

		// ゲームシーンに切り替える
		pSceneNext = new CSceneMain;
	}

	return pSceneNext;
}
