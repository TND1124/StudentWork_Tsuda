#include "Collision.h"

#include "CShader.h"
#include "CDevice3D.h"

extern CModel* g_pDebugBoxModel;
extern CModel* g_pDebugSphereModel;

using namespace Collision;

bool Collision::HitAABB3D(const AABB3D& Aabb1, const AABB3D& Aabb2)
{
	// x軸があたっているかどうか
	if (Aabb1.m_vPosition.x - Aabb1.m_vScale.x <=
		Aabb2.m_vPosition.x + Aabb2.m_vScale.x &&
		Aabb1.m_vPosition.x + Aabb1.m_vScale.x >=
		Aabb2.m_vPosition.x - Aabb2.m_vScale.x)
	{
		// y軸があたっているかどうか
		if (Aabb1.m_vPosition.y - Aabb1.m_vScale.y <=
			Aabb2.m_vPosition.y + Aabb2.m_vScale.y &&
			Aabb1.m_vPosition.y + Aabb1.m_vScale.y >=
			Aabb2.m_vPosition.y - Aabb2.m_vScale.y)
		{
			// z軸があたっているかどうか
			if (Aabb1.m_vPosition.z - Aabb1.m_vScale.z <=
				Aabb2.m_vPosition.z + Aabb2.m_vScale.z &&
				Aabb1.m_vPosition.z + Aabb1.m_vScale.z >=
				Aabb2.m_vPosition.z - Aabb2.m_vScale.z)
			{
				return true;
			}
		}
	}

	return false;
}

bool Collision::HitSphere3D(const Sphere3D & Sphere1, const Sphere3D & Sphere2)
{
	D3DXVECTOR3 vDifference; // 球体2つの差ベクトル
	float fDistance2;		 // 球体2点の距離(2乗されている)
	float fAddRadian;		 // 球体2つの半径の合計

	vDifference = Sphere1.m_vPosition - Sphere2.m_vPosition;
	fDistance2 = D3DXVec3LengthSq(&vDifference);
	fAddRadian = Sphere1.m_fRadius + Sphere2.m_fRadius;

	// 球体2つの半径の合計よりも球体2点の距離が短ければ、衝突している
	return (fDistance2 < (fAddRadian * fAddRadian));
}

bool Collision::HitSegmentAndTriangle(const Segment3D& Segment, const D3DXVECTOR3& vPoint1,
	const D3DXVECTOR3& vPoint2, const D3DXVECTOR3& vPoint3, D3DXVECTOR3* voutInter)
{
	D3DXVECTOR3 vNor;						 // 引数の三角形の法線
	D3DXVECTOR3 vTriangle1, vTriangle2;		 // 三角形の点を結ぶベクトル
	D3DXVECTOR3 vSegment, vInvSegment;		 // 線分の点を結ぶベクトル
	D3DXVECTOR3 vDifference, vInvDifference; // 線分の点と三角形の一点と結ぶベクトル
	vTriangle1 = vPoint2 - vPoint1;
	vTriangle2 = vPoint3 - vPoint2;
	vSegment = Segment.m_vEndPosition - Segment.m_vStartPosition;
	vInvSegment = Segment.m_vStartPosition - Segment.m_vEndPosition;
	vDifference = Segment.m_vStartPosition - vPoint1;
	vInvDifference = Segment.m_vEndPosition - vPoint1;

	// 外積で法線を求める
	D3DXVec3Cross(&vNor, &vTriangle1, &vTriangle2);
	D3DXVec3Normalize(&vNor, &vNor);

	// 内積で線分が三角形のどちらの向きからあたっているかを調べる
	float InterLen = D3DXVec3Dot(&vDifference, &vNor);
	// 裏側なら
	if (InterLen < 0.f)
	{
		// 当たっていない
		return false;
	}

	// 内積で線分が三角形のどちらの向きからあたっているかを調べる
	float InvInterLen = D3DXVec3Dot(&vInvDifference, &vNor);
	// 表側なら
	if (InvInterLen > 0.f)
	{
		// 当たっていない
		return false;
	}

	// 視点から交点までのベクトルを求める
	D3DXVECTOR3 vInter;
	vInter = vSegment * ( InterLen / ( InterLen - InvInterLen) );

	// 交点を求める
	D3DXVECTOR3 vInterPoint;
	vInterPoint = Segment.m_vStartPosition + vInter;
	
	// 交点が三角形の内側かどうかを調べる--------------------------------------------------
	D3DXVECTOR3 vCross1, vCross2;
	vCross1 = vPoint1 - vInterPoint;
	vCross2 = vPoint2 - vPoint1;

	D3DXVECTOR3 vCross;
	D3DXVec3Cross(&vCross, &vCross1, &vCross2);

	// 向きをチェック
	float direction;
	direction = D3DXVec3Dot(&vCross, &vNor);
	if (direction < 0.f)
	{
		// 当たっていない
		return false;
	}

	vCross1 = vPoint2 - vInterPoint;
	vCross2 = vPoint3 - vPoint2;

	D3DXVec3Cross(&vCross, &vCross1, &vCross2);

	direction = D3DXVec3Dot(&vCross, &vNor);
	if (direction < 0.f)
	{
		// 当たっていない
		return false;
	}

	vCross1 = vPoint3 - vInterPoint;
	vCross2 = vPoint1 - vPoint3;
	
	D3DXVec3Cross(&vCross, &vCross1, &vCross2);

	direction = D3DXVec3Dot(&vCross, &vNor);
	if (direction < 0.f)
	{
		// 当たっていない
		return false;
	}
	// -------------------------------------------------------------------------------------

	*voutInter = vInter;

	// 当たっている
	return true;
}

