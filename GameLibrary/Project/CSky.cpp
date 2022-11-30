#include "CSky.h"

#include "CShader.h"
#include "Collision.h"

CSky::CSky()
{
	m_Model.LoadNXModel(L"Asset\\Model\\sky.nx");

	m_vScale = D3DXVECTOR3(80.f, 80.f, 80.f);

	// ワールド行列作成
	CreateMatWorld();
}

void CSky::Draw()
{
	// 空を覆うオブジェクトを表示
	CShader* pShader = CShader::GetInstance();
	pShader->ModelDraw(&m_Model, m_matWorld, false);
}
