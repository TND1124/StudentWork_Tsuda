#include "CPlayer.h"

#include "CShader.h"
#include "CInput.h"

#include "Cyuka.h"
#include "CSky.h"
#include "CCamera.h"
#include "CEnemy.h"
#include "CModelManager.h"

#include "CSound.h"

extern CCamera* g_pCamera;

CPlayer::CPlayer()
{
	CModelManager* pModelManger = CModelManager::GetInstance();
	m_Model = pModelManger->GetModel(L"player.nx");
	m_ColObb = *(m_Model->GetObb());

	m_Velocity = D3DXVECTOR3(0.f, 0.f, 0.f);
	m_MoveSpeed = 1.f;
	
	// �L�����N�^�[�Ɏ������錕
	m_Sword = new CSword;
	
	// �^�X�N�ɒǉ�
	CTaskSystem* pTaskSystem = CTaskSystem::GetInstance();
	pTaskSystem->AddObj(m_Sword);

	m_Hp = m_HpMax      = 100;
	m_SlashSwordFlag	= false;
	m_nInvincibleTime	= 0;
	m_DamageAniMoveFlag = false;

	m_fIdelAniFrame   = 0.f;
	m_fRunAniFrame	  = 0.f;
	m_fSwordAniFrame  = 0.f;
	m_fWalkAniFrame	  = 0.f;
}

void CPlayer::Update()
{
	// ���G���Ԓ��Ȃ�A���G�L�����Ԃ����炷
	if (m_nInvincibleTime != 0)
	{
		m_nInvincibleTime--;
	}
	
	CInput* pInput = CInput::GetInstance();
	// ����U��Ƃ������͂���x�������
	if (pInput->GetKeyDown('Z') == true)
	{
		if (m_SlashSwordFlag == false)
		{
			CSound* pSound = CSound::GetInstance();
			pSound->PlayWave2D(2);
		}

		m_SlashSwordFlag = true;
	}

	// �ړ�����
	if (m_SlashSwordFlag == false)
	{
		Move();
	}

	// ����U��Ƃ������͂��󂯎������A����U��A�j���[�V�������I���܂Ői�߂�
	if (m_SlashSwordFlag == true)
	{
		// ����U��
		SwordSlash();
	}
	else
	{
		// �����蔻�菈��
		Collsion();
	}

	// �L�����N�^�[���J�����̕����Ɍ�����
	const D3DXVECTOR3* pRadian = g_pCamera->GetRadian();
	// pvRadian->x�͌��݂̃J�����̊p�x 3.14�̓L�����N�^�[���J�����̏��������ɍ��킹��(180.f / 180.f * 3.14f�̂���)
	D3DXQuaternionRotationYawPitchRoll(&m_qRotate, pRadian->x + 3.14f, 0.f, 0.f);

	// ���[���h�s��쐬
	CreateMatWorld();


	// ���̈ʒu�X�V
	{
		D3DXMATRIX matSwordWorld;
		matSwordWorld = m_Model->GetBone(50)->m_matOffset;
		matSwordWorld._41 = -matSwordWorld._41;
		matSwordWorld._42 = -matSwordWorld._42;

		D3DXMATRIX matAniSwordWorld = m_Model->GetBone(50)->m_matAnimation;
		matSwordWorld = matSwordWorld * matAniSwordWorld * m_matWorld;

		// �����ړ�������
		m_Sword->Move(matSwordWorld);
	}


	D3DXVECTOR3 vPlayerEye = m_vPosition;
	vPlayerEye.y += 2.00f;
	
	g_pCamera->SetEye(vPlayerEye);

	vPlayerEye.y -= 0.5f;
	g_pCamera->SetLookAt(vPlayerEye);

	// �J�������X�V
	g_pCamera->Update();
}

