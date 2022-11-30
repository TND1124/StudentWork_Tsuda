#include "CModel.h"

#include "CDevice3D.h"
#include "CTextureManager.h"

#include <wchar.h>
#include <vector>

void WeightInsertionSort(float* p_fSortWeight, int* p_nSortOffsetId, int nElementCount);

MeshType::MeshType()
{
	m_pVertexBuffer = NULL;
	m_ppIndexBuffer = NULL;
	m_nMaterialCount = 0;
}

MeshType::~MeshType()
{
	// バーテックスバッファ解放
	if (m_pVertexBuffer != NULL)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
	}

	// インデックスバッファを解放
	if (m_ppIndexBuffer != NULL)
	{
		for (int i = 0; i < m_nMaterialCount; i++)
		{
			if (m_pMaterialIndexCount[i] != 0)
			{
				m_ppIndexBuffer[i]->Release();
				m_ppIndexBuffer[i] = NULL;
			}
		}
		delete[] m_ppIndexBuffer;
	}

	// マテリアル情報解放
	if (m_pMaterialType != NULL)
	{
		delete[] m_pMaterialType;
		m_pMaterialType = NULL;
	}

	// マテリアルインデックス情報解放
	if (m_pMaterialIndexCount != NULL)
	{
		delete[] m_pMaterialIndexCount;
		m_pMaterialIndexCount = NULL;
	}
}

CModel::CModel()
{
	m_nMeshCount = 0;

	m_pMeshType = NULL;
	m_pBone = NULL;
	m_pObb = NULL;
}

CModel::~CModel()
{
	// メッシュデータ解放
	if (m_pMeshType != NULL)
	{
		delete[] m_pMeshType;
		m_pMeshType = NULL;
	}

	// ボーンデータ解放
	if (m_pBone != NULL)
	{
		delete[] m_pBone;
		m_pBone = NULL;
	}

	// OBBデータ解放
	if (m_pObb != NULL)
	{
		delete[] m_pObb;
		m_pObb = NULL;
	}
}

