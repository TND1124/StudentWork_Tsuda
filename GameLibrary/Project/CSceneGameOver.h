#pragma once

#include "CSceneManager.h"

// �V�[�� : �Q�[���I�[�o�[
class CSceneGameOver : public CScene
{
public:
	CSceneGameOver();
	~CSceneGameOver() {};
	CScene* Update();
};