// 線分とモデルの衝突判定
bool Collision::HitSegmentAndModel(const Segment3D& Segment, const CModel& Model, const D3DXMATRIX& matWorld, D3DXVECTOR3* voutInter)
{
	VertexType* pVertex; // モデルの頂点データを入れるポインタ
	void* pVertexPtr;	 // モデルの頂点データを取得するポインタ
	int* pIndex;		 // モデルのインデックスデータを入れるポインタ
	void* pIndexPtr;	 // モデルのインデックスデータを取得するポインタ
	HRESULT hr;			 // Map関数が成功したかどうかを調べる変数

	float LenShort2 = 9999999.f; // 最短距離(2倍) 
	bool outHit = false;		 // ヒットフラグ(戻り値)

	// 線分をモデルのローカル座標に変換-----------------------------------------
	D3DXMATRIX matInvWorld;
	D3DXMatrixInverse(&matInvWorld, NULL, &matWorld);

	D3DXVECTOR4 vLocalStartPosition, vLocalEndPosition;
	D3DXVec3Transform(&vLocalStartPosition, &Segment.m_vStartPosition, &matInvWorld);
	D3DXVec3Transform(&vLocalEndPosition, &Segment.m_vEndPosition, &matInvWorld);

	Segment3D LocalSegment;
	LocalSegment.m_vStartPosition = D3DXVECTOR3(vLocalStartPosition);
	LocalSegment.m_vEndPosition = D3DXVECTOR3(vLocalEndPosition);
	// -------------------------------------------------------------------------

	for (int i = 0; i < Model.GetMeshCount(); i++)
	{
		// モデルの頂点データを取得
		hr = Model.GetMesh()[i].m_pVertexBuffer->Map(D3D10_MAP_WRITE_NO_OVERWRITE, 0, (void**)&pVertexPtr);
		if (FAILED(hr))
		{
			MessageBox(NULL, L"バーテックスバッファのマップ関数失敗", L"エラー", MB_OK);
			return false;
		}
		
		pVertex = (VertexType*)pVertexPtr;

		for (int j = 0; j < Model.GetMesh()[i].m_nMaterialCount; j++)
		{
			// モデルのインデックスデータを取得
			Model.GetMesh()[i].m_ppIndexBuffer[j]->Map(D3D10_MAP_WRITE_NO_OVERWRITE, 0, (void**)&pIndexPtr);
			if (FAILED(hr))
			{
				MessageBox(NULL, L"インデックスバッファのマップ関数失敗", L"エラー", MB_OK);
				return false;
			}

			pIndex = (int*)pIndexPtr;

			int nIndexCount = Model.GetMesh()[i].m_pMaterialIndexCount[j];

			for (int k = 0; k < nIndexCount; k += 3)
			{
				D3DXVECTOR3 v1, v2, v3;
				D3DXVECTOR3 vInter;

				v1 = pVertex[pIndex[k]].m_vPos;
				v2 = pVertex[pIndex[k + 1]].m_vPos;
				v3 = pVertex[pIndex[k + 2]].m_vPos;

				// 線分と三角形の当たり判定
				if (HitSegmentAndTriangle(LocalSegment, v1, v2, v3, &vInter) == true)
				{
					float Len2 = D3DXVec3LengthSq(&vInter);

					// 最短の交点を調べる
					if (Len2 < LenShort2)
					{
						// 情報を入れる
						LenShort2 = Len2;
						outHit = true;
						*voutInter = vInter;
					}
				}
			}
		}
	}

	// 視点から交点へのベクトルをワールド座標に戻す
	D3DXVECTOR4 vtemp;
	D3DXVec3Transform(&vtemp, voutInter, &matWorld);
	voutInter->x = vtemp.x;
	voutInter->y = vtemp.y;
	voutInter->z = vtemp.z;

	return outHit;
}