void CPlayer::Draw()
{
	CShader* pShader = CShader::GetInstance();
	if (m_nInvincibleTime % 2 == 0)
	{
		pShader->ModelDraw(m_Model, m_matWorld, false);
	}

	// ����U���Ă���Ԃ́A�����蔻����o���Ȃ�
	if (m_SlashSwordFlag == false)
	{
		// �����蔻��`��
		Collision::DrawDebugOBB(m_ColObb);
	}

	// �e��t����
	pShader->ModelShadowDraw(m_Model, m_matWorld);

	// ��l���̈ʒu�\���f�o�b�O�p
#ifdef _DEBUG
	wchar_t str[256];
	swprintf_s(str, L"PlayerX : %f", m_vPosition.x);
	pShader->StrDraw(400.f, 0.f, 24.f, str, 0.f);
	swprintf_s(str, L"PlayerY : %f", m_vPosition.y);
	pShader->StrDraw(400.f, 24.f, 24.f, str, 0.f);
	swprintf_s(str, L"PlayerZ : %f", m_vPosition.z);
	pShader->StrDraw(400.f, 48.f, 24.f, str, 0.f);
#endif 

	pShader->StrDraw(0.f, 80.f, 32.f, L"Z�L�[�ōU��");
	pShader->StrDraw(0.f, 112.f, 32.f, L"X�L�[�ŃJ������](��)");
	pShader->StrDraw(0.f, 144.f, 32.f, L"C�L�[�ŃJ������](�E)");
	pShader->StrDraw(0.f, 176.f, 32.f, L"���L�[�ňړ�");
	pShader->StrDraw(0.f, 208.f, 32.f, L"���L�[ + Shift�Ń_�b�V��");
	
}

// �ړ�����
void CPlayer::Move()
{
	// ���͏���------------------------------------------

	CInput* pInput = CInput::GetInstance();
	if (pInput->GetKey(VK_LEFT) == true)
	{
		m_Velocity.x -= 1.f;
	}
	if (pInput->GetKey(VK_DOWN) == true)
	{
		m_Velocity.z -= 1.f;
	}
	if (pInput->GetKey(VK_RIGHT) == true)
	{
		m_Velocity.x += 1.f;
	}
	if (pInput->GetKey(VK_UP) == true)
	{
		m_Velocity.z += 1.f;
	}

	// �ړ����͂���Ă����
	if (m_Velocity.x != 0.f || m_Velocity.z != 0.f)
	{
		D3DXVECTOR3 vCamera;
		vCamera = *g_pCamera->GetLookAt() - *g_pCamera->GetEye();
		D3DXVec3Normalize(&vCamera, &vCamera);

		float cos_vec, sin_vec;
		float radian = (-90.f / 180.f) * 3.14f;
		cos_vec = m_Velocity.x * cosf(radian) - m_Velocity.z * sinf(radian);
		sin_vec = m_Velocity.z * cosf(radian) + m_Velocity.x * sinf(radian);

		m_Velocity.x = vCamera.x * cos_vec - vCamera.z * sin_vec;
		m_Velocity.z = vCamera.z * cos_vec + vCamera.x * sin_vec;

		D3DXVec3Normalize(&m_Velocity, &m_Velocity);

		// ����
		if (pInput->GetKey(VK_SHIFT) == true)
		{
			m_MoveSpeed = 0.3f;

			// ����A�j���[�V�����Đ�
			// �X�L���A�j���[�V�����Ǘ�------
			m_fRunAniFrame += 0.5f;
			if (m_fRunAniFrame >= m_Model->GetAnimationFrame((int)AnimationKey::Run))
			{
				m_fRunAniFrame = 0.f;
			}
			m_Model->SkinAnimation(m_fRunAniFrame, (int)AnimationKey::Run);
			// ------------------------------
		}
		else
		{
			m_MoveSpeed = 0.1f;

			// �ړ��A�j���[�V�����Đ�
			// �X�L���A�j���[�V�����Ǘ�------
			m_fWalkAniFrame += 0.5f;
			if (m_fWalkAniFrame >= m_Model->GetAnimationFrame((int)AnimationKey::Walk))
			{
				m_fWalkAniFrame = 0.f;
			}
			m_Model->SkinAnimation(m_fWalkAniFrame, (int)AnimationKey::Walk);
			// ------------------------------
		}

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
			m_vPosition.x += m_Velocity.x * m_MoveSpeed;
			m_vPosition.z += m_Velocity.z * m_MoveSpeed;
		}
		m_Velocity = D3DXVECTOR3(0.f, 0.f, 0.f);
	}
	else
	{
		m_fIdelAniFrame += 0.5f;
		if (m_fIdelAniFrame >= m_Model->GetAnimationFrame((int)AnimationKey::Idle))
		{
			m_fIdelAniFrame = 0.f;
		}
		m_Model->SkinAnimation(m_fIdelAniFrame, (int)AnimationKey::Idle);
	}

	// -------------------------------------------------
}

