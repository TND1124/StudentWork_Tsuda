#include "Cyuka.h"

#include "CShader.h"
#include "Collision.h"

Cyuka::Cyuka()
{
	m_Model.LoadNXModel(L"Asset\\Model\\zimen.nx");

	m_vScale = D3DXVECTOR3(100.f, 100.f, 100.f);


	D3DXMATRIX matScale, matTrans;
	D3DXMatrixScaling(&matScale, m_vScale.x, m_vScale.y, m_vScale.z);
	D3DXMatrixTranslation(&matTrans, m_vPosition.x, m_vPosition.y, m_vPosition.z);

	m_matWorld = matScale * matTrans;
}

void Cyuka::Update()
{
	
}

void Cyuka::Draw()
{
	CShader* pShader = CShader::GetInstance();
	pShader->ModelDraw(&m_Model, m_matWorld, false);
}