// 独自フォーマットのnxファイルを読み込む
bool CModel::LoadNXModel(const wchar_t* pFileName)
{
	FILE* pFile;
	wchar_t FileData[256];
	
	VertexType* pVertexType = NULL;
	int nVertexCount = 0;

	// マテリアル別にインデックス情報を入れる入れ物
	int** ppMaterialIndex = NULL;
	
	// メッシュの数はnxファイルでは一つしか対応していません
	m_pMeshType = new MeshType[1];
	m_nMeshCount = 1;

	// ファイルオープン
	if (_wfopen_s(&pFile, pFileName, L"r") != 0)
	{
		wchar_t ErrorString[256];
		swprintf_s(ErrorString, 256, L"%sが見つかりませんでした。",
			pFileName);
		MessageBox(NULL, ErrorString, L"エラー", MB_OK);
		return false;
	}
	
	while (fwscanf_s(pFile, L"%s", FileData, 256) != EOF)
	{
		// 頂点情報読み込み箇所
		if (wcscmp(FileData, L"vnum") == 0)
		{
			NXLoadVertex(&pFile, &pVertexType, &nVertexCount);
		}
		// 法線情報読み込み
		else if (wcscmp(FileData, L"nnum") == 0)
		{
			NXLoadNormal(&pFile, &pVertexType, nVertexCount);
		}
		// uv情報読み込み
		else if (wcscmp(FileData, L"uvnum") == 0)
		{
			NXLoadUv(&pFile, &pVertexType, nVertexCount);
		}
		// マテリアル情報読み込み
		else if (wcscmp(FileData, L"matenum") == 0)
		{
			NXLoadMaterial(&pFile, &pVertexType, nVertexCount, &ppMaterialIndex);
		}
		// ボーン情報取得
		else if (wcscmp(FileData, L"bonenum") == 0)
		{
			// ボーン数取得
			fwscanf_s(pFile, L"%s", FileData, 256);
			swscanf_s(FileData, L"%d", &m_nBoneCount);

			// ボーンを情報を入れる入れ物を作成
			m_pBone = new Bone[m_nBoneCount];

			// ボーンの代目数を取得(1代目なら0が入る)
			for (int i = 0; i < m_nBoneCount; i++)
			{
				fwscanf_s(pFile, L"%s", FileData, 256);
				fwscanf_s(pFile, L"%s", FileData, 256);
				swscanf_s(FileData, L"%d", &m_pBone[i].m_nBoneDepthCount);
			}

			// 子ボーン情報を入れる
			for (int i = 0; i < m_nBoneCount; i++)
			{
				fwscanf_s(pFile, L"%s", FileData, 256);
				fwscanf_s(pFile, L"%s", FileData, 256);
				swscanf_s(FileData, L"%d", &m_pBone[i].m_nBoneChildCount);

				// 子のボーンの数だけ、子のボーンを入れる入れ物を作る
				m_pBone[i].m_ppBoneChild = new Bone*[m_pBone[i].m_nBoneChildCount];

				int count = 0;
				// 子のボーンを調べる
				for (int j = i + 1; count != m_pBone[i].m_nBoneChildCount; j++)
				{
					if (m_pBone[i].m_nBoneDepthCount + 1 == m_pBone[j].m_nBoneDepthCount)
					{
						m_pBone[i].m_ppBoneChild[count] = &m_pBone[j];

						count++;
					}
				}
			}

			for (int i = 0; i < m_nBoneCount; i++)
			{
				fwscanf_s(pFile, L"%s", FileData, 256);
				fwscanf_s(pFile, L"%s", FileData, 256);

				fgetws(FileData, 256, pFile);

				// ウェイト値取得
				// ボーンオフセット行列取得
				for (int j = 0; j < nVertexCount; j++)
				{
					int nWeightIndex = 0;
					float nWeight = 0;
					fgetws(FileData, 256, pFile);
					// 取得したウェイト値を保存
					swscanf_s(FileData, L"weight %d : %f",
						&nWeightIndex, &nWeight);

					if (nWeight == 0.f) continue;

					for (int wi = 0; wi < 4; wi++)
					{
						// 空いている場所があったら
						if (pVertexType[nWeightIndex].m_weight[wi] == 0.f)
						{
							// ウェイト値を入れる
							pVertexType[nWeightIndex].m_weight[wi] = nWeight;

							// ボーンオフセットをウェイト値と同じ要素数で入れる
							// ボーンオフセット値とウェイト値を関連付ける
							pVertexType[nWeightIndex].m_matoffsetid[wi] = i;
							break;
						}

						// モデルの頂点に5つ以上のボーンの影響を受けているものが
						if (wi == 3)
						{
							float SortWeight[5];
							int SortOffsetId[5];

							// ソート用の配列に入れなおす
							for (int si = 0; si < 5; si++)
							{
								SortWeight[si] = pVertexType[nWeightIndex].m_weight[si];
								SortOffsetId[si] = pVertexType[nWeightIndex].m_matoffsetid[si];
							}
							
							SortWeight[4] = nWeight;
							SortOffsetId[4] = i;

							// ウェイトが大きい順にソート
							WeightInsertionSort(SortWeight, SortOffsetId, 5);

							// ソート用の配列から元の配列に入れなおす
							for (int si = 0; si < 4; si++)
							{
								pVertexType[nWeightIndex].m_weight[si] = SortWeight[si];
								pVertexType[nWeightIndex].m_matoffsetid[si] = SortOffsetId[si];
							}
							//MessageBox(NULL, L"このモデルの頂点に5つ以上のボーンの影響を受けているものがあります", L"警告", MB_OK);
						}
					}
				}

				// 行列オフセット取得
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"matoffset %f,%f,%f,%f",
					&m_pBone[i].m_matOffset._11, &m_pBone[i].m_matOffset._12,
					&m_pBone[i].m_matOffset._13, &m_pBone[i].m_matOffset._14);

				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"matoffset %f,%f,%f,%f",
					&m_pBone[i].m_matOffset._21, &m_pBone[i].m_matOffset._22,
					&m_pBone[i].m_matOffset._23, &m_pBone[i].m_matOffset._24);

				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"matoffset %f,%f,%f,%f",
					&m_pBone[i].m_matOffset._31, &m_pBone[i].m_matOffset._32,
					&m_pBone[i].m_matOffset._33, &m_pBone[i].m_matOffset._34);

				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"matoffset %f,%f,%f,%f",
					&m_pBone[i].m_matOffset._41, &m_pBone[i].m_matOffset._42,
					&m_pBone[i].m_matOffset._43, &m_pBone[i].m_matOffset._44);

				// 逆行列オフセット取得
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"invmatoffset %f,%f,%f,%f",
					&m_pBone[i].m_matInvOffset._11, &m_pBone[i].m_matInvOffset._12,
					&m_pBone[i].m_matInvOffset._13, &m_pBone[i].m_matInvOffset._14);

				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"invmatoffset %f,%f,%f,%f",
					&m_pBone[i].m_matInvOffset._21, &m_pBone[i].m_matInvOffset._22,
					&m_pBone[i].m_matInvOffset._23, &m_pBone[i].m_matInvOffset._24);
				
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"invmatoffset %f,%f,%f,%f",
					&m_pBone[i].m_matInvOffset._31, &m_pBone[i].m_matInvOffset._32,
					&m_pBone[i].m_matInvOffset._33, &m_pBone[i].m_matInvOffset._34);

				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"invmatoffset %f,%f,%f,%f",
					&m_pBone[i].m_matInvOffset._41, &m_pBone[i].m_matInvOffset._42,
					&m_pBone[i].m_matInvOffset._43, &m_pBone[i].m_matInvOffset._44);
			}
		}
		else if (wcscmp(FileData, L"obb") == 0)
		{
			// データ取得用変数
			D3DXVECTOR3 vScale, vPosition;

			fgetws(FileData, 256, pFile);
			fgetws(FileData, 256, pFile);
			swscanf_s(FileData, L"obb_scale %f,%f,%f",
				&vScale.x, &vScale.y, &vScale.z);
				
			fgetws(FileData, 256, pFile);
			swscanf_s(FileData, L"obb_pos %f,%f,%f",
				&vPosition.x, &vPosition.y, &vPosition.z);

			// OBB情報初期値を入れる
			m_pObb = new Collision::OBB3D;
			m_pObb->Init(vScale, vPosition);
		}
	}

	//バーテックスバッファー作成
	// CPUからアクセス可能にしました。(遅くなるかも)
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VertexType) * nVertexCount;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = pVertexType;

	CDevice3D* pDevice3D = CDevice3D::GetInstance();
	ID3D10Device* pDevice = pDevice3D->GetDevice();

	if (FAILED(pDevice->CreateBuffer(&bd, &InitData, &(m_pMeshType[0].m_pVertexBuffer))))
	{
		MessageBox(NULL, L"メッシュバーテックスバッファー作成失敗", L"エラー", MB_OK);
		return false;
	}
	
	// インデックスバッファー初期化
	m_pMeshType[0].m_ppIndexBuffer = new ID3D10Buffer*[m_pMeshType[0].m_nMaterialCount];
	for (int i = 0; i < m_pMeshType[0].m_nMaterialCount; i++)
	{
		// 使用されていないマテリアルはインデックスバッファーを作成しない
		if (m_pMeshType[0].m_pMaterialIndexCount[i] == 0) continue;

		//インデックスバッファーを作成(マテリアルの数だけ)
		// CPUからアクセス可能にしました。(遅くなるかも)
		bd.Usage = D3D10_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(int) * m_pMeshType[0].m_pMaterialIndexCount[i];
		bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		InitData.pSysMem = ppMaterialIndex[i];
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;
		if (FAILED(pDevice->CreateBuffer(&bd, &InitData, &(m_pMeshType[0].m_ppIndexBuffer[i]))))
		{
			MessageBox(NULL, L"メッシュインデックスバッファー作成失敗", L"エラー", MB_OK);

			return false;
		}
	}

	delete[] pVertexType;

	if (ppMaterialIndex != NULL)
	{
		for(int i = 0; i < m_pMeshType[0].m_nMaterialCount; i++)
		{
			delete[] ppMaterialIndex[i];
		}
		delete[] ppMaterialIndex;
	}

	return true;
}

