#include "CGameClearBackGround.h"

#include "CShader.h"
#include "Window.h"

CGameClearBackGround::CGameClearBackGround()
{
	CTextureManager* pTextureManager = CTextureManager::GetInstance();
	m_BackGroundTexture = pTextureManager->GetTexture(L"GameClear.jpeg");
}

void CGameClearBackGround::Draw()
{
	CShader* pShader = CShader::GetInstance();

	D3DXVECTOR4 vColor(1.f, 1.f, 0.f, 1.f);

	pShader->TextureDraw(0.f, 0.f, SCREEN_WIDTH, SCREEN_HEIGHT, m_BackGroundTexture, false, 0.01f);
	
	// GameOver‚Ì•¶Žš•\Ž¦
	pShader->StrDraw(200.f, 150.f, 64.f, L"GameClear", 0.f);

	pShader->StrDraw(200.f, 300.f, 64.f, L"Push Z", 0.f);
}