// 線分と点の最短距離
float Collision::SegmentAndPointDistanceSq(const Segment3D& Segment, const D3DXVECTOR3& Point)
{
	D3DXVECTOR3 vp = Point - Segment.m_vStartPosition;
	D3DXVECTOR3 v = Segment.m_vEndPosition - Segment.m_vStartPosition;
	D3DXVECTOR3 v_nor;
	D3DXVec3Normalize(&v_nor, &v);
	float t = D3DXVec3Dot(&v_nor, &vp) / D3DXVec3Length(&v);
	D3DXVECTOR3 h = v * t - vp; // 最短ベクトル

	return D3DXVec3LengthSq(&h);
}

// カプセルと球体の衝突判定
bool Collision::HitSphereAndCapsule(const Sphere3D& Sphere, const Capsule3D& Capusule)
{
	float fDistance2;		 // 球体2点の距離(2乗されている)
	float fAddRadian;		 // 球体とカプセルの半径の合計

	fAddRadian = Sphere.m_fRadius + Capusule.m_fRadius;

	fDistance2 = SegmentAndPointDistanceSq( Capusule.m_Segment, Sphere.m_vPosition);

	// オブジェクト2つの半径の合計よりもオブジェクト2点の距離が短ければ、衝突している
	return fDistance2 <= (fAddRadian * fAddRadian);
}

// カプセル同士の衝突判定
bool Collision::HitCapsule3D(const Capsule3D& Capsule1, const Capsule3D& Capsule2)
{
	D3DXVECTOR3 vDifference; // カプセル2つの差ベクトル
	float fDistance2;		 // 球体2点の距離(2乗されている)
	float fAddRadian;		 // 球体とカプセルの半径の合計

	fAddRadian = Capsule1.m_fRadius + Capsule2.m_fRadius;

	// カプセルの線分同士で最も近い点を求める------------------
	D3DXVECTOR3 n; // カプセル同士の法線
	D3DXVECTOR3 CapsuleDirection1, CapsuleDirection2; // カプセル別の向きベクトル
	CapsuleDirection1 = Capsule1.m_Segment.m_vEndPosition - Capsule1.m_Segment.m_vStartPosition;
	CapsuleDirection2 = Capsule2.m_Segment.m_vEndPosition - Capsule2.m_Segment.m_vStartPosition;
	D3DXVec3Cross(&n, &CapsuleDirection1, &CapsuleDirection2);
	// カプセル別の向きベクトル同士が平行だった場合
	if (D3DXVec3LengthSq(&n) == 0.f)
	{
		// ある軸が平行だと2Dで判定することができる
		// カプセルの始点と終点で線と点の判定を使って、一番短い情報を調べる
		fDistance2 = SegmentAndPointDistanceSq(Capsule1.m_Segment, Capsule2.m_Segment.m_vStartPosition);
		
		float ftempDistance2 = SegmentAndPointDistanceSq(Capsule1.m_Segment, Capsule2.m_Segment.m_vEndPosition);

		if (fDistance2 > ftempDistance2)
		{
			fDistance2 = ftempDistance2;
		}

		ftempDistance2 = SegmentAndPointDistanceSq(Capsule2.m_Segment, Capsule1.m_Segment.m_vStartPosition);

		if (fDistance2 > ftempDistance2)
		{
			fDistance2 = ftempDistance2;
		}

		ftempDistance2 = SegmentAndPointDistanceSq(Capsule2.m_Segment, Capsule1.m_Segment.m_vEndPosition);

		if (fDistance2 > ftempDistance2)
		{
			fDistance2 = ftempDistance2;
		}
	}
	else
	{
		D3DXVec3Normalize(&n, &n);
		D3DXVECTOR3 vPointDifference; // カプセル別のある点の差ベクトル
		vPointDifference = Capsule1.m_Segment.m_vStartPosition - Capsule2.m_Segment.m_vStartPosition;
		float len = D3DXVec3Dot(&vPointDifference, &n);
		vDifference = n * len; // 差ベクトルを入れる
	}
	// --------------------------------------------------------

	fDistance2 = D3DXVec3LengthSq(&vDifference);

	// カプセル2つの半径の合計よりもカプセル2点の距離が短ければ、衝突している
	return fDistance2 <= (fAddRadian * fAddRadian);
}

