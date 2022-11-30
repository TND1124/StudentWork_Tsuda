#pragma once

#include "CSceneManager.h"

// シーン : ゲームオーバー
class CSceneGameOver : public CScene
{
public:
	CSceneGameOver();
	~CSceneGameOver() {};
	CScene* Update();
};