// nxファイルの頂点情報を取得する関数
void CModel::NXLoadVertex(FILE** pFile, VertexType** pVertexType, int* pVertexCount)
{
	// ファイルに入っている文字列を取得する変数
	wchar_t FileData[256];

	// 頂点数取得
	fwscanf_s(*pFile, L"%s", FileData, 256);
	swscanf_s(FileData, L"%d", pVertexCount);

	// 頂点データを入れる入れ物作成
	*pVertexType = new VertexType[*pVertexCount];

	fgetws(FileData, 256, *pFile);
	for (int i = 0; i < *pVertexCount; i++)
	{
		// 頂点位置取得
		fgetws(FileData, 256, *pFile);
		swscanf_s(FileData, L"v %f,%f,%f",
			&(*pVertexType)[i].m_vPos.x,
			&(*pVertexType)[i].m_vPos.y,
			&(*pVertexType)[i].m_vPos.z);
	}
}

// nxファイルの法線情報を取得する関数
void CModel::NXLoadNormal(FILE** pFile, VertexType** pVertexType, int nVertexCount)
{
	// ファイルに入っている文字列を取得する変数
	wchar_t FileData[256];

	fwscanf_s(*pFile, L"%s", FileData, 256);

	fgetws(FileData, 256, *pFile);
	for (int i = 0; i < nVertexCount; i++)
	{
		// 法線情報取得
		fgetws(FileData, 256, *pFile);
		swscanf_s(FileData, L"n %f,%f,%f",
			&(*pVertexType)[i].m_vNor.x,
			&(*pVertexType)[i].m_vNor.y,
			&(*pVertexType)[i].m_vNor.z);
	}
}

// nxファイルのUV情報を取得する関数
void CModel::NXLoadUv(FILE** pFile, VertexType** pVertexType, int nVertexCount)
{
	// ファイルに入っている文字列を取得する変数
	wchar_t FileData[256];

	fwscanf_s(*pFile, L"%s", FileData, 256);

	fgetws(FileData, 256, *pFile);
	for (int i = 0; i < nVertexCount; i++)
	{
		// uv情報取得
		fgetws(FileData, 256, *pFile);
		swscanf_s(FileData, L"uv %f,%f",
			&(*pVertexType)[i].m_vTex.x,
			&(*pVertexType)[i].m_vTex.y);
	}
}