// OBBと点の最短距離
float Collision::ObbAndPointDistance(const OBB3D& Obb, const D3DXVECTOR3& Point)
{
	float outDistance = 0.f;

	// 三方向
	for (int i = 0; i < 3; i++)
	{
		// OBBの中心と点の差ベクトル
		D3DXVECTOR3 vDifference; 

		vDifference = Point - Obb.m_vPosition;

		// はみ出し判定
		float s = D3DXVec3Dot( &vDifference, &Obb.m_vSlope[i] ) / Obb.m_vLength[i];

		s = fabsf(s);
		// はみ出していたら
		if (s > 1.f)
		{
			// はみ出している距離だけ離れている
			outDistance += ( s - 1.f ) * Obb.m_vLength[i];
		}
	}

	return outDistance;
}

// OBBと球体の衝突判定
bool Collision::HitObbAndSphere3D(const OBB3D& Obb, const Sphere3D& Sphere)
{
	float fDistance = ObbAndPointDistance(Obb, Sphere.m_vPosition);

	return (fDistance <= Sphere.m_fRadius);
}

// OBBとOBBの衝突判定
bool Collision::HitObb3D(const OBB3D& Obb1, const OBB3D& Obb2)
{
	D3DXVECTOR3 vDifference; // OBBとOBBの中心の差ベクトル
	vDifference = Obb1.m_vPosition - Obb2.m_vPosition;

	float Len = 0.f;
	float fDistance = 0.f;

	// OBB1の分離軸
	for (int i = 0; i < 3; i++)
	{
		Len =
			fabsf(D3DXVec3Dot(&Obb1.m_vSlope[i], &Obb2.m_vSlope[0])) * Obb2.m_vLength[0] + 
			fabsf(D3DXVec3Dot(&Obb1.m_vSlope[i], &Obb2.m_vSlope[1])) * Obb2.m_vLength[1] +
			fabsf(D3DXVec3Dot(&Obb1.m_vSlope[i], &Obb2.m_vSlope[2])) * Obb2.m_vLength[2] +
			Obb1.m_vLength[i];
		fDistance = fabsf(D3DXVec3Dot(&Obb1.m_vSlope[i], &vDifference));
		if (Len < fDistance)
		{
			return false;
		}
	}

	// OBB2の分離軸
	for (int i = 0; i < 3; i++)
	{
		Len =
			fabsf(D3DXVec3Dot(&Obb2.m_vSlope[i], &Obb1.m_vSlope[0])) * Obb1.m_vLength[0] +
			fabsf(D3DXVec3Dot(&Obb2.m_vSlope[i], &Obb1.m_vSlope[1])) * Obb1.m_vLength[1] +
			fabsf(D3DXVec3Dot(&Obb2.m_vSlope[i], &Obb1.m_vSlope[2])) * Obb1.m_vLength[2] +
			Obb2.m_vLength[i];
		fDistance = fabsf(D3DXVec3Dot(&Obb2.m_vSlope[i], &vDifference));
		if (Len < fDistance)
		{
			return false;
		}
	}

	D3DXVECTOR3 vCross;
	// OBB1とOBB2の外積分離軸
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			D3DXVec3Cross(&vCross, &Obb1.m_vSlope[i], &Obb2.m_vSlope[j]);
			Len =
				fabsf(D3DXVec3Dot(&vCross, &Obb1.m_vSlope[0])) * Obb1.m_vLength[0] +
				fabsf(D3DXVec3Dot(&vCross, &Obb1.m_vSlope[1])) * Obb1.m_vLength[1] +
				fabsf(D3DXVec3Dot(&vCross, &Obb1.m_vSlope[2])) * Obb1.m_vLength[2] +
				fabsf(D3DXVec3Dot(&vCross, &Obb2.m_vSlope[0])) * Obb2.m_vLength[0] +
				fabsf(D3DXVec3Dot(&vCross, &Obb2.m_vSlope[1])) * Obb2.m_vLength[1] +
				fabsf(D3DXVec3Dot(&vCross, &Obb2.m_vSlope[2])) * Obb2.m_vLength[2];
			fDistance = fabsf(D3DXVec3Dot(&vCross, &vDifference));
			if (Len < fDistance)
			{
				return false;
			}
		}
	}

	return true;
}

