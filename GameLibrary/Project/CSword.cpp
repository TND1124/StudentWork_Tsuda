#include "CSword.h"

#include "CShader.h"
#include "CInput.h"
#include "CModelManager.h"

#include "CEnemy.h"

CSword::CSword()
{
	CModelManager* pModelManager = CModelManager::GetInstance();
	m_Model = pModelManager->GetModel(L"sword.nx");
	m_ColObb = *(m_Model->GetObb());
	
	m_vScale = D3DXVECTOR3(0.2f, 0.2f, 0.2f);
	m_isHitBoxDraw = false;
}

void CSword::Draw()
{
	if (m_isHitBoxDraw == true)
	{
		// �����蔻��\��
		Collision::DrawDebugOBB(m_ColObb);
		m_isHitBoxDraw = false;
	}

	CShader* pShader = CShader::GetInstance();
	pShader->ModelDraw(m_Model, m_matWorld, false);

	// �e��t����
	pShader->ModelShadowDraw(m_Model, m_matWorld);
}

// �����蔻��n�̏���
void CSword::Collision()
{
	// �����蔻��\���t���O�𗧂Ă�
	m_isHitBoxDraw = true;

	// �����蔻��̑傫������
	D3DXVECTOR3 vObbColScale = D3DXVECTOR3(m_vScale.x, m_vScale.y, m_vScale.z);

	D3DXQUATERNION qRotate;
	D3DXMATRIX matWorld;

	// �X�P�[�����Ɖ�]�����|�����킵���f�[�^�����]�������o��
	matWorld._11 = m_matWorld._11 / m_vScale.x;
	matWorld._12 = m_matWorld._12 / m_vScale.y;
	matWorld._13 = m_matWorld._13 / m_vScale.z;
	matWorld._14 = 0.f;
	matWorld._21 = m_matWorld._21 / m_vScale.x;
	matWorld._22 = m_matWorld._22 / m_vScale.y;
	matWorld._23 = m_matWorld._23 / m_vScale.z;
	matWorld._24 = 0.f;
	matWorld._31 = m_matWorld._31 / m_vScale.x;
	matWorld._32 = m_matWorld._32 / m_vScale.y;
	matWorld._33 = m_matWorld._33 / m_vScale.z;
	matWorld._34 = 0.f;
	matWorld._41 = 0.f;
	matWorld._42 = 0.f;
	matWorld._43 = 0.f;
	matWorld._44 = 1.f;

	D3DXQuaternionRotationMatrix(&qRotate, &matWorld);
	D3DXQuaternionMultiply(&qRotate, &m_qRotate, &qRotate);

	D3DXVECTOR3 vTranslate = D3DXVECTOR3(m_matWorld._41, m_matWorld._42, m_matWorld._43);

	m_ColObb.Update(vObbColScale, qRotate, vTranslate);

	std::vector<std::shared_ptr<CObj>> EnemysData;

	// ���ׂĂ̓G�̏����擾
	CTaskSystem* pTaskSystem = CTaskSystem::GetInstance();
	pTaskSystem->ObjsInfomation(typeid(CEnemy), &EnemysData);

	for (auto ip = EnemysData.begin(); ip != EnemysData.end(); ip++)
	{
		CEnemy* pEnemy = (CEnemy*)ip->get();
		if (Collision::HitObb3D(m_ColObb, *pEnemy->GetObb()) == true)
		{
			// �G�̍폜�t���O�𗧂Ă�
			pEnemy->SetDefeatFlag(true);
		}
	}
}

void CSword::Move(const D3DXMATRIX& matWorld)
{
	// ���̃I�u�W�F�N�g�̃��[���h�s��쐬
	CreateMatWorld();

	// �v���C���[�̃{�[���ɍ��킹��
	m_matWorld = m_matWorld * matWorld;
}
