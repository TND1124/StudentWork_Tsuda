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

	// ゲームクリア背景オブジェクト作成
	CGameClearBackGround* pGameClearBackGounrd = new CGameClearBackGround;
	pTaskSystem->AddObj(pGameClearBackGounrd);

	// 音データ読み込み
	CSound* pSound = CSound::GetInstance();

	// ゲームクリア音
	pSound->LoadWave2D(L"Asset\\Audio\\GameClear_BGM.wav", 0);
	// BGMを流す
	pSound->PlayLoodWave2D(0);

	// 決定音
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

		// 音データ読み込み
		CSound* pSound = CSound::GetInstance();

		// 決定音を流す
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
