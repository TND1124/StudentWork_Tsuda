#pragma once

#pragma warning(push)
#pragma warning(disable : 4005)
#include <d3dx10.h>
#pragma warning(pop)

#define RIGHT_NUM_MAX (4)

// ライトの種類
enum LightKind
{
	Light_None,			// なし
	Light_Point,		// 点光源
	Light_Directional,  // 平行光源
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

	//ライトの種類設定
	void SetLightKind(const LightKind eLightKind) { m_eLightKind = eLightKind; }
	// 平行ライト用
	void SetLightDirectional(const D3DXVECTOR3& vLightDirection) { m_vLightDirction = vLightDirection; }
	// 点ライト用
	void SetLightPoint(const D3DXVECTOR4& vLightPoint) { m_vLightPoint = vLightPoint; }
	// 点ライト用
	void SetLightIntensity(const float fLightIntensity) { m_fLightIntensity = fLightIntensity; }

	LightKind GetLightKind() { return m_eLightKind; }
	D3DXVECTOR3 GetLightDirection() { return m_vLightDirction; }
	D3DXVECTOR4 GetLightPoint() { return m_vLightPoint; }
	float GetLightIntensity() { return m_fLightIntensity; }

private:
	LightKind	m_eLightKind;		// ライトの種類
	D3DXVECTOR3 m_vLightDirction;	// 平行ライトの方向
	D3DXVECTOR4 m_vLightPoint;		// 点ライトの位置
	float		m_fLightIntensity;	// 点ライトの強さ
};

// ライト情報管理クラス(シングルトン)
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

	// ライトオブジェクト取得
	CLight* GetLight(int nNumber) 
	{ 
		if (nNumber >= RIGHT_NUM_MAX) return NULL;
		if (nNumber < 0) return NULL;

		return &m_Light[nNumber]; 
	}

private:
	static CLightManager* m_pInstance;

	CLight m_Light[RIGHT_NUM_MAX];	   // 使用できるライトの数は4つ
};
