#pragma once

#include "CSceneManager.h"

// シーン : ゲームクリア
class CSceneGameClear : public CScene
{
public:
	CSceneGameClear();
	~CSceneGameClear() {};
	CScene* Update();
};
