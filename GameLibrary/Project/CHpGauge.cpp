#include "CHpGauge.h"

#include "CDevice3D.h"
#include "CShader.h"
#include "CTextureManager.h"
#include "CInput.h"
#include "CCamera.h"
#include "Window.h"

#include "Collision.h"

// 引数1 int* : 体力情報が入ってるアドレス(参照したいHP情報を指定)
// 引数2 int  : 体力最大値
CHpGauge::CHpGauge(int* pHp, int HPMax)
{
	CTextureManager* pTex = CTextureManager::GetInstance();
	// テクスチャをロード
	pTex->CreateTexture(L"HpGauge.png");
	m_pHp = pHp;
	m_HpMax = HPMax;
}

void CHpGauge::Draw()
{
	CShader* pShader = CShader::GetInstance();
	CTextureManager* pTextureManager = CTextureManager::GetInstance();

	// 切り取り位置範囲設定用変数
	RectUV Rect;

	float HpRatio;
	if (m_pHp != 0)
	{
		// 体力の残り比率によって、画像の切り取り位置を変える(色を変える)
		HpRatio = 1.f - (float)*m_pHp / (float)m_HpMax;

		// 左端と右端の画像の色がおかしくなるので、画像切り取り範囲を変える
		if (HpRatio > 0.9f) HpRatio = 0.9f;
		else if (HpRatio < 0.1f) HpRatio = 0.1f;

		// 切り取り位置設定
		Rect = RectUV(0.f, HpRatio, HpRatio, 1.f);
	}

	// HPゲージ表示
	pShader->TextureDraw(10.f, 10.f, 150.f * ((float)*m_pHp / (float)m_HpMax), 30.f,
		pTextureManager->GetTexture(L"HpGauge.png"), Rect, false);
}
