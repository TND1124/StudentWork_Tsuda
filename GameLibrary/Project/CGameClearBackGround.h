#pragma once

#include "CTaskSystem.h"
#include "CTextureManager.h"

// �Q�[���N���A�w�i�\���N���X
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
