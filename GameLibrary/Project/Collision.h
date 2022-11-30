#pragma once

#pragma warning(push)
#pragma warning(disable : 4005)
#include <d3dx10.h>
#pragma warning(pop)

// モデルクラスとの相互インクルード回避
class CModel;

namespace Collision
{
	// AABB3D
	struct AABB3D
	{
		D3DXVECTOR3 m_vPosition;	// 中心座標
		D3DXVECTOR3 m_vScale;		// 大きさ
	};

	// AABB同士の衝突判定
	bool HitAABB3D(const AABB3D& Aabb_1, const AABB3D& Aabb_2);

	// 球体
	struct Sphere3D
	{
		D3DXVECTOR3 m_vPosition; // 中心座標
		float m_fRadius;		 // 半径
	};

	// Sphere同士の衝突判定
	bool HitSphere3D(const Sphere3D& Sphere_1, const Sphere3D& Sphere_2);

	// 線分
	struct Segment3D
	{
		D3DXVECTOR3 m_vStartPosition; // 始点座標
		D3DXVECTOR3 m_vEndPosition;   // 終点座標
	};

	// 線分と点の最短距離(2倍)
	float SegmentAndPointDistanceSq(const Segment3D& Segment, const D3DXVECTOR3& Point);

	// 線分と三角形の当たり判定
	// 引数1 Segment    : 線分情報
	// 引数2 vPoint1    : 三角形の点の位置ベクトルその1
	// 引数3 vPoint2    : 三角形の点の位置ベクトルその2
	// 引数4 vPoint3    : 三角形の点の位置ベクトルその3
	// 引数5 voutInter* : 線分の始点から交点へのベクトル(out)
	// 戻り値		    : 当たったかどうか
	bool HitSegmentAndTriangle(const Segment3D& Segment, const D3DXVECTOR3& vPoint1,
		const D3DXVECTOR3& vPoint2, const D3DXVECTOR3& vPoint3, D3DXVECTOR3* ovutInter);

	// 線分とモデルの当たり判定
	// 引数1 Segment    : 線分情報
	// 引数2 CModel     : モデルデータ
	// 引数3 D3DXMATRIX : モデルのワールド座標
	// 引数4 voutInter* : 線分の始点から交点へのベクトル(out)
	// 戻り値		    : 当たったかどうか
	// 線分とモデルの当たり判定
	bool HitSegmentAndModel(const Segment3D& Segment, const CModel& Model, const D3DXMATRIX& matWorld, D3DXVECTOR3* voutInter);

	// カプセル
	struct Capsule3D
	{
		Segment3D m_Segment; // 線分
		float m_fRadius;	 // 半径
	};

	// カプセルと球体の衝突判定
	bool HitSphereAndCapsule(const Sphere3D& Sphere, const Capsule3D& Capusule);

	// カプセルとカプセルの衝突判定(テスト未)
	bool HitCapsule3D(const Capsule3D& Capusule1, const Capsule3D& Capusule2);

	// OBB
	struct OBB3D
	{
		OBB3D()
		{
			m_vInitLength = D3DXVECTOR3(1.f, 1.f, 1.f);
			m_vInitPosition = D3DXVECTOR3(0.f, 0.f, 0.f);
		}

		// 現在のOBBの値
		D3DXVECTOR3 m_vPosition; // 中心座標
		D3DXVECTOR3 m_vSlope[3]; // XYZの各座標軸の傾きを表す方向ベクトル
		D3DXVECTOR3 m_vLength;   // OBBの各座標軸に沿った長さの半分

	private:
		// OBBの初期状態保存用
		D3DXVECTOR3 m_vInitPosition; // 中心座標
		D3DXVECTOR3 m_vInitLength;   // OBBの各座標軸に沿った長さの半分

	public:
		
		void Init(const D3DXVECTOR3& vLength, const D3DXVECTOR3& vPosition); // OBBを初期化

		void Update(const D3DXVECTOR3& vScale, const D3DXQUATERNION& qRotate, const D3DXVECTOR3& vPosition); // OBBの情報更新
	};
											  	  
	// OBBと点の最短距離
	float ObbAndPointDistance(const OBB3D& Obb, const D3DXVECTOR3& Point);

	// OBBと球体の衝突判定
	bool HitObbAndSphere3D(const OBB3D& Obb, const Sphere3D& Sphere);

	// OBBとOBBの衝突判定
	bool HitObb3D(const OBB3D& Obb1, const OBB3D& Obb2);

	// デバッグ用当たり判定---------------------------------------------
	// OBB当たり判定表示
	void DrawDebugOBB(const OBB3D& Obb);

	// Sphere当たり判定表示
	void DrawDebugSphere(const Sphere3D& Obb);
	// -----------------------------------------------------------------
}