void CModel::NXLoadMaterial(FILE** pFile, VertexType** pVertexType, int nVertexCount, int*** pppMaterialIndex)
{
	// ファイルに入っている文字列を取得する変数
	wchar_t FileData[256];

	// マテリアルの数取得
	fwscanf_s(*pFile, L"%s", FileData, 256);
	swscanf_s(FileData, L"%d", &m_pMeshType[0].m_nMaterialCount);

	m_pMeshType[0].m_pMaterialType = new MaterialType[m_pMeshType[0].m_nMaterialCount];

	fgetws(FileData, 256, *pFile);
	for (int i = 0; i < m_pMeshType[0].m_nMaterialCount; i++)
	{
		// テクスチャ情報取得
		fgetws(FileData, 256, *pFile);
		wchar_t sTextureName[256];
		if (swscanf_s(FileData, L"tex %s", sTextureName, 256) != EOF)
		{
			// テクスチャを作成
			CTextureManager* pTextureManager = CTextureManager::GetInstance();
			pTextureManager->CreateTexture(sTextureName);
			m_pMeshType[0].m_pMaterialType[i].m_pTexture = pTextureManager->GetTexture(sTextureName);
		}

		// デフューズ
		fgetws(FileData, 256, *pFile);
		swscanf_s(FileData, L"dif %f,%f,%f,%f",
			&m_pMeshType[0].m_pMaterialType[i].m_vDiffuseColor.x,
			&m_pMeshType[0].m_pMaterialType[i].m_vDiffuseColor.y,
			&m_pMeshType[0].m_pMaterialType[i].m_vDiffuseColor.z,
			&m_pMeshType[0].m_pMaterialType[i].m_vDiffuseColor.w);

		// スペキュラーパワー
		fgetws(FileData, 256, *pFile);
		swscanf_s(FileData, L"spep %f",
			&m_pMeshType[0].m_pMaterialType[i].m_fSpecularPower);

		// スペキュラーカラー
		fgetws(FileData, 256, *pFile);
		swscanf_s(FileData, L"spe %f,%f,%f",
			&m_pMeshType[0].m_pMaterialType[i].m_vSpecularColor.x,
			&m_pMeshType[0].m_pMaterialType[i].m_vSpecularColor.y,
			&m_pMeshType[0].m_pMaterialType[i].m_vSpecularColor.z);

		m_pMeshType[0].m_pMaterialType[i].m_vSpecularColor.w = 0.f;

		// エミッシブ
		fgetws(FileData, 256, *pFile);
		swscanf_s(FileData, L"emi %f,%f,%f",
			&m_pMeshType[0].m_pMaterialType[i].m_vEmission.x,
			&m_pMeshType[0].m_pMaterialType[i].m_vEmission.y,
			&m_pMeshType[0].m_pMaterialType[i].m_vEmission.z);

		m_pMeshType[0].m_pMaterialType[i].m_vEmission.z = 0.f;
	}

	// マテリアルインデックス数を取得
	m_pMeshType[0].m_pMaterialIndexCount = new int[m_pMeshType[0].m_nMaterialCount];
	*pppMaterialIndex = new int*[m_pMeshType[0].m_nMaterialCount];

	fgetws(FileData, 256, *pFile);

	// マテリアルインデックスの中身を初期化
	for (int i = 0; i < m_pMeshType[0].m_nMaterialCount; i++)
	{
		// マテリアルを適用する面の数を取得
		int nMaterialSurfaceCount = 0;
		fgetws(FileData, 256, *pFile);
		swscanf_s(FileData, L"matesurfacenum %d",
			&nMaterialSurfaceCount);

		// インデックス数を入れる
		m_pMeshType[0].m_pMaterialIndexCount[i] = nMaterialSurfaceCount * 3;

		(*pppMaterialIndex)[i] = new int[m_pMeshType[0].m_pMaterialIndexCount[i]];

		for (int j = 0; j < nMaterialSurfaceCount; j++)
		{
			int nMaterialIndex;
			fgetws(FileData, 256, *pFile);
			swscanf_s(FileData, L"mate %d",
				&nMaterialIndex);

			(*pppMaterialIndex)[i][j * 3 + 0] = nMaterialIndex * 3 + 0;
			(*pppMaterialIndex)[i][j * 3 + 1] = nMaterialIndex * 3 + 1;
			(*pppMaterialIndex)[i][j * 3 + 2] = nMaterialIndex * 3 + 2;
		}
	}
}

