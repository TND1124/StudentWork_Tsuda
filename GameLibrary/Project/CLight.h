#pragma once

#pragma warning(push)
#pragma warning(disable : 4005)
#include <d3dx10.h>
#pragma warning(pop)

#define RIGHT_NUM_MAX (4)

// ���C�g�̎��
enum LightKind
{
	Light_None,			// �Ȃ�
	Light_Point,		// �_����
	Light_Directional,  // ���s����
};

class CLight
{
public:
	CLight()
	{
		m_eLightKind = Light_None;
		m_vLightDirction = D3DXVECTOR3(0.f, 0.f, 0.f);
		m_vLightPoint = D3DXVECTOR4(0.f, 0.f, 0.f, 0.f);
		m_fLightIntensity = 0.f;
	}
	CLight(const CLight&) {}
	~CLight() {}

	//���C�g�̎�ސݒ�
	void SetLightKind(const LightKind eLightKind) { m_eLightKind = eLightKind; }
	// ���s���C�g�p
	void SetLightDirectional(const D3DXVECTOR3& vLightDirection) { m_vLightDirction = vLightDirection; }
	// �_���C�g�p
	void SetLightPoint(const D3DXVECTOR4& vLightPoint) { m_vLightPoint = vLightPoint; }
	// �_���C�g�p
	void SetLightIntensity(const float fLightIntensity) { m_fLightIntensity = fLightIntensity; }

	LightKind GetLightKind() { return m_eLightKind; }
	D3DXVECTOR3 GetLightDirection() { return m_vLightDirction; }
	D3DXVECTOR4 GetLightPoint() { return m_vLightPoint; }
	float GetLightIntensity() { return m_fLightIntensity; }

private:
	LightKind	m_eLightKind;		// ���C�g�̎��
	D3DXVECTOR3 m_vLightDirction;	// ���s���C�g�̕���
	D3DXVECTOR4 m_vLightPoint;		// �_���C�g�̈ʒu
	float		m_fLightIntensity;	// �_���C�g�̋���
};

// ���C�g���Ǘ��N���X(�V���O���g��)
class CLightManager
{
private:
	CLightManager() {}
	CLightManager(const CLightManager&) {}
	~CLightManager() {}

public:
	static void Create();
	static CLightManager* GetInstance();
	static void Destory();

	// ���C�g�I�u�W�F�N�g�擾
	CLight* GetLight(int nNumber) 
	{ 
		if (nNumber >= RIGHT_NUM_MAX) return NULL;
		if (nNumber < 0) return NULL;

		return &m_Light[nNumber]; 
	}

private:
	static CLightManager* m_pInstance;

	CLight m_Light[RIGHT_NUM_MAX];	   // �g�p�ł��郉�C�g�̐���4��
};
