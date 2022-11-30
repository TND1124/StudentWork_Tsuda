#include "CHpGauge.h"

#include "CDevice3D.h"
#include "CShader.h"
#include "CTextureManager.h"
#include "CInput.h"
#include "CCamera.h"
#include "Window.h"

#include "Collision.h"

// ����1 int* : �̗͏�񂪓����Ă�A�h���X(�Q�Ƃ�����HP�����w��)
// ����2 int  : �̗͍ő�l
CHpGauge::CHpGauge(int* pHp, int HPMax)
{
	CTextureManager* pTex = CTextureManager::GetInstance();
	// �e�N�X�`�������[�h
	pTex->CreateTexture(L"HpGauge.png");
	m_pHp = pHp;
	m_HpMax = HPMax;
}

void CHpGauge::Draw()
{
	CShader* pShader = CShader::GetInstance();
	CTextureManager* pTextureManager = CTextureManager::GetInstance();

	// �؂���ʒu�͈͐ݒ�p�ϐ�
	RectUV Rect;

	float HpRatio;
	if (m_pHp != 0)
	{
		// �̗͂̎c��䗦�ɂ���āA�摜�̐؂���ʒu��ς���(�F��ς���)
		HpRatio = 1.f - (float)*m_pHp / (float)m_HpMax;

		// ���[�ƉE�[�̉摜�̐F�����������Ȃ�̂ŁA�摜�؂���͈͂�ς���
		if (HpRatio > 0.9f) HpRatio = 0.9f;
		else if (HpRatio < 0.1f) HpRatio = 0.1f;

		// �؂���ʒu�ݒ�
		Rect = RectUV(0.f, HpRatio, HpRatio, 1.f);
	}

	// HP�Q�[�W�\��
	pShader->TextureDraw(10.f, 10.f, 150.f * ((float)*m_pHp / (float)m_HpMax), 30.f,
		pTextureManager->GetTexture(L"HpGauge.png"), Rect, false);
}
