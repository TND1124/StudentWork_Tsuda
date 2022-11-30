#include "CEnemy.h"

#include "CDevice3D.h"
#include "CShader.h"
#include "CSound.h"

#include "CPlayer.h"
#include "Cyuka.h"
#include "CSky.h"
#include "CSword.h"
#include "CModelManager.h"

// �W�����v����^�C�~���O
#define JUMP_TIMING (150)

CEnemy::CEnemy(const D3DXVECTOR3& vPosition)
{
	CModelManager* pModelManager = CModelManager::GetInstance();
	m_pModel = pModelManager->GetModel(L"JumpEnemy.nx");

	m_ColObb = *(m_pModel->GetObb());

	m_vPosition = vPosition;
	m_Velocity = D3DXVECTOR3(0.f, 0.f, 0.f);
	m_MoveSpeed = 0.05f;
	m_jump_time = 0;

	m_vGravity = D3DXVECTOR3(0.f, 0.f, 0.f);

	m_isDefeat = false;
	m_DefeatRadian = 0.f;

	// �v���C���[�����擾
	CTaskSystem* pTaskSystem = CTaskSystem::GetInstance();
	std::vector<std::shared_ptr<CObj>> vecPlayer;
	pTaskSystem->ObjsInfomation(typeid(CPlayer), &vecPlayer);
	CPlayer* pPlayer = (CPlayer*)(vecPlayer.begin())->get();
	std::vector<std::shared_ptr<CObj>> swap;
	vecPlayer.swap(swap);

	// �v���C���[�̈ʒu�����擾
	m_pvPlayerPosition = pPlayer->GetTranslate();

	// �����蔻��X�V
	m_ColObb.Update(m_vScale, m_qRotate, m_vPosition);
}

void CEnemy::Update()
{
	// �|���ꂽ��
	if (m_isDefeat == true)
	{
		// �|���A�j���[�V�������쐬
		m_DefeatRadian += 0.05f;
		D3DXVECTOR3 axis(1.f, 0.f, 0.f);
		D3DXQUATERNION q;
		D3DXQuaternionRotationAxis(&q, &axis, 0.05f);

		D3DXQuaternionMultiply(&m_qRotate, &q, &m_qRotate);

		if (m_DefeatRadian * 180.f / 3.14 > 90.f)
		{
			ObjDelete();
		}
	}
	else
	{
		// �ړ�����
		Move();

		// �����蔻�菈��
		Collsion();
	}

	// ���[���h�s��쐬
	CreateMatWorld();
}

void CEnemy::Draw()
{
	CShader* pShader = CShader::GetInstance();
	pShader->ModelDraw(m_pModel, m_matWorld, false);

	// �|���ꂽ�Ƃ��́A�����蔻��\����؂�)
	if (m_isDefeat == false)
	{
		Collision::DrawDebugOBB(m_ColObb);
	}

	// �e��t����
	pShader->ModelShadowDraw(m_pModel, m_matWorld);
}

// �ړ�����
void CEnemy::Move()
{
	// �W�����v������ 
	if (m_jump_time == JUMP_TIMING)
	{
		m_Velocity.y = 50.f;
		m_jump_time = 0;

		CSound* pSound = CSound::GetInstance();
		pSound->PlayWave2D(5);
	}
	else
	{
		m_jump_time++;
	}

	// �v���C���[��ǂ�������
	float radian = atan2f(m_pvPlayerPosition->z - m_vPosition.z, m_pvPlayerPosition->x - m_vPosition.x);
	
	m_Velocity.x = cos(radian);
	m_Velocity.z = sin(radian);

	D3DXVECTOR3 vAxis = D3DXVECTOR3(0.f, 1.f, 0.f);

	// �v���C���[�̕���������
	D3DXQuaternionRotationAxis(&m_qRotate, &vAxis, -radian + 270.f / 180.f * 3.14f);

	D3DXVECTOR3 MoveOffset = -m_Velocity;

	// �ړ����ł��邩�ǂ���
	m_ColSegment.m_vStartPosition = m_vPosition + D3DXVECTOR3(0.f, 0.5f, 0.f) + MoveOffset;
	m_ColSegment.m_vEndPosition = m_vPosition + D3DXVECTOR3(0.f, 0.5f, 0.f) + m_Velocity * m_MoveSpeed;

	CTaskSystem* pTaskSystem = CTaskSystem::GetInstance();
	std::vector<std::shared_ptr<CObj>> sky;
	pTaskSystem->ObjsInfomation(typeid(CSky), &sky);
	CSky* pSky = (CSky*)(sky.begin())->get();
	std::vector<std::shared_ptr<CObj>> swap;
	sky.swap(swap);

	// �ǂɂԂ����Ă��Ȃ����ǂ����𒲂ׂ�
	D3DXVECTOR3 out;
	if (Collision::HitSegmentAndModel(m_ColSegment, *pSky->GetModel(), *pSky->GetMatWorld(), &out) == true)
	{
		// �Փ�
		// �Ԃ������ꏊ�܂ňړ�
		m_vPosition += out + MoveOffset;
	}
	else
	{
		// �Փ˂��ĂȂ�
		// �ړ�
		m_vPosition += m_Velocity * m_MoveSpeed;
	}
	m_Velocity = D3DXVECTOR3(0.f, 0.f, 0.f);
}

// �����蔻��n�̏���
void CEnemy::Collsion()
{
	// �d�͂̒l�X�V
	m_vGravity.y += -0.0098f;

	// �����蔻��X�V
	m_ColObb.Update(m_vScale, m_qRotate, m_vPosition);

	D3DXVECTOR3 vGravityOffset = D3DXVECTOR3(0.f, 1.f, 0.f);

	// �d�͂ŏ��Ƃ̏Փ˔���ɕK�v�Ȑ������쐬
	m_ColSegment.m_vStartPosition = m_vPosition + vGravityOffset;
	m_ColSegment.m_vEndPosition = m_vPosition + m_vGravity;

	// �����蔻��𒲂ׂ������I�u�W�F�N�g���擾
	CTaskSystem* pTaskSystem = CTaskSystem::GetInstance();
	std::vector<std::shared_ptr<CObj>> ObjData;
	pTaskSystem->ObjsInfomation(typeid(Cyuka), &ObjData);
	Cyuka* Yuka = (Cyuka*)(ObjData.begin())->get();
	std::vector<std::shared_ptr<CObj>> swap;
	ObjData.swap(swap);

	// �d�̓x�N�g���Ə��Ƃ̏Փ˔���
	D3DXVECTOR3 out;
	if (Collision::HitSegmentAndModel(m_ColSegment, *Yuka->GetModel(),
		*Yuka->GetMatWorld(), &out) == true)
	{
		// �������Ă����
		m_vPosition += out + vGravityOffset;

		m_vGravity.y = 0.f;
	}
	else
	{
		// �d�͂����̂܂܂�����
		m_vPosition += m_vGravity;
	}
}
