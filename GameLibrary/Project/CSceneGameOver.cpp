#include "CSceneGameOver.h"
#include "CSceneTitle.h"

#include "CInput.h"
#include "CSound.h"

#include "CGameOverBackGround.h"

CSceneGameOver::CSceneGameOver()
{
	CTaskSystem* pTaskSystem = CTaskSystem::GetInstance();

	// ゲームオーバー背景オブジェクト作成
	CGameOverBackGround* pGameOverBackGounrd = new CGameOverBackGround;
	pTaskSystem->AddObj(pGameOverBackGounrd);

	// 音情報 読み込み
	CSound* pSound = CSound::GetInstance();

	// ゲームオーバーBGM
	pSound->LoadWave2D(L"Asset\\Audio\\GameOver_BGM.wav", 0);
	pSound->PlayLoodWave2D(0);
	
	// 決定音
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

		// 決定音を流す
		CSound* pSound = CSound::GetInstance();
		pSound->PlayWave2D(1);

		// BGMを止める
		pSound->StopWave2D(0);

		// 音情報削除
		pSound->Delete();

		// タイトル画面に戻る
		pSceneNext = new CSceneTitle;
	}

	return pSceneNext;
}
