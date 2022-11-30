#pragma once

#include "CTaskSystem.h"
#include "CTextureManager.h"

// プレイヤーのHPゲージを管理するクラス
class CHpGauge : public CObj
{
public:
	CHpGauge(int* pHp, int HPMax);
	~CHpGauge() {};
	void Update() {};
	void Draw();

private:
	Texture m_Texture; // 使用する画像情報

	int m_HpMax;	   // HP最大値
	int* m_pHp;		   // HP
};
