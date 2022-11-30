#include "CGameOverBackGround.h"

#include "CShader.h"

void CGameOverBackGround::Draw()
{
	CShader* pShader = CShader::GetInstance();

	// GameOver‚Ì•¶Žš•\Ž¦
	pShader->StrDraw(200.f, 200.f, 64.f, L"GameOver");

	pShader->StrDraw(200.f, 300.f, 64.f, L"Push Z");
}
