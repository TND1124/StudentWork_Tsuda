#pragma once

#pragma warning(push)
#pragma warning(disable : 4005)
#include <d3dx10.h>
#pragma warning(pop)

// �J�����N���X
class CCamera
{
public:
	CCamera() {}
	~CCamera() {}
	void Init(const D3DXVECTOR3& vEye,
		const D3DXVECTOR3& vLook, const D3DXVECTOR3& vUp);
	void SetEye(const D3DXVECTOR3& vEye) { m_vEye = vEye; }
	void SetLookAt(const D3DXVECTOR3& vLook) { m_vLook = vLook; }
	void SetIsTps(const bool isFps) { m_isTps = isFps; }
	void Update();
	void Rotation(const D3DXVECTOR3& vRotation);
	const D3DXMATRIX* GetMatView()	 const { return &m_matView; }
	const D3DXMATRIX GetMatInvView() const;
	const D3DXMATRIX* GetMatProj()   const { return &m_matProj; }
	const D3DXVECTOR3* GetEye()		 const { return &m_vEye; }
	const D3DXVECTOR3* GetLookAt()	 const { return &m_vLook; }
	const D3DXVECTOR3* GetRadian()	 const { return &m_vRadian; }

private:
	D3DXVECTOR3 m_vEye;	   // �J�����i���_�j�ʒu
	D3DXVECTOR3 m_vLook;   // �����ʒu
	D3DXVECTOR3 m_vUp;	   // ����ʒu

	D3DXMATRIX m_matView;  // �r���[�}�g���b�N�X�s��
	D3DXMATRIX m_matProj;  // �v���W�F�N�V�����s��

	D3DXVECTOR3 m_vRadian; // �J��������]���������p�x
	int m_MouseX;		   // �}�E�X�̈ʒu
	int m_MouseY;		   
	bool m_isTps;		   // FPS�d�l�̃J�����ɕς��邩�ǂ���
	float m_fRadian;
};

// �x���@���烉�W�A���p
inline float AngleConvertRadian(float angle)
{
	return (angle / 180.f) * 3.14f;
}
