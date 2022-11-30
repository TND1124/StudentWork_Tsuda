#pragma once

#pragma warning(push)
#pragma warning(disable : 4005)
#include <d3dx10.h>
#include <fstream>
#include <wchar.h>
#pragma warning(pop)

#include "Collision.h"

// キャラクターが持てる最大のアニメーション数
#define ANIMATION_DATA_MAX (10) 

// 頂点データ
struct VertexType
{
	VertexType()
	{
		// ウェイト値初期化
		memset(m_weight, 0x00, sizeof(m_weight));

		// ボーン行列id値初期化
		memset(m_matoffsetid, 0x00, sizeof(m_matoffsetid));
	}

	D3DXVECTOR3 m_vPos;				// オブジェクト座標
	D3DXVECTOR2 m_vTex;				// テクスチャ座標
	D3DXVECTOR3 m_vNor;				// 法線情報
	float		m_weight[4];		// ウェイト値
	int		    m_matoffsetid[4];	// ボーン行列id
};

// アニメーションキー
struct AnimationKey
{
	AnimationKey()
	{
		m_pmatRotation = NULL;
		m_pvScale = NULL;
		m_pvTranslation = NULL;
	}

	~AnimationKey()
	{
		if (m_pmatRotation != NULL)
		{
			delete[] m_pmatRotation;
		}
		if (m_pvScale != NULL)
		{
			delete[] m_pvScale;
		}
		if (m_pvTranslation != NULL)
		{
			delete[] m_pvTranslation;
		}
	}

	D3DXMATRIX*	    m_pmatRotation;	 // 回転行列
	D3DXVECTOR3*	m_pvScale;		 // 拡大縮小
	D3DXVECTOR3*	m_pvTranslation; // 平行移動
};

// マテリアルデータ
struct MaterialType
{
public:
	MaterialType()
	{
		m_pTexture		 = NULL;
		m_vAmbientColor  = D3DXVECTOR4(0.1f, 0.1f, 0.1f, 0.f);
		m_vDiffuseColor  = D3DXVECTOR4(0.5f, 0.5f, 0.5f, 0.f);
		m_vSpecularColor = D3DXVECTOR4(0.2f, 0.2f, 0.2f, 1.f);
		m_fSpecularPower = 0.f;
		m_vEmission		 = D3DXVECTOR4(0.f, 0.f, 0.f, 0.f);
	}

public:
	ID3D10ShaderResourceView* m_pTexture;		 // テクスチャ情報
	D3DXVECTOR4				  m_vAmbientColor;	 // 環境光
	D3DXVECTOR4				  m_vDiffuseColor;	 // 拡散光
	D3DXVECTOR4				  m_vSpecularColor;	 // 鏡面反射
	float					  m_fSpecularPower;	 // 鏡面反射光の強さ
	D3DXVECTOR4				  m_vEmission;		 // 発光
};

// ボーンデータ
class Bone
{
public:
	Bone()
	{
		D3DXMatrixIdentity(&m_matOffset);
		m_ppBoneChild	  = NULL;
		m_nBoneChildCount = 0;
		m_nBoneDepthCount = 0;
		D3DXMatrixIdentity(&m_matAnimation);
		m_pmatOyaAnimation = NULL;
		m_pmatOyaInvOffset = NULL;
	}

	~Bone()
	{
		if (m_ppBoneChild != NULL)
		{
			delete[] m_ppBoneChild;
			m_ppBoneChild = NULL;
		}

		if (m_pmatOyaAnimation != NULL)
		{
			delete[] m_pmatOyaAnimation;
			m_pmatOyaAnimation = NULL;
		}

		if (m_pmatOyaInvOffset != NULL)
		{
			delete[] m_pmatOyaInvOffset;
			m_pmatOyaInvOffset = NULL;
		}
	}

	D3DXMATRIX   m_matOffset;		  // ボーンオフセット行列
	D3DXMATRIX   m_matInvOffset;	  // ボーン逆オフセット行列
	D3DXMATRIX   m_matAnimation;	  // アニメーション行列
	Bone**	     m_ppBoneChild;		  // 子ボーン情報
	int		     m_nBoneChildCount;	  // 子ボーンの数
	int		     m_nBoneDepthCount;	  // 何世代目のボーンか
	D3DXMATRIX*  m_pmatOyaAnimation;  // 親ボーンのアニメション行列
	D3DXMATRIX*  m_pmatOyaInvOffset;  // 親ボーンの逆行列
	AnimationKey m_pAnimationKey[10]; // アニメーション情報(10種類の動きしか入れられない)
};

// メッシュデータ
struct MeshType
{
public:
	MeshType();
	~MeshType();

public:
	ID3D10Buffer*  m_pVertexBuffer;		  // 頂点バッファ
	ID3D10Buffer** m_ppIndexBuffer;		  // インデックスバッファ
	int*		   m_pMaterialIndexCount; // マテリアル別のインデックスの数
	MaterialType*  m_pMaterialType;		  // マテリアル情報
	int			   m_nMaterialCount;	  // マテリアルの数
};

// モデルクラス
class CModel
{
public:
	CModel();
	~CModel();
	// 独自フォーマットのnxファイルを読み込む
	bool LoadNXModel(const wchar_t* pFileName);
	// 独自フォーマットのnxアニメーションファイルを読み込む
	bool LoadNXAnimation(const wchar_t* pFileName, int id);

	// 点情報からモデルを作成する
	void PointToCreateModel(const D3DXVECTOR3* pPoints, int PointNum);
	// 四角形を作成する(3D描画用)
	void CreateQuadrangle3D();
	// 四角形を作成する(2D描画用)
	void CreateQuadrangle2D();

	MeshType* GetMesh() const { return m_pMeshType; }
	int GetMeshCount() const { return m_nMeshCount; }
	Bone* GetBone(int nBone) const { return &m_pBone[nBone]; }
	int GetBoneCount() const { return m_nBoneCount; }
	int GetAnimationFrame(int id) { return m_nEndFrame[id]; }
	Collision::OBB3D* GetObb() { return m_pObb; }
	// スキンアニメーション
	// 引数1 fFrameSecond : アニメーションフレーム
	// 引数2 id			  : アニメーション番号
	void SkinAnimation(float fFrameSecond, int id);

private:
	// ボーンアニメーションを作成する(再帰関数)
	void RecursionAnimation(Bone* pBone, const D3DXMATRIX* pmatOyaAni, const 
		D3DXMATRIX* pmatOyaInv, int nAnimationNumber, float fFrameSecond);

	// nx頂点情報を取得する関数
	// 引数2 pVertexData : 頂点情報を入れる入れ物
	// 引数3 pVertexNum  : 頂点数
	void NXLoadVertex(FILE** pFile, VertexType** pVertexType, int* pVertexCount);
	// nx法線情報を取得する関数
	void NXLoadNormal(FILE** pFile, VertexType** pVertexType, int pVertexCount);
	// nxUV情報を取得する関数
	void NXLoadUv(FILE** pFile, VertexType** pVertexType, int pVertexCount);
	// nxマテリアル情報を取得する関数
	void NXLoadMaterial(FILE** pFile, VertexType** pVertexType, int pVertexCount, int*** pppMaterialIndex);

private:
	MeshType* m_pMeshType;  // メッシュ情報
	int m_nMeshCount;		// メッシュの数

	Bone* m_pBone;			// ボーン情報
	int m_nBoneCount;		// ボーンの数

	Collision::OBB3D* m_pObb; // Obb情報

	int	m_nEndFrame[ANIMATION_DATA_MAX]; // アニメーションが終了するフレーム数
};
