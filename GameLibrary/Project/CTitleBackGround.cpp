#include "CTitleBackGround.h"

#include "CTextureManager.h"
#include "CShader.h"

#include "Window.h"

CTitleBackGround::CTitleBackGround()
{
	CTextureManager* pTex = CTextureManager::GetInstance();
	pTex->CreateTexture(L"Title.jpeg");
	pTex->CreateTexture(L"HpGauge.png");
}

void CTitleBackGround::Update()
{

}

void CTitleBackGround::Draw()
{
	CTextureManager* pTextureManager = CTextureManager::GetInstance();
	
	CShader* pShader = CShader::GetInstance();

	RectUV Rect;

	// タイトル画像を描画
	pShader->TextureDraw(0.f, 0.f, SCREEN_WIDTH, SCREEN_HEIGHT,
		pTextureManager->GetTexture(L"Title.jpeg"), false, 0.01f);

	pShader->StrDraw(40.f, 240.f, 64.f, L"オリジナルゲーム描画エンジン", 0.f);

	pShader->StrDraw(150.f, 120.f, 64.f, L"GalaxyStar", 0.f);

	pShader->StrDraw(300.f, 360.f, 36.f, L"Push Z", 0.f);

	pShader->StrDraw(250.f, 450.f, 48.f, L"Produced by 津田", 0.f);
}
