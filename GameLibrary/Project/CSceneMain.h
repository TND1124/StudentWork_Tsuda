#pragma once

#include "CSceneManager.h"
#include "CPlayer.h"

// �V�[�� : �Q�[�����
class CSceneMain : public CScene
{
public:
	CSceneMain();
	~CSceneMain() {};
	CScene* Update();

private:
	CPlayer* m_pPlayer;
	// �G�̑S�ł�������
	int m_nEnemyAnnihilation;
};
