#include "CCamera.h"
#include "Window.h"

#include "CInput.h"

// ��p
#define VIEW_SCOPE (180.f / 3.f)

void CCamera::Init(const D3DXVECTOR3& vEye, const D3DXVECTOR3& vLook, const D3DXVECTOR3& vUp)
{
	CInput* pInput = CInput::GetInstance();

	m_vEye = vEye;
	m_vLook = vLook;
	m_vUp = vUp;

	m_vRadian = D3DXVECTOR3(0.f, 0.f, 0.f);

	m_isTps = false;
	m_fRadian = 0.f;
}

void CCamera::Update()
{
	CInput* pInput = CInput::GetInstance();

	if (m_isTps == true)
	{
		// TPS�p�̃J�����ɐݒ�
		// �J��������]������
		if (pInput->GetKey('X') == true)
		{
			m_fRadian -= 0.01f;
		}
		else if (pInput->GetKey('C') == true)
		{
			m_fRadian += 0.01f;
		}

		// ��l���𒆐S�ɉ���]
		D3DXVECTOR3 vAxis(0.f, 1.f, 0.f);
		D3DXMATRIX matRotation;
		D3DXMatrixRotationAxis(&matRotation, &vAxis, m_fRadian);

		// ��l�����班������
		D3DXVECTOR3 vTrans(0.f, 0.f, -2.f);
		D3DXVec3TransformCoord(&vTrans, &vTrans, &matRotation);
		m_vEye += vTrans;
	}

	// �r���[�s��쐬
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vLook, &m_vUp);

	// ��ʔ䗦
	float AspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
	
	// �v���W�F�N�V�����g�����X�t�H�[���i�ˉe�ϊ��j
	D3DXMatrixPerspectiveFovLH(&m_matProj, AngleConvertRadian(VIEW_SCOPE), AspectRatio, 0.1f, 10000.0f);
}

// �J��������]������
// ����1 const D3DXVECTOR3& : ��]���Ăق���XYZ���ꂼ��̊p�x(radian)
void CCamera::Rotation(const D3DXVECTOR3& vRotation)
{
	D3DXQUATERNION Quaternion;
	D3DXMATRIX matRotation;

	// XYZ�̉�]���Ăق����p�x(���W�A��)���N�H�[�^�j�I���ϊ�
	D3DXQuaternionRotationYawPitchRoll(&Quaternion, vRotation.x, vRotation.y, vRotation.z);
	D3DXMatrixRotationQuaternion(&matRotation, &Quaternion);
	
	D3DXVec3TransformCoord(&m_vLook, &m_vLook, &matRotation);
	
	m_vLook += m_vEye;
}

// �r���{�[�h�p�̃J�����̂ق��������s��
const D3DXMATRIX CCamera::GetMatInvView() const
{
	D3DXMATRIX matInv;
	D3DXVECTOR3 vInvLook;
	vInvLook = m_vEye - m_vLook;

	D3DXMatrixIdentity(&matInv);
	D3DXMatrixLookAtLH(&matInv, &D3DXVECTOR3(0.f, 0.f, 0.f), &vInvLook, &m_vUp);
	D3DXMatrixInverse(&matInv, NULL, &matInv);
	
	return matInv;
}
