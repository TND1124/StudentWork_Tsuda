#pragma once

#include "CTaskSystem.h"
#include "CTextureManager.h"

// ゲームクリア背景表示クラス
class CGameClearBackGround : public CObj
{
public:
	CGameClearBackGround();
	~CGameClearBackGround() {}
	void Update() {}
	void Draw();

private:
	Texture m_BackGroundTexture;
};