// OBB情報初期化
void Collision::OBB3D::Init(const D3DXVECTOR3& vLength, const D3DXVECTOR3& vPosition)
{
	// 初期値を入れる
	m_vInitLength = vLength;
	m_vInitPosition = vPosition;
}

// OBB情報更新
void Collision::OBB3D::Update(const D3DXVECTOR3& vScale, const D3DXQUATERNION& qRotate, const D3DXVECTOR3& vPosition)
{
	m_vLength.x = m_vInitLength.x * vScale.x;
	m_vLength.y = m_vInitLength.y * vScale.y;
	m_vLength.z = m_vInitLength.z * vScale.z;

	D3DXMATRIX matRotate;
	D3DXMatrixRotationQuaternion(&matRotate, &qRotate);
	m_vSlope[0].x = matRotate._11;
	m_vSlope[0].y = matRotate._12;
	m_vSlope[0].z = matRotate._13;
	m_vSlope[1].x = matRotate._21;
	m_vSlope[1].y = matRotate._22;
	m_vSlope[1].z = matRotate._23;
	m_vSlope[2].x = matRotate._31;
	m_vSlope[2].y = matRotate._32;
	m_vSlope[2].z = matRotate._33;

	// 最初に登録された初期位置にもスケールと回転をかける
	D3DXVECTOR3 InitPosition;
	InitPosition.x = m_vInitPosition.x * vScale.x;
	InitPosition.y = m_vInitPosition.y * vScale.y;
	InitPosition.z = m_vInitPosition.z * vScale.z;
	D3DXVec3TransformNormal(&InitPosition, &InitPosition, &matRotate);

	// 初期位置と現在の位置を合わせる
	m_vPosition.x = InitPosition.x + vPosition.x;
	m_vPosition.y = InitPosition.y + vPosition.y;
	m_vPosition.z = InitPosition.z + vPosition.z;
}

// OBBの当たり判定表示
void Collision::DrawDebugOBB(const OBB3D& Obb)
{
#ifdef _DEBUG
	D3DXMATRIX matWorld, matScale, matRota, matTrans;
	D3DXMatrixScaling(&matScale, Obb.m_vLength[0], Obb.m_vLength[1], Obb.m_vLength[2]);
	matRota._11 = Obb.m_vSlope[0].x;
	matRota._12 = Obb.m_vSlope[0].y;
	matRota._13 = Obb.m_vSlope[0].z;
	matRota._14 = 0.f;
	matRota._21 = Obb.m_vSlope[1].x;
	matRota._22 = Obb.m_vSlope[1].y;
	matRota._23 = Obb.m_vSlope[1].z;
	matRota._24 = 0.f;
	matRota._31 = Obb.m_vSlope[2].x;
	matRota._32 = Obb.m_vSlope[2].y;
	matRota._33 = Obb.m_vSlope[2].z;
	matRota._34 = 0.f;
	matRota._41 = 0.f;
	matRota._42 = 0.f;
	matRota._43 = 0.f;
	matRota._44 = 1.f;
	D3DXMatrixTranslation(&matTrans, Obb.m_vPosition.x, Obb.m_vPosition.y, Obb.m_vPosition.z);
	matWorld =  matScale * matRota * matTrans;

	CShader* pShader = CShader::GetInstance();
	pShader->ModelDraw(g_pDebugBoxModel, matWorld, true);
#endif 
}

// Sphereの当たり判定表示
void Collision::DrawDebugSphere(const Sphere3D& Sphere)
{
#ifdef _DEBUG
	D3DXMATRIX matWorld, matScale, matTrans;
	D3DXMatrixScaling(&matScale, Sphere.m_fRadius, Sphere.m_fRadius, Sphere.m_fRadius);
	D3DXMatrixTranslation(&matTrans, Sphere.m_vPosition.x, Sphere.m_vPosition.y, Sphere.m_vPosition.z);
	matWorld = matScale * matTrans;

	CShader* pShader = CShader::GetInstance();
	pShader->ModelDraw(g_pDebugSphereModel, matWorld, true);
#endif 
}