// スキンアニメーション計算
void CModel::SkinAnimation(float fFrameSecond, int nAnimationId)
{
	if (m_nBoneCount == 0) return;

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity(&matIdentity);
	RecursionAnimation(&m_pBone[0], NULL, NULL, nAnimationId, fFrameSecond);
}

// 親から子へアニメーション計算をする(再帰関数)
void CModel::RecursionAnimation(Bone* pBone, const D3DXMATRIX* pmatOyaAni, const D3DXMATRIX* pmatOyaInv, int nAnimationId, float fFrameSecond)
{
	D3DXMATRIX matScaleOld, matRotationOld, matTranslateOld;
	D3DXMATRIX matAniamtion, matScale, matRotation, matTranslate;

	// 整数値を取得
	int nFrameSecond = (int)fFrameSecond;

	// 拡大縮小
	D3DXMatrixScaling(&matScaleOld,
		pBone->m_pAnimationKey[nAnimationId].m_pvScale[nFrameSecond].x,
		pBone->m_pAnimationKey[nAnimationId].m_pvScale[nFrameSecond].y, 
		pBone->m_pAnimationKey[nAnimationId].m_pvScale[nFrameSecond].z);

	// 回転行列
	matRotationOld = pBone->m_pAnimationKey[nAnimationId].m_pmatRotation[nFrameSecond];

	// 平行移動
	D3DXMatrixTranslation(&matTranslateOld,
		pBone->m_pAnimationKey[nAnimationId].m_pvTranslation[nFrameSecond].x,
		pBone->m_pAnimationKey[nAnimationId].m_pvTranslation[nFrameSecond].y,
		pBone->m_pAnimationKey[nAnimationId].m_pvTranslation[nFrameSecond].z);

	if (nFrameSecond + 1 >= m_nEndFrame[nAnimationId])
	{
		D3DXMatrixScaling(&matScale,
			pBone->m_pAnimationKey[nAnimationId].m_pvScale[0].x,
			pBone->m_pAnimationKey[nAnimationId].m_pvScale[0].y,
			pBone->m_pAnimationKey[nAnimationId].m_pvScale[0].z);

		matRotation = pBone->m_pAnimationKey[nAnimationId].m_pmatRotation[0];

		D3DXMatrixTranslation(&matTranslate,
			pBone->m_pAnimationKey[nAnimationId].m_pvTranslation[0].x,
			pBone->m_pAnimationKey[nAnimationId].m_pvTranslation[0].y,
			pBone->m_pAnimationKey[nAnimationId].m_pvTranslation[0].z);
	}
	else
	{
		D3DXMatrixScaling(&matScale,
			pBone->m_pAnimationKey[nAnimationId].m_pvScale[nFrameSecond + 1].x,
			pBone->m_pAnimationKey[nAnimationId].m_pvScale[nFrameSecond + 1].y,
			pBone->m_pAnimationKey[nAnimationId].m_pvScale[nFrameSecond + 1].z);

		matRotation = pBone->m_pAnimationKey[nAnimationId].m_pmatRotation[nFrameSecond + 1];

		D3DXMatrixTranslation(&matTranslate,
			pBone->m_pAnimationKey[nAnimationId].m_pvTranslation[nFrameSecond + 1].x,
			pBone->m_pAnimationKey[nAnimationId].m_pvTranslation[nFrameSecond + 1].y,
			pBone->m_pAnimationKey[nAnimationId].m_pvTranslation[nFrameSecond + 1].z);
	}
	
	// 補間式---------------------------------
	matScale     = (1.f - (fFrameSecond - (float)nFrameSecond)) * matScaleOld	  + (fFrameSecond - (float)nFrameSecond) * matScale;
	matRotation  = (1.f - (fFrameSecond - (float)nFrameSecond)) * matRotationOld  + (fFrameSecond - (float)nFrameSecond) * matRotation;
	matTranslate = (1.f - (fFrameSecond - (float)nFrameSecond)) * matTranslateOld + (fFrameSecond - (float)nFrameSecond) * matTranslate;
	// ---------------------------------------

	// 座標変換---------------------------------------
	D3DXMatrixMultiply(&matAniamtion, &matRotation, &matScale);
	D3DXMatrixMultiply(&matAniamtion, &matAniamtion, &matTranslate);
	// -----------------------------------------------s

	// 自分のアニメーション行列計算
	pBone->m_matAnimation = pBone->m_matOffset * matAniamtion  * pBone->m_matInvOffset;

	// 親ボーン分のアニメーションを反映させる
	for (int i = pBone->m_nBoneDepthCount - 1; i >= 0; i--)
	{
		pBone->m_matAnimation = pBone->m_matAnimation * pmatOyaAni[i] * pmatOyaInv[i];
	}

	// 子ボーンに渡す行列データ作成
	pBone->m_pmatOyaAnimation = new D3DXMATRIX[pBone->m_nBoneDepthCount + 1];
	pBone->m_pmatOyaInvOffset = new D3DXMATRIX[pBone->m_nBoneDepthCount + 1];

	// 親ボーンのアニメーション行列と逆ボーン行列情報を子ボーンに渡す行列データに入れる
	for (int i = 0; i < pBone->m_nBoneDepthCount; i++)
	{
		pBone->m_pmatOyaAnimation[i] = pmatOyaAni[i];
		pBone->m_pmatOyaInvOffset[i] = pmatOyaInv[i];
	}

	// 子ボーンに渡す行列データに自分のアニメーション行列と逆ボーン行列情報を入れる
	pBone->m_pmatOyaAnimation[pBone->m_nBoneDepthCount] = matAniamtion;
	pBone->m_pmatOyaInvOffset[pBone->m_nBoneDepthCount] = pBone->m_matInvOffset;

	// 子ボーンの数だけ、アニメーション情報を送ってアニメーション行列を作成する
	for (int i = 0; i < pBone->m_nBoneChildCount; i++)
	{
		RecursionAnimation(pBone->m_ppBoneChild[i], pBone->m_pmatOyaAnimation,
			pBone->m_pmatOyaInvOffset, nAnimationId, fFrameSecond);
	}

	// アニメーションを作成し終えたので削除
	delete[] pBone->m_pmatOyaAnimation;
	pBone->m_pmatOyaAnimation = NULL;
	delete[] pBone->m_pmatOyaInvOffset;
	pBone->m_pmatOyaInvOffset = NULL;
}