// �����蔻��n�̏���
void CPlayer::Collsion()
{
	// �����蔻��̑傫������
	D3DXVECTOR3 vObbColScale = D3DXVECTOR3(m_vScale.x / 2.f, m_vScale.y, m_vScale.z);

	// �����蔻��X�V
	m_ColObb.Update(vObbColScale, m_qRotate, m_vPosition);

	std::vector<std::shared_ptr<CObj>> ObjData;	// �~�����f�[�^������x�N�^�[
	std::vector<std::shared_ptr<CObj>> swap;    // �x�N�^�[�̃���������p

	CTaskSystem* pTaskSystem = CTaskSystem::GetInstance();

	// ���G���łȂ���΁A
	if (m_nInvincibleTime == 0)
	{
		// �G�ɓ�����ƃ_���[�W
		pTaskSystem->ObjsInfomation(typeid(CEnemy), &ObjData);
		for (auto ip = ObjData.begin(); ip != ObjData.end(); ip++)
		{
			CEnemy* pEnemy = (CEnemy*)(ip->get());
			if (Collision::HitObb3D(m_ColObb, *pEnemy->GetObb()) == true)
			{
				// �_���[�W��
				CSound* pSound = CSound::GetInstance();
				pSound->PlayWave2D(1);

				m_nInvincibleTime = 100;
				m_Hp -= 10;
				m_DamageAniMoveFlag = true;
				break;
			}
		}
		ObjData.swap(swap);
	}

	D3DXVECTOR3 vGravity = D3DXVECTOR3(0.f, -0.3f, 0.f);
	D3DXVECTOR3 GravityOffset = D3DXVECTOR3(0.f, 1.f, 0.f);

	// �d�͂ŏ��Ƃ̏Փ˔���ɕK�v�Ȑ������쐬
	m_ColSegment.m_vStartPosition = m_vPosition + GravityOffset;
	m_ColSegment.m_vEndPosition = m_vPosition + vGravity;

	pTaskSystem->ObjsInfomation(typeid(Cyuka), &ObjData);
	Cyuka* pYuka = (Cyuka*)(ObjData.begin())->get();
	ObjData.swap(swap);

	// �d�̓x�N�g���Ə��Ƃ̏Փ˔���
	D3DXVECTOR3 out;
	if (Collision::HitSegmentAndModel(m_ColSegment, *pYuka->GetModel(), *pYuka->GetMatWorld(), &out) == true)
	{
		// �������Ă����
		m_vPosition += out + GravityOffset;
	}
	else
	{
		// �d�͂����̂܂܂�����
		m_vPosition += vGravity;
	}
}

// ����U�鏈��
void CPlayer::SwordSlash()
{
	// ����U��A�j���[�V������i�߂�
	m_fSwordAniFrame += 0.5f;
	if (m_fSwordAniFrame >= m_Model->GetAnimationFrame((int)AnimationKey::Slash))
	{
		m_fSwordAniFrame = 0.f;
		m_SlashSwordFlag = false;
	}
	// �A�j���[�V����������
	m_Model->SkinAnimation(m_fSwordAniFrame, (int)AnimationKey::Slash);

	// ���̓����蔻���L���ɂ���
	m_Sword->Collision();
}
