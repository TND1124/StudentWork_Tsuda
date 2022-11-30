#include "CCamera.h"
#include "Window.h"

#include "CInput.h"

// 画角
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
		// TPS用のカメラに設定
		// カメラを回転させる
		if (pInput->GetKey('X') == true)
		{
			m_fRadian -= 0.01f;
		}
		else if (pInput->GetKey('C') == true)
		{
			m_fRadian += 0.01f;
		}

		// 主人公を中心に回る回転
		D3DXVECTOR3 vAxis(0.f, 1.f, 0.f);
		D3DXMATRIX matRotation;
		D3DXMatrixRotationAxis(&matRotation, &vAxis, m_fRadian);

		// 主人公から少し離す
		D3DXVECTOR3 vTrans(0.f, 0.f, -2.f);
		D3DXVec3TransformCoord(&vTrans, &vTrans, &matRotation);
		m_vEye += vTrans;
	}

	// ビュー行列作成
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vLook, &m_vUp);

	// 画面比率
	float AspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
	
	// プロジェクショントランスフォーム（射影変換）
	D3DXMatrixPerspectiveFovLH(&m_matProj, AngleConvertRadian(VIEW_SCOPE), AspectRatio, 0.1f, 10000.0f);
}

// カメラを回転させる
// 引数1 const D3DXVECTOR3& : 回転してほしいXYZそれぞれの角度(radian)
void CCamera::Rotation(const D3DXVECTOR3& vRotation)
{
	D3DXQUATERNION Quaternion;
	D3DXMATRIX matRotation;

	// XYZの回転してほしい角度(ラジアン)をクォータニオン変換
	D3DXQuaternionRotationYawPitchRoll(&Quaternion, vRotation.x, vRotation.y, vRotation.z);
	D3DXMatrixRotationQuaternion(&matRotation, &Quaternion);
	
	D3DXVec3TransformCoord(&m_vLook, &m_vLook, &matRotation);
	
	m_vLook += m_vEye;
}

// ビルボード用のカメラのほうを向く行列
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