// 四角形を作成する(3D描画用)
void CModel::CreateQuadrangle3D()
{
	VertexType* pVertexType = NULL;
	int* pIndex = NULL;

	m_nMeshCount = 1;
	m_pMeshType = new MeshType[m_nMeshCount];

	m_pMeshType[0].m_nMaterialCount = 1;
	m_pMeshType[0].m_pMaterialType = new MaterialType[m_pMeshType[0].m_nMaterialCount];
	m_pMeshType[0].m_pMaterialIndexCount = new int[m_pMeshType[0].m_nMaterialCount];
	m_pMeshType[0].m_ppIndexBuffer = new ID3D10Buffer*[m_pMeshType[0].m_nMaterialCount];
	m_pMeshType[0].m_pMaterialIndexCount[0] = 6;

	// バーテックス情報設定--------------------------------------------------
	pVertexType = new VertexType[4];

	// 頂点設定
	pVertexType[0].m_vPos = D3DXVECTOR3(-0.5f, 0.5f, 0.f); // 左上
	pVertexType[1].m_vPos = D3DXVECTOR3(0.5f, 0.5f, 0.f); // 右上
	pVertexType[2].m_vPos = D3DXVECTOR3(-0.5f, -0.5f, 0.f); // 左下
	pVertexType[3].m_vPos = D3DXVECTOR3(0.5f, -0.5f, 0.f); // 右下

	// 法線設定
	for (int i = 0; i < 4; i++)
		pVertexType[i].m_vNor = D3DXVECTOR3(0, 0, -1);

	// uv設定
	pVertexType[0].m_vTex = D3DXVECTOR2(0.f, 0.f);		// 左上
	pVertexType[1].m_vTex = D3DXVECTOR2(1.f, 0.f);		// 右上
	pVertexType[2].m_vTex = D3DXVECTOR2(0.f, 1.f);		// 左下
	pVertexType[3].m_vTex = D3DXVECTOR2(1.f, 1.f);		// 右下
	// ----------------------------------------------------------------------

	// インデックス情報設定
	pIndex = new int[6];

	pIndex[0] = 0;
	pIndex[1] = 1;
	pIndex[2] = 2;
	pIndex[3] = 1;
	pIndex[4] = 3;
	pIndex[5] = 2;

	//バーテックスバッファー作成
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VertexType) * 4;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = pVertexType;

	CDevice3D* pDevice3D = CDevice3D::GetInstance();
	ID3D10Device* pDevice = pDevice3D->GetDevice();

	// バーテックスバッファー作成
	if (FAILED(pDevice->CreateBuffer(&bd, &InitData, &(m_pMeshType[0].m_pVertexBuffer))))
	{
		MessageBox(NULL, L"メッシュバーテックスバッファー作成失敗", L"エラー", MB_OK);

		return;
	}

	//インデックスバッファー作成
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(int) * m_pMeshType[0].m_pMaterialIndexCount[0];
	bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	InitData.pSysMem = pIndex;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	if (FAILED(pDevice->CreateBuffer(&bd, &InitData, &(m_pMeshType[0].m_ppIndexBuffer[0]))))
	{
		MessageBox(NULL, L"メッシュバーテックスバッファー作成失敗", L"エラー", MB_OK);

		return;
	}

	delete[] pVertexType;
	delete[] pIndex;
}

