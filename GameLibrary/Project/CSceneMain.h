#pragma once

#include "CSceneManager.h"
#include "CPlayer.h"

// シーン : ゲーム画面
class CSceneMain : public CScene
{
public:
	CSceneMain();
	~CSceneMain() {};
	CScene* Update();

private:
	CPlayer* m_pPlayer;
	// 敵の全滅させた数
	int m_nEnemyAnnihilation;
};
