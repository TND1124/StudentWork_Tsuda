#pragma once

#pragma warning(push)
#pragma warning(disable : 4005)
#include <d3dx10.h>
#pragma warning(pop)

// カメラクラス
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
	D3DXVECTOR3 m_vEye;	   // カメラ（視点）位置
	D3DXVECTOR3 m_vLook;   // 注視位置
	D3DXVECTOR3 m_vUp;	   // 上方位置

	D3DXMATRIX m_matView;  // ビューマトリックス行列
	D3DXMATRIX m_matProj;  // プロジェクション行列

	D3DXVECTOR3 m_vRadian; // カメラを回転させたい角度
	int m_MouseX;		   // マウスの位置
	int m_MouseY;		   
	bool m_isTps;		   // FPS仕様のカメラに変えるかどうか
	float m_fRadian;
};

// 度数法からラジアン角
inline float AngleConvertRadian(float angle)
{
	return (angle / 180.f) * 3.14f;
}