// 四角形を作成する(2D描画用)
void CModel::CreateQuadrangle2D()
{
	VertexType* pVertexType = NULL;
	int* pIndex = NULL;

	m_nMeshCount = 1;
	m_pMeshType = new MeshType[m_nMeshCount];
	
	m_pMeshType[0].m_nMaterialCount = 1;
	m_pMeshType[0].m_pMaterialType		 = new MaterialType[m_pMeshType[0].m_nMaterialCount];
	m_pMeshType[0].m_pMaterialIndexCount = new int[m_pMeshType[0].m_nMaterialCount];
	m_pMeshType[0].m_ppIndexBuffer		 = new ID3D10Buffer*[m_pMeshType[0].m_nMaterialCount];
	m_pMeshType[0].m_pMaterialIndexCount[0] = 6;

	// バーテックス情報設定--------------------------------------------------
	pVertexType = new VertexType[4];
	
	// 頂点設定
	pVertexType[0].m_vPos = D3DXVECTOR3(-0.5f, -0.5f, 0.f); // 左上
	pVertexType[1].m_vPos = D3DXVECTOR3( 0.5f, -0.5f, 0.f); // 右上
	pVertexType[2].m_vPos = D3DXVECTOR3(-0.5f,  0.5f, 0.f); // 左下
	pVertexType[3].m_vPos = D3DXVECTOR3( 0.5f,  0.5f, 0.f); // 右下

	// 法線設定
	for (int i = 0; i < 4; i++)
		pVertexType[i].m_vNor = D3DXVECTOR3(0, 0, -1);

	// uv設定
	pVertexType[0].m_vTex = D3DXVECTOR2( 0.f, 0.f);		// 左上
	pVertexType[1].m_vTex = D3DXVECTOR2( 1.f, 0.f);		// 右上
	pVertexType[2].m_vTex = D3DXVECTOR2( 0.f, 1.f);		// 左下
	pVertexType[3].m_vTex = D3DXVECTOR2( 1.f, 1.f);		// 右下
	// ----------------------------------------------------------------------

	// インデックス情報設定
	pIndex = new int[6];

	pIndex[0] = 0;
	pIndex[1] = 1;
	pIndex[2] = 2;
	pIndex[3] = 1;
	pIndex[4] = 3;
	pIndex[5] = 2;
	
	//バーテックスバッファー作成
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VertexType) * 4;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = pVertexType;

	CDevice3D* pDevice3D = CDevice3D::GetInstance();
	ID3D10Device* pDevice = pDevice3D->GetDevice();

	// バーテックスバッファー作成
	if (FAILED(pDevice->CreateBuffer(&bd, &InitData, &(m_pMeshType[0].m_pVertexBuffer))))
	{
		MessageBox(NULL, L"メッシュバーテックスバッファー作成失敗", L"エラー", MB_OK);

		return;
	}

	//インデックスバッファー作成
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(int) * m_pMeshType[0].m_pMaterialIndexCount[0];
	bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	InitData.pSysMem = pIndex;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	if (FAILED(pDevice->CreateBuffer(&bd, &InitData, &(m_pMeshType[0].m_ppIndexBuffer[0]))))
	{
		MessageBox(NULL, L"メッシュバーテックスバッファー作成失敗", L"エラー", MB_OK);

		return;
	}

	delete[] pVertexType;
	delete[] pIndex;
}

// 点情報からモデルを作成する
void CModel::PointToCreateModel(const D3DXVECTOR3* pPoints, int PointNum)
{
	VertexType* pVertexType = NULL;
	int* pIndex = NULL;

	m_nMeshCount = 1;
	m_pMeshType = new MeshType[m_nMeshCount];

	m_pMeshType[0].m_nMaterialCount = 1;
	m_pMeshType[0].m_pMaterialType = new MaterialType[m_pMeshType[0].m_nMaterialCount];
	m_pMeshType[0].m_pMaterialIndexCount = new int[m_pMeshType[0].m_nMaterialCount];
	m_pMeshType[0].m_ppIndexBuffer = new ID3D10Buffer*[m_pMeshType[0].m_nMaterialCount];
	m_pMeshType[0].m_pMaterialIndexCount[0] = PointNum;

	// バーテックス情報設定--------------------------------------------------
	pVertexType = new VertexType[PointNum];

	for (int i = 0; i < PointNum; i++)
	{
		// 頂点設定
		pVertexType[i].m_vPos = pPoints[i]; // 左上
	}

	// 法線設定
	for (int i = 0; i < PointNum; i++)
		pVertexType[i].m_vNor = D3DXVECTOR3(0, 0, -1);

	for (int i = 0; i < PointNum; i++)
	{
		pVertexType[i].m_vTex = D3DXVECTOR2(0.f, 0.f);
	}
	// ----------------------------------------------------------------------

	// インデックス情報設定
	pIndex = new int[PointNum];

	for (int i = 0; i < PointNum; i++)
	{
		pIndex[i] = i;
	}

	//バーテックスバッファー作成
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VertexType) * PointNum;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = pVertexType;

	CDevice3D* pDevice3D = CDevice3D::GetInstance();
	ID3D10Device* pDevice = pDevice3D->GetDevice();

	// バーテックスバッファー作成
	if (FAILED(pDevice->CreateBuffer(&bd, &InitData, &(m_pMeshType[0].m_pVertexBuffer))))
	{
		MessageBox(NULL, L"メッシュバーテックスバッファー作成失敗", L"エラー", MB_OK);

		return;
	}

	//インデックスバッファー作成
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(int) * m_pMeshType[0].m_pMaterialIndexCount[0];
	bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	InitData.pSysMem = pIndex;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	if (FAILED(pDevice->CreateBuffer(&bd, &InitData, &(m_pMeshType[0].m_ppIndexBuffer[0]))))
	{
		MessageBox(NULL, L"メッシュバーテックスバッファー作成失敗", L"エラー", MB_OK);

		return;
	}

	delete[] pVertexType;
	delete[] pIndex;
}

