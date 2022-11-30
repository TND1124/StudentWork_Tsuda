#pragma once

#include "CSceneManager.h"

// シーン : タイトル画面
class CSceneTitle : public CScene
{
public:
	CSceneTitle();
	~CSceneTitle() {};
	CScene* Update();
};
