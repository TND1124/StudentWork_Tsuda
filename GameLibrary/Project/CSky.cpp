#include "CSky.h"

#include "CShader.h"
#include "Collision.h"

CSky::CSky()
{
	m_Model.LoadNXModel(L"Asset\\Model\\sky.nx");

	m_vScale = D3DXVECTOR3(80.f, 80.f, 80.f);

	// ���[���h�s��쐬
	CreateMatWorld();
}

void CSky::Draw()
{
	// ��𕢂��I�u�W�F�N�g��\��
	CShader* pShader = CShader::GetInstance();
	pShader->ModelDraw(&m_Model, m_matWorld, false);
}