// nxファイルのアニメーションを読み込む
bool CModel::LoadNXAnimation(const wchar_t* pFileName, int id)
{
	FILE* pFile;
	wchar_t FileData[256];

	// ファイルオープン
	if (_wfopen_s(&pFile, pFileName, L"r") != 0)
	{
		wchar_t ErrorString[256];
		swprintf_s(ErrorString, 256, L"%sが見つかりませんでした。",
			pFileName);
		MessageBox(NULL, ErrorString, L"エラー", MB_OK);
		return false;
	}

	fwscanf_s(pFile, L"%s", FileData, 256);

	// アニメーション取得
	if (wcscmp(FileData, L"framenum") == 0)
	{
		fwscanf_s(pFile, L"%s", FileData, 256);
		// アニメーション最大フレーム数取得
		swscanf_s(FileData, L"%d", &m_nEndFrame[id]);

		for (int i = 0; i < m_nBoneCount; i++)
		{			
			m_pBone[i].m_pAnimationKey[id].m_pmatRotation  = new D3DXMATRIX[m_nEndFrame[id]];
			m_pBone[i].m_pAnimationKey[id].m_pvScale		 = new D3DXVECTOR3[m_nEndFrame[id]];
			m_pBone[i].m_pAnimationKey[id].m_pvTranslation = new D3DXVECTOR3[m_nEndFrame[id]];

			fgetws(FileData, 256, pFile);
			// アニメーションフレーム最大数までアニメーション情報取得
			for (int j = 0; j < m_nEndFrame[id]; j++)
			{
				fgetws(FileData, 256, pFile);

				// 回転情報取得
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"rota %f,%f,%f,%f",
					&m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._11, &m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._12,
					&m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._13, &m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._14);
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"rota %f,%f,%f,%f",
					&m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._21, &m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._22,
					&m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._23, &m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._24);
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"rota %f,%f,%f,%f",
					&m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._31, &m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._32,
					&m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._33, &m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._34);
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"rota %f,%f,%f,%f",
					&m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._41, &m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._42,
					&m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._43, &m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._44);

				// 拡大縮小情報取得
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"scale %f,%f,%f",
					&m_pBone[i].m_pAnimationKey[id].m_pvScale[j].x, &m_pBone[i].m_pAnimationKey[id].m_pvScale[j].y,
					&m_pBone[i].m_pAnimationKey[id].m_pvScale[j].z);

				// 平行移動情報取得
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"trans %f,%f,%f",
					&m_pBone[i].m_pAnimationKey[id].m_pvTranslation[j].x, &m_pBone[i].m_pAnimationKey[id].m_pvTranslation[j].y,
					&m_pBone[i].m_pAnimationKey[id].m_pvTranslation[j].z);
			}
		}
	}

	return true;
}

// ウェイト値をソートする
void WeightInsertionSort(float* p_fSortWeight, int* p_nSortOffsetId, int nElementCount)
{
	for (int i = 0; i < nElementCount; i++)
	{
		for (int j = 0; j < i; j++)
		{
			if (p_fSortWeight[i] > p_fSortWeight[j])
			{
				// 入れ替え
				float tempWeight;
				tempWeight = p_fSortWeight[i];
				p_fSortWeight[i] = p_fSortWeight[j];
				p_fSortWeight[j] = tempWeight;

				int tempOffsetId;
				tempOffsetId = p_nSortOffsetId[i];
				p_nSortOffsetId[i] = p_nSortOffsetId[j];
				p_nSortOffsetId[j] = tempOffsetId;
			}
		}
	}
}
