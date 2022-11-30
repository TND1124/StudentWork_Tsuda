#pragma once

#include "CTaskSystem.h"
#include "CTextureManager.h"

// �v���C���[��HP�Q�[�W���Ǘ�����N���X
class CHpGauge : public CObj
{
public:
	CHpGauge(int* pHp, int HPMax);
	~CHpGauge() {};
	void Update() {};
	void Draw();

private:
	Texture m_Texture; // �g�p����摜���

	int m_HpMax;	   // HP�ő�l
	int* m_pHp;		   // HP
};
