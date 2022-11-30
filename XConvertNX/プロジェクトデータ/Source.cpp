#include <stdio.h>
#include <conio.h>
#include <wchar.h>
#include <math.h>

#include "Source.h"
#include "math.h"

#define DELETE_ARRAY(p) { if(p != NULL) delete[] p; }

bool LoadXFile(wchar_t* sFileName);
bool CreateNXFile(wchar_t* sFileName);
bool CreateNXAnimationFile(wchar_t* sFileName);
void MemoryRelease();

// XNファイルを作るのに必要な物----------------------------------------------
float* g_pVertex		 = NULL; // 頂点位置データ
float* g_pTex			 = NULL; // uvデータ
float* g_pNormal		 = NULL; // 法線データ
float* g_pVertexColor	 = NULL; // 頂点カラーデータ
int* g_pIndexVertex		 = NULL; // 頂点インデックスデータ
int* g_pIndexTex		 = NULL; // uvインデックスデータ
int* g_pIndexNormal		 = NULL; // 法線インデックスデータ
int* g_pIndexVertexColor = NULL; // 頂点カラーインデックスデータ
int** g_ppIndexMaterial  = NULL; // マテリアル毎のインデックス情報
// --------------------------------------------------------------------------

bool g_isRoot = false;			 // Root情報取得フラグ(Root文字が変なところにあった場合の対策)

Matrix4x4 g_RootMatrix4x4;		 // Root行列を入れる入れ物
Matrix4x4 g_matArmature4x4;		 // Armature行列を入れる入れ物

int g_nVertexCount = 0;			 // 頂点数
int g_nTexCount = 0;			 // uv数
int g_nNormalCount = 0;			 // 法線数
int g_nVertexColorCount = 0;	 // 頂点カラー数
int g_nIndexCount = 0;			 // インデックス数

// ボーン情報--------------------------------
wchar_t** g_ppBoneName;					// ボーン名(xファイルではボーン情報の順番がおかしい時があるのでその対策用)
int* g_pBoneChild = NULL;				// ボーンの子情報の数
int g_nBoneCount = 0;					// ボーン数
int* g_pSkinWeightsCount = NULL;		// ウェイト数
float** g_ppSkinWeights	 = NULL;		// ウェイト値
int** g_ppSkinWeightsIndex = NULL;		// ウェイトインデックス情報
Matrix4x4* g_pBoneMatrix4x4 = NULL;		// ボーンオフセット値
D3DXMATRIX* g_pBoneInvMatrix4x4 = NULL;	// 逆ボーンオフセット値(逆行列)
int* g_pBoneDepth = NULL;				// 親が0、子の深さの分だけ増える
// -----------------------------------------

// アニメーション情報------------------------
AnimationKey* g_pAnimationKey = NULL;
// ------------------------------------------

// マテリアル処理用変数------------------------------
Material* g_pMaterial = NULL;			// マテリアル情報
int g_nMaterialCount	= 0;			// マテリアルの総数
int* g_pMaterialNumber = NULL;			// 面の数だけマテリアル番号を入れる入れ物	 
int* g_pMaterialSurfaceNum;				// それぞれのマテリアルを反映させる面の数
int* g_pTriangleSurfaceNumber = NULL;	// 面情報の四角形を三角形にする
int g_nTriangleSurfaceCount = 0;		// 三角形の面の数(四角形を三角形にするときに必要)
int g_nSurfaceCount = 0;				// 面の数(四角形を三角形にするときに必要)
int g_nInitIndex = 0;					// インデックス数初期値(四角形を三角形にするときに必要)
// ----------------------------------------------------

// OBB情報を書き込む用変数--------------------------------------------------------
Vector3D object_scale[2] = { Vector3D(0.f, 0.f, 0.f), Vector3D(0.f, 0.f, 0.f) }; // オブジェクトの大きさ [0]最大値 [1]最小値
Vector3D obb_scale = { 0.f, 0.f, 0.f};	   // OBB大きさ
Vector3D obb_position = { 0.f, 0.f, 0.f};  // OBBの位置情報
// -------------------------------------------------------------------------------

int main()
{
	wchar_t sFileName[256];

	printf_s("変換するXファイルを入力してください\n");
	wscanf_s(L"%s", sFileName, 256);

	// Xファイルを読み込む
	if (LoadXFile(sFileName) == false)
	{
		MemoryRelease();
		return -1;
	}

	// xファイル名のxの部分をnxに変える
	for (int i = 0; i < 256; i++)
	{
		if (sFileName[i] == L'\0')
		{
			sFileName[i - 1] = L'n';
			sFileName[i] = L'x';
			sFileName[i + 1] = L'\0';
			break;
		}
	}

	// nxファイルを作成
	if (CreateNXFile(sFileName) == false)
	{
		MemoryRelease();
		return -1;
	}

	// メモリ解放
	MemoryRelease();

	printf_s("変換が終了しました。\n");

	_getch();

	return 0;
}

bool LoadXFile(wchar_t* sFileName)
{
	FILE* pFile;  // 読み込むファイル
	wchar_t FileData[256];

	g_RootMatrix4x4.Identity();

	// 読み込みファイルを開く
	if (_wfopen_s(&pFile, sFileName, L"r") != 0)
	{
		// ファイルが見つからなかった場合
		wchar_t ErrorString[256];
		swprintf_s(ErrorString, 256, L"%sが見つかりませんでした。", sFileName);
		wprintf_s(ErrorString);
		return false;
	}

	while (fwscanf_s(pFile, L"%s", FileData, 256) != EOF)
	{
		// Blender用変換行列取得
		if (wcscmp(FileData, L"Root") == 0)
		{
			if (g_isRoot == true) continue; // End of Rootを回避
			fwscanf_s(pFile, L"%s", FileData, 256);
			fwscanf_s(pFile, L"%s", FileData, 256);
			fwscanf_s(pFile, L"%s", FileData, 256);
			fgetws(FileData, 256, pFile);

			// 行列情報取得
			for (int i = 0; i < 4; i++)
			{
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"%f,%f,%f,%f",
					(&g_RootMatrix4x4._11) + i * 4, 
					(&g_RootMatrix4x4._12) + i * 4, 
					(&g_RootMatrix4x4._13) + i * 4,
					(&g_RootMatrix4x4._14) + i * 4);
			}

			g_isRoot = true;

			fgetws(FileData, 256, pFile);
			fgetws(FileData, 256, pFile);
			fgetws(FileData, 256, pFile);

			// 行列情報取得
			for (int i = 0; i < 4; i++)
			{
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"%f,%f,%f,%f",
					(&g_matArmature4x4._11) + i * 4,
					(&g_matArmature4x4._12) + i * 4,
					(&g_matArmature4x4._13) + i * 4,
					(&g_matArmature4x4._14) + i * 4);
			}
		}
		// 頂点読み込み
		else if (wcscmp(FileData, L"Mesh") == 0)
		{
			fgetws(FileData, 256, pFile);
			// 頂点数を取得
			fwscanf_s(pFile, L"%s", FileData, 256);

			if (FileData[0] == L'<') continue; //templateは省く

			swscanf_s(FileData, L"%d;", &g_nVertexCount);
			// 頂点の数だけ頂点情報を入れる箱を作る
			g_pVertex = new float[g_nVertexCount * 3];

			fgetws(FileData, 256, pFile);
			// 頂点情報取得
			for (int i = 0; i < g_nVertexCount; i++)
			{
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"%f;%f;%f",
					&g_pVertex[i * 3 + 0],
					&g_pVertex[i * 3 + 1],
					&g_pVertex[i * 3 + 2]);

				// 頂点の位置を正しい位置に変換(blender限定)
				Vector3D vertex_vec = Vector3D(g_pVertex[i * 3 + 0], g_pVertex[i * 3 + 1], g_pVertex[i * 3 + 2]);
				vertex_vec = g_RootMatrix4x4.TransformPointVector3D(vertex_vec);
				g_pVertex[i * 3 + 0] = vertex_vec.x;
				g_pVertex[i * 3 + 1] = vertex_vec.y;
				g_pVertex[i * 3 + 2] = vertex_vec.z;

				// 頂点の最大値と最低値を調べる(OBBでの当たり判定情報に調べる)
				if (object_scale[0].x < vertex_vec.x)
				{
					object_scale[0].x = vertex_vec.x;
				}
				if (object_scale[1].x > vertex_vec.x)
				{
					object_scale[1].x = vertex_vec.x;
				}
				if (object_scale[0].y < vertex_vec.y)
				{
					object_scale[0].y = vertex_vec.y;
				}
				if (object_scale[1].y > vertex_vec.y)
				{
					object_scale[1].y = vertex_vec.y;
				}
				if (object_scale[0].z < vertex_vec.z)
				{
					object_scale[0].z = vertex_vec.z;
				}
				if (object_scale[1].z > vertex_vec.z)
				{
					object_scale[1].z = vertex_vec.z;
				}
			}

			// 頂点インデックス数を取得
			fwscanf_s(pFile, L"%s", FileData, 256);
			swscanf_s(FileData, L"%d;", &g_nIndexCount);

			g_nInitIndex = g_nIndexCount; // インデックス初期値を保存

			// 頂点の数だけ頂点情報を入れる箱を作る
			g_pIndexVertex = new int[g_nIndexCount * 2 * 3];

			g_pTriangleSurfaceNumber = new int[g_nIndexCount * 2];

			// 頂点インデックス情報取得
			for (int i = 0, count = 0; i < g_nIndexCount; i++, count++)
			{
				fwscanf_s(pFile, L"%s", FileData, 256);
				if (swscanf_s(FileData, L"3;%d,%d,%d;,",
					&g_pIndexVertex[i * 3 + 0],
					&g_pIndexVertex[i * 3 + 1],
					&g_pIndexVertex[i * 3 + 2])
					== 0)
				{
					// 四角形があったので三角形に変換
					int index1, index2, index3, index4;
					swscanf_s(FileData, L"4;%d,%d,%d,%d;,",
						&index1, &index2, &index3, &index4);
					
					g_pIndexVertex[i * 3]			= index1;
					g_pIndexVertex[i * 3 + 1]		= index2;
					g_pIndexVertex[i * 3 + 2]		= index3;

					g_pIndexVertex[(i + 1) * 3]		= index3;
					g_pIndexVertex[(i + 1) * 3 + 1] = index4;
					g_pIndexVertex[(i + 1) * 3 + 2] = index1;

					g_nIndexCount++;
					i++;

					//-----------------------------------------------------------
					// マテリアル適用面番号を割り当てる
					g_pTriangleSurfaceNumber[g_nTriangleSurfaceCount] = count;
					g_nTriangleSurfaceCount++;
					//-----------------------------------------------------------
				}

				//-----------------------------------------------------------
				// マテリアル適用面番号を割り当てる
				g_pTriangleSurfaceNumber[g_nTriangleSurfaceCount] = count;
				g_nTriangleSurfaceCount++;
				//-----------------------------------------------------------
			}

		}
		// 頂点カラー読み込み
		else if (wcscmp(FileData, L"MeshVertexColors") == 0)
		{
			fgetws(FileData, 256, pFile);
			// 頂点カラー数を取得
			fwscanf_s(pFile, L"%s", FileData, 256);

			if (FileData[0] == L'<') continue; //templateは省く

			swscanf_s(FileData, L"%d;", &g_nVertexColorCount);
			// 頂点カラーの数だけ頂点カラー情報を入れる箱を作る
			g_pVertexColor = new float[g_nVertexColorCount * 4];

			fgetws(FileData, 256, pFile);
			// 頂点カラー情報取得
			for (int i = 0; i < g_nVertexColorCount; i++)
			{
				// ダミー用変数
				int none;

				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"%d; %f; %f; %f; %f;;,",
					&none,
					&g_pVertexColor[i * 4 + 0],
					&g_pVertexColor[i * 4 + 1],
					&g_pVertexColor[i * 4 + 2],
					&g_pVertexColor[i * 4 + 3]);
			}

			fwscanf_s(pFile, L"%s", FileData, 256);
			// 頂点カラーの数だけ頂点カラーを入れる箱を作る
			g_pIndexVertexColor = new int[g_nIndexCount * 3];

			fgetws(FileData, 256, pFile);
			// 頂点カラーインデックス作成
			// 頂点カラーインデックスは頂点インデックスと同じにする
			memcpy_s(g_pIndexVertexColor, sizeof(int) * g_nIndexCount * 3,
				g_pIndexVertex, sizeof(int) * g_nIndexCount * 3);
		}
		// 法線読み込み
		else if (wcscmp(FileData, L"MeshNormals") == 0)
		{
			fgetws(FileData, 256, pFile);
			// 法線数を取得
			fwscanf_s(pFile, L"%s", FileData, 256);

			if (FileData[0] == L'<') continue; //templateは省く

			swscanf_s(FileData, L"%d;", &g_nNormalCount);
			// 法線の数だけ法線情報を入れる箱を作る
			g_pNormal = new float[g_nNormalCount * 3];

			fgetws(FileData, 256, pFile);
			// 法線情報取得
			for (int i = 0; i < g_nNormalCount; i++)
			{
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"%f;%f;%f",
					&g_pNormal[i * 3 + 0], &g_pNormal[i * 3 + 1], &g_pNormal[i * 3 + 2]);

				// 法線の向きを正しい向きに変換(blender専用)
				Vector3D normal_vec = Vector3D(g_pNormal[i * 3 + 0], g_pNormal[i * 3 + 1], g_pNormal[i * 3 + 2]);
				normal_vec = g_RootMatrix4x4.TransformNormalVector3D(normal_vec);

				g_pNormal[i * 3 + 0] = normal_vec.x;
				g_pNormal[i * 3 + 1] = normal_vec.y;
				g_pNormal[i * 3 + 2] = normal_vec.z;
			}
			
			fwscanf_s(pFile, L"%s", FileData, 256);
			// 法線インデックスの数だけインデックス情報を入れる箱を作る
			g_pIndexNormal = new int[g_nIndexCount * 3];

			fgetws(FileData, 256, pFile);
			// 法線インデックス情報取得
			for (int i = 0; i < g_nIndexCount; i++)
			{
				fwscanf_s(pFile, L"%s", FileData, 256);
				if (swscanf_s(FileData, L"3;%d,%d,%d;,",
					&g_pIndexNormal[i * 3],
					&g_pIndexNormal[i * 3 + 1],
					&g_pIndexNormal[i * 3 + 2])
					== 0)
				{
					// 四角形があったので三角形に変換
					int index1, index2, index3, index4;
					swscanf_s(FileData, L"4;%d,%d,%d,%d;,",
						&index1, &index2, &index3, &index4);

					g_pIndexNormal[i * 3]		    = index1;
					g_pIndexNormal[i * 3 + 1]		= index2;
					g_pIndexNormal[i * 3 + 2]		= index3;

					g_pIndexNormal[(i + 1) * 3]		= index3;
					g_pIndexNormal[(i + 1) * 3 + 1] = index4;
					g_pIndexNormal[(i + 1) * 3 + 2] = index1;

					i++;
				}
			}
		}
		// uv読み込み
		else if (wcscmp(FileData, L"MeshTextureCoords") == 0)
		{
			fgetws(FileData, 256, pFile);
			fwscanf_s(pFile, L"%s", FileData, 256);

			if (FileData[0] == L'<') continue; //templateは省く

			swscanf_s(FileData, L"%d;", &g_nTexCount);

			// uvの数だけuv情報を入れる箱を作る
			g_pTex = new float[g_nTexCount * 2];

			fgetws(FileData, 256, pFile);
			// uv情報取得
			for (int i = 0; i < g_nTexCount; i++)
			{
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"%f; %f;,",
					&g_pTex[i * 2 + 0], &g_pTex[i * 2 + 1]);
			}

			fwscanf_s(pFile, L"%s", FileData, 256);
			// インデックスの数だけuvを入れる箱を作る
			g_pIndexTex = new int[g_nIndexCount * 3];

			fgetws(FileData, 256, pFile);
			// uvインデックス作成
			// uvインデックスは頂点インデックスと同じにする
			memcpy_s(g_pIndexTex, sizeof(int) * g_nIndexCount * 3,
				g_pIndexVertex, sizeof(int) * g_nIndexCount * 3);
		}
		// マテリアル情報取得
		else if (wcscmp(FileData, L"MeshMaterialList") == 0)
		{
			fgetws(FileData, 256, pFile);
			fwscanf_s(pFile, L"%s", FileData, 256);

			if (FileData[0] == L'<') continue; //templateは省く

			swscanf_s(FileData, L"%d;", &g_nMaterialCount);
			fwscanf_s(pFile, L"%s", FileData, 256);
			swscanf_s(FileData, L"%d;", &g_nSurfaceCount);

			g_pMaterialNumber = new int[g_nTriangleSurfaceCount];

			for (int i = 0; i < g_nSurfaceCount; i++)
			{
				fwscanf_s(pFile, L"%s", FileData, 256);
				swscanf_s(FileData, L"%d", &g_pMaterialNumber[i]);
			}
			fwscanf_s(pFile, L"%s", FileData, 256);

			g_pMaterial = new Material[g_nMaterialCount];

			for (int i = 0; i < g_nMaterialCount; i++)
			{
				fgetws(FileData, 256, pFile);
				// 拡散光
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"%f; %f; %f; %f;;",
					&(g_pMaterial[i].vDiffuseColor[0]),
					&(g_pMaterial[i].vDiffuseColor[1]),
					&(g_pMaterial[i].vDiffuseColor[2]),
					&(g_pMaterial[i].vDiffuseColor[3]));

				// 鏡面反射光の強さ
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"%f;",
					&(g_pMaterial[i].fSpecularPower));

				// 鏡面反射
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"%f; %f; %f;;",
					&(g_pMaterial[i].vSpecularColor[0]),
					&(g_pMaterial[i].vSpecularColor[1]),
					&(g_pMaterial[i].vSpecularColor[2]));

				// 発光
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"%f; %f; %f;;",
					&(g_pMaterial[i].vEmission[0]),
					&(g_pMaterial[i].vEmission[1]),
					&(g_pMaterial[i].vEmission[2]));

				// テクスチャ名
				wchar_t tempTextureName[256];
				fwscanf_s(pFile, L"%s", FileData, 256);
				fwscanf_s(pFile, L"%s", FileData, 256);
				if (swscanf_s(FileData, L"{%s;}", tempTextureName, 256) == 0)
				{
					// テクスチャがない
					g_pMaterial[i].sTextureName[0] = L'\0';
				}
				else
				{
					// blender以外で作成したXファイルの場合
					if (wcscmp(FileData, L"{") == 0)
					{
						fwscanf_s(pFile, L"%s", FileData, 256);
						swscanf_s(FileData, L"%s", tempTextureName, 256);

						// テクスチャ名を取り出す-------------------------------
						// 文字列の長さを取得
						int nLenght = 0;
						for (int j = 0; j < 256; j++)
						{
							if (tempTextureName[j] == L'\0') break;
							nLenght++;
						}

						// 「"」を「\0」にする 例( {"tex.jpg";} → {"tex.jpg\0 )
						for (int j = nLenght - 1; j >= 0; j--)
						{
							if (tempTextureName[j] == L'"')
							{
								tempTextureName[j] = L'\0';
								break;
							}
						}

						// 「"」から左の部分を取り除く   例( {"tex.jpg\0        → tex.jpg\0 )
						// 「\\」から左の部分を取り除く  例( {"Asset\\tex.jpg\0 → tex.jpg\0 )
						for (int j = nLenght - 1; j >= 0; j--)
						{
							if (tempTextureName[j] == L'"' || 
								tempTextureName[j] == L'\\')
							{
								memcpy_s(g_pMaterial[i].sTextureName, 256, &tempTextureName[j + 1], 256);
								break;
							}
						}

						// -----------------------------------------------------
						fgetws(FileData, 256, pFile);
						fgetws(FileData, 256, pFile);
						fgetws(FileData, 256, pFile);
						fgetws(FileData, 256, pFile);
					}
					// blenderで作成したXファイルの場合
					else
					{
						// テクスチャがある
						memcpy_s(g_pMaterial[i].sTextureName, 256, &tempTextureName[1], 256);
						for (int j = 0; j < 256; j++)
						{
							if (g_pMaterial[i].sTextureName[j] == L'"')
							{
								g_pMaterial[i].sTextureName[j] = L'\0';
								break;
							}
						}
						fgetws(FileData, 256, pFile);
						fgetws(FileData, 256, pFile);
					}
				}
			}
		}
		// ボーン情報取得
		else if (wcscmp(FileData, L"XSkinMeshHeader") == 0)
		{
			fgetws(FileData, 256, pFile);
			fwscanf_s(pFile, L"%s", FileData, 256);

			if (FileData[0] == L'<') continue; //templateは省く

			fwscanf_s(pFile, L"%s", FileData, 256);
			fwscanf_s(pFile, L"%s", FileData, 256);
			swscanf_s(FileData, L"%d;", &g_nBoneCount);

			// データを入れる場所を作る
			g_ppBoneName = new wchar_t*[g_nBoneCount];
			g_ppSkinWeightsIndex = new int*[g_nBoneCount];
			g_ppSkinWeights = new float*[g_nBoneCount];
			g_pSkinWeightsCount = new int[g_nBoneCount];
			g_pBoneMatrix4x4 = new Matrix4x4[g_nBoneCount];

			fwscanf_s(pFile, L"%s", FileData, 256);

			for (int i = 0; i < g_nBoneCount; i++)
			{
				fwscanf_s(pFile, L"%s", FileData, 256);
				fwscanf_s(pFile, L"%s", FileData, 256);
				
				// ボーン名取得
				fwscanf_s(pFile, L"%s", FileData, 256);
				g_ppBoneName[i] = new wchar_t[256];
				swscanf_s(FileData, L"%s;", g_ppBoneName[i], 256);
				// "ボーン名;" から ボーン名に文字列情報を変える
				g_ppBoneName[i] = &g_ppBoneName[i][1];
				int len = wcslen(g_ppBoneName[i]);
				g_ppBoneName[i][len - 2] = L'\0';

				// ウェイトの数取得
				fwscanf_s(pFile, L"%s", FileData, 256);
				swscanf_s(FileData, L"%d;", &g_pSkinWeightsCount[i]);

				// ウェイト値を取得
				g_ppSkinWeightsIndex[i] = new int[g_pSkinWeightsCount[i]];
				g_ppSkinWeights[i] = new float[g_nVertexCount];
				memset(g_ppSkinWeights[i], 0x00, sizeof(float) * g_nVertexCount);

				// ボーンインデックス取得
				for (int j = 0; j < g_pSkinWeightsCount[i]; j++)
				{
					fwscanf_s(pFile, L"%s", FileData, 256);
					swscanf_s(FileData, L"%d,", &g_ppSkinWeightsIndex[i][j]);
				}

				// ボーンウェイト値取得
				for (int j = 0; j < g_pSkinWeightsCount[i]; j++)
				{
					fwscanf_s(pFile, L"%s", FileData, 256);
					swscanf_s(FileData, L"%f,", &g_ppSkinWeights[i][g_ppSkinWeightsIndex[i][j]]);
				}

				fgetws(FileData, 256, pFile);

				// blenderで作成したXファイルの場合
				if (g_isRoot == true)
				{
					// ボーンオフセット行列取得
					for (int j = 0; j < 4; j++)
					{
						fgetws(FileData, 256, pFile);
						swscanf_s(FileData, L"%f,%f,%f,%f",
							(&g_pBoneMatrix4x4[i]._11) + j * 4,
							(&g_pBoneMatrix4x4[i]._12) + j * 4,
							(&g_pBoneMatrix4x4[i]._13) + j * 4,
							(&g_pBoneMatrix4x4[i]._14) + j * 4);
					}
				}

				fgetws(FileData, 256, pFile);
			}
		}
		// アニメーションキー取得
		else if (wcscmp(FileData, L"AnimationSet") == 0)
		{
			fgetws(FileData, 256, pFile);
			if (fgetws(FileData, 256, pFile) == NULL) continue;
			fgetws(FileData, 256, pFile);

			g_pAnimationKey = new AnimationKey[g_nBoneCount];

			int nBoneCount = 0;

			while (1)
			{
				while (1)
				{
					if (wcscmp(FileData, L"Animation") == 0) break;
					fwscanf_s(pFile, L"%s", FileData, 256);
				}

				fgetws(FileData, 256, pFile);
				fgetws(FileData, 256, pFile);

				for (int i = 0; i < g_nBoneCount; i++)
				{
					// 入れ替え
					wchar_t* psframe;
					wchar_t* ctx;
					psframe = wcstok_s(FileData, L" {}", &ctx);

					if (wcscmp(g_ppBoneName[i], psframe) == 0)
					{
						fgetws(FileData, 256, pFile);
						fgetws(FileData, 256, pFile);
						fgetws(FileData, 256, pFile);

						swscanf_s(FileData, L"%d;", &g_pAnimationKey[i].nEndFrame);
						// 変換行列をアニメーションフレーム分だけ入れる入れ物を作る
						g_pAnimationKey[i].pWorld = new World[g_pAnimationKey[i].nEndFrame];

						// 回転情報取得
						for (int j = 0; j < g_pAnimationKey[i].nEndFrame; j++)
						{
							int none1, none2;
							fgetws(FileData, 256, pFile);
							swscanf_s(FileData, L"%d;%d;%f,%f,%f,%f;;",
								&none1, &none2,
								&g_pAnimationKey[i].pWorld[j].qRotation[3],
								&g_pAnimationKey[i].pWorld[j].qRotation[0],
								&g_pAnimationKey[i].pWorld[j].qRotation[1],
								&g_pAnimationKey[i].pWorld[j].qRotation[2]);

							// z値で反転させる
							D3DXQUATERNION qRotation;
							qRotation.x = g_pAnimationKey[i].pWorld[j].qRotation[0];
							qRotation.y = g_pAnimationKey[i].pWorld[j].qRotation[1];
							qRotation.z = -g_pAnimationKey[i].pWorld[j].qRotation[2];
							qRotation.w = g_pAnimationKey[i].pWorld[j].qRotation[3];

							// クォータニオンから回転行列に変換
							D3DXMatrixRotationQuaternion(&g_pAnimationKey[i].pWorld[j].matRotation, &qRotation);
						}

						fgetws(FileData, 256, pFile);
						fgetws(FileData, 256, pFile);
						fgetws(FileData, 256, pFile);
						fgetws(FileData, 256, pFile);

						// 拡大縮小情報
						for (int j = 0; j < g_pAnimationKey[i].nEndFrame; j++)
						{
							int none1, none2;
							fgetws(FileData, 256, pFile);
							swscanf_s(FileData, L"%d;%d;%f,%f,%f;;",
								&none1, &none2,
								&g_pAnimationKey[i].pWorld[j].vScale[0],
								&g_pAnimationKey[i].pWorld[j].vScale[1],
								&g_pAnimationKey[i].pWorld[j].vScale[2]);
						}

						fgetws(FileData, 256, pFile);
						fgetws(FileData, 256, pFile);
						fgetws(FileData, 256, pFile);
						fgetws(FileData, 256, pFile);

						// 平行移動情報
						for (int j = 0; j < g_pAnimationKey[i].nEndFrame; j++)
						{
							// 必要のないデータを入れる入れ物
							int none1, none2;
							float none3;

							fgetws(FileData, 256, pFile);
							swscanf_s(FileData, L"%d;%d;%f,%f,%f,%f;;",
								&none1, &none2,
								&g_pAnimationKey[i].pWorld[j].vPosition[0],
								&g_pAnimationKey[i].pWorld[j].vPosition[1],
								&g_pAnimationKey[i].pWorld[j].vPosition[2],
								&none3);
						}

						// ボーンオフセット行列を正しくなるように変換
						g_pBoneMatrix4x4[i].Multiply(g_matArmature4x4);
						float temp;
						temp = g_pBoneMatrix4x4[i]._42;
						g_pBoneMatrix4x4[i]._42 = g_pBoneMatrix4x4[i]._43;
						g_pBoneMatrix4x4[i]._43 = temp;

						// アニメーションの平行移動にボーンオフセット行列の平行移動値が入っているので、取り除く----------------
						float tempPosition[3];
						tempPosition[0] = g_pAnimationKey[i].pWorld[0].vPosition[0];
						tempPosition[1] = g_pAnimationKey[i].pWorld[0].vPosition[1];
						tempPosition[2] = g_pAnimationKey[i].pWorld[0].vPosition[2];

						for (int j = 0; j < g_pAnimationKey[i].nEndFrame; j++)
						{
							g_pAnimationKey[i].pWorld[j].vPosition[0] -= tempPosition[0];
							g_pAnimationKey[i].pWorld[j].vPosition[1] -= tempPosition[1];
							g_pAnimationKey[i].pWorld[j].vPosition[2] -= tempPosition[2];
						// ---------------------------------------------------------------------------------------------------

							Vector3D vPosition = Vector3D(g_pAnimationKey[i].pWorld[j].vPosition);
							vPosition = g_matArmature4x4.TransformPointVector3D(vPosition);

							vPosition = g_RootMatrix4x4.TransformPointVector3D(vPosition);
							g_pAnimationKey[i].pWorld[j].vPosition[0] = vPosition.x;
							g_pAnimationKey[i].pWorld[j].vPosition[1] = vPosition.y;
							g_pAnimationKey[i].pWorld[j].vPosition[2] = vPosition.z;
						}
						

						nBoneCount++;
						break;
					}
				}
				
				if (g_nBoneCount == nBoneCount) break;
			}
		}
	}

	rewind(pFile);

	// ボーンの親子関係情報を入れる処理
	// blenderで作成したXファイルなら
	if(g_nBoneCount > 0 && g_isRoot == true)
	{
		g_pBoneDepth = new int[g_nBoneCount];
		memset(g_pBoneDepth, 0x00, sizeof(int) * g_nBoneCount);
		g_pBoneChild = new int[g_nBoneCount];
		memset(g_pBoneChild, 0x00, sizeof(int) * g_nBoneCount);
		g_pBoneInvMatrix4x4 = new D3DXMATRIX[g_nBoneCount];

		bool FindArmature = false; // ファイル内にArmatureをみつけたかどうかのフラグ
		int nBoneCount = 0;
		int nBoneDepthCount = 0;

		// ここでボーンの親子関係を取得する
		while(fgetws(FileData, 256, pFile) != NULL)
		{	
			wchar_t* psframe;
			wchar_t* ctx;
			psframe = wcstok_s(FileData, L" ", &ctx);

			if (wcscmp(psframe, L"Frame") == 0)
			{
				psframe = wcstok_s(NULL, L" ", &ctx);

				for (int iBC = 0; iBC < g_nBoneCount; iBC++)
				{
					if (wcscmp(psframe, g_ppBoneName[iBC]) == 0)
					{
						int nBoneDepthCount = 0; // ボーンの親子関係の深さ

						for (int i = 2; i < 256; i++)
						{
							if (FileData[i * 2] == L'F') break;

							nBoneDepthCount++;
						}
						g_pBoneDepth[nBoneCount] = nBoneDepthCount;

						// ボーン名を入れる
						wchar_t sBoneName[256];
						wcscpy_s(sBoneName, 256, psframe);

						int nBoneNumber;
						for (nBoneNumber = 0; nBoneNumber < g_nBoneCount; nBoneNumber++)
						{
							if (wcscmp(g_ppBoneName[nBoneNumber], sBoneName) == 0) break;
						}

						if (nBoneCount != nBoneNumber)
						{
							// ボーン情報を順番に入れ替え----------------------------------------------
							wchar_t NameTemp[256];
							int nWeightTemp;
							int *pWeightIndexTemp;
							float *pWeightTemp;

							Matrix4x4 matTemp;
							wcscpy_s(NameTemp, 256, g_ppBoneName[nBoneCount]);
							wcscpy_s(g_ppBoneName[nBoneCount], 256, g_ppBoneName[nBoneNumber]);
							wcscpy_s(g_ppBoneName[nBoneNumber], 256, NameTemp);

							nWeightTemp = g_pSkinWeightsCount[nBoneCount];
							g_pSkinWeightsCount[nBoneCount] = g_pSkinWeightsCount[nBoneNumber];
							g_pSkinWeightsCount[nBoneNumber] = nWeightTemp;

							pWeightIndexTemp = g_ppSkinWeightsIndex[nBoneCount];
							g_ppSkinWeightsIndex[nBoneCount] = g_ppSkinWeightsIndex[nBoneNumber];
							g_ppSkinWeightsIndex[nBoneNumber] = pWeightIndexTemp;

							pWeightTemp = g_ppSkinWeights[nBoneCount];
							g_ppSkinWeights[nBoneCount] = g_ppSkinWeights[nBoneNumber];
							g_ppSkinWeights[nBoneNumber] = pWeightTemp;

							matTemp = g_pBoneMatrix4x4[nBoneCount];
							g_pBoneMatrix4x4[nBoneCount] = g_pBoneMatrix4x4[nBoneNumber];
							g_pBoneMatrix4x4[nBoneNumber] = matTemp;

							AnimationKey tempAnimation;
							tempAnimation = g_pAnimationKey[nBoneCount];
							g_pAnimationKey[nBoneCount] = g_pAnimationKey[nBoneNumber];
							g_pAnimationKey[nBoneNumber] = tempAnimation;
							// -------------------------------------------------------------------------
						}

						fgetws(FileData, 256, pFile);

						D3DXMATRIX TempInvBoneMatrix;
						// 逆ボーンオフセット行列情報取得(平行移動情報を直す用)
						for (int i = 0; i < 4; i++)
						{
							fgetws(FileData, 256, pFile);
							swscanf_s(FileData, L"%f,%f,%f,%f",
								(&g_pBoneInvMatrix4x4[nBoneCount]._11) + i * 4,
								(&g_pBoneInvMatrix4x4[nBoneCount]._12) + i * 4,
								(&g_pBoneInvMatrix4x4[nBoneCount]._13) + i * 4,
								(&g_pBoneInvMatrix4x4[nBoneCount]._14) + i * 4);
						}

						D3DXMATRIX BoneMatrix = D3DXMATRIX((float*)&g_pBoneMatrix4x4[nBoneCount]);
						// ここで逆ボーン行列作成
						if (g_pBoneDepth[nBoneCount] == 0)
						{
							// 親ボーンならボーン行列の逆行列を求める
							D3DXMatrixInverse(&g_pBoneInvMatrix4x4[nBoneCount], NULL, &BoneMatrix);
						}
						else
						{
							int iBoneCount = nBoneCount;
							while (1)
							{
								iBoneCount--;
								if (g_pBoneDepth[iBoneCount] == g_pBoneDepth[nBoneCount] - 1)
								{
									// 子ボーンの数を増やす 
									g_pBoneChild[iBoneCount]++;
									break;
								}
							}
							D3DXMATRIX BoneInvMatrix = D3DXMATRIX((float*)&g_pBoneMatrix4x4[iBoneCount]);
							D3DXMatrixInverse(&BoneInvMatrix, NULL, &BoneInvMatrix);
							D3DXMatrixMultiply(&BoneInvMatrix,
								&BoneMatrix, &BoneInvMatrix);
							D3DXMatrixInverse(&g_pBoneInvMatrix4x4[nBoneCount], NULL, &BoneInvMatrix);
						}

						nBoneCount++;
						if (nBoneCount == g_nBoneCount) break;
					}
				}

				
			}
		}
	}

	// MeshMaterialListの面の数が合わない場合(応急処置です、後で修正します)
	if (g_nTriangleSurfaceCount == g_nSurfaceCount * 2)
	{
		int* pTemp = new int[g_nTriangleSurfaceCount];

		for (int i = 0; i < g_nTriangleSurfaceCount; i++)
		{
			pTemp[i] = g_pMaterialNumber[i / 2];
		}

		memcpy_s(g_pMaterialNumber, sizeof(int) * g_nTriangleSurfaceCount,
			pTemp, sizeof(int) * g_nTriangleSurfaceCount);

		delete[] pTemp;
	}
	// 四角形と三角形でのマテリアルのずれを修正
	else if (g_nInitIndex == g_nSurfaceCount)
	{
		// マテリアル番号を入れる箱を一時的に作成(あとでg_pTriangleSurfaceNumberに入れ替える)
		int* pTemp = new int[g_nTriangleSurfaceCount];

		for (int i = 0; i < g_nTriangleSurfaceCount; i++)
		{
			pTemp[i] = g_pMaterialNumber[g_pTriangleSurfaceNumber[i]];
		}

		memcpy_s(g_pMaterialNumber, sizeof(int) * g_nTriangleSurfaceCount,
			pTemp, sizeof(int) * g_nTriangleSurfaceCount);

		delete[] pTemp;
	}

	// マテリアル情報がある場合、xnファイルで使える形に変換
	// マテリアル番号別に面の数と面の情報を入れる
	if (g_nMaterialCount > 0)
	{
		g_ppIndexMaterial = new int*[g_nMaterialCount];
		g_pMaterialSurfaceNum = new int[g_nMaterialCount];

		// マテリアルを反映する面の数を調べる
		for (int matenumi = 0; matenumi < g_nMaterialCount; matenumi++)
		{
			// 変数初期化
			g_pMaterialSurfaceNum[matenumi] = 0;

			for (int j = 0; j < g_nTriangleSurfaceCount; j++)
			{
				// マテリアル番号が一致したら
				if (matenumi == g_pMaterialNumber[j])
				{
					// 反映させる面の数を増やす
					g_pMaterialSurfaceNum[matenumi]++;
				}
			}
		}

		for (int matenumi = 0; matenumi < g_nMaterialCount; matenumi++)
		{
			// マテリアルを反映させる面の数だけマテリアルインデックスを入れる箱を作る
			g_ppIndexMaterial[matenumi] = new int[g_pMaterialSurfaceNum[matenumi]];

			// マテリアルインデックスを入れた数
			int count = 0;

			for (int j = 0; j < g_nTriangleSurfaceCount; j++)
			{
				// マテリアル番号が一致したら
				if (matenumi == g_pMaterialNumber[j])
				{
					// マテリアルインデックスを入れる
					g_ppIndexMaterial[matenumi][count] = j;

					count++;

					// マテリアルデータを最後まで入れたら終了
					if (count >= g_pMaterialSurfaceNum[matenumi]) break;
				}
			}
		}
	}

	// OBB情報を入れる
	obb_position = (object_scale[0] / 2.f) + (object_scale[1] / 2.f);
	obb_scale = object_scale[0] - obb_position;

	fclose(pFile);

	return true;
}

// モデルnxファイルを作成する
bool CreateNXFile(wchar_t* sFileName)
{
	FILE* pFile; // 書き込むファイル

	// 書き込みファイルを開く
	if (_wfopen_s(&pFile, sFileName, L"w") != 0)
	{
		// ファイルが作成できなかった場合
		wchar_t ErrorString[256];
		swprintf_s(ErrorString, 256, L"%sが作成できませんでした。", sFileName);
		wprintf_s(ErrorString);
		return false;
	}

	// 頂点情報書き込み-----------------------------
	// 頂点数書き出し
	fwprintf_s(pFile, L"vnum %d\n", g_nIndexCount * 3);

	// 頂点情報書き出し
	for (int i = 0; i < g_nIndexCount * 3; i++)
	{
		fwprintf_s(pFile, L"v %f,%f,%f\n",
			g_pVertex[g_pIndexVertex[i] * 3 + 0],
			g_pVertex[g_pIndexVertex[i] * 3 + 1],
			g_pVertex[g_pIndexVertex[i] * 3 + 2]);
	}
	fwprintf_s(pFile, L"\n");
	// ---------------------------------------------
	
	// 頂点カラー情報書き込み-----------------------
	// 頂点カラー数書き出し
	if (g_nVertexColorCount == 0)
	{
		//頂点カラーなし
	}
	else
	{
		fwprintf_s(pFile, L"cnum %d\n", g_nIndexCount * 3);

		// 頂点カラー情報書き出し
		for (int i = 0; i < g_nIndexCount * 3; i++)
		{
			fwprintf_s(pFile, L"c %f,%f,%f,%f\n",
				g_pVertexColor[g_pIndexVertexColor[i] * 3 + 0],
				g_pVertexColor[g_pIndexVertexColor[i] * 3 + 1],
				g_pVertexColor[g_pIndexVertexColor[i] * 3 + 2],
				g_pVertexColor[g_pIndexVertexColor[i] * 3 + 3]);
		}
	}
	fwprintf_s(pFile, L"\n");
	// ---------------------------------------------
	
	// 法線情報書き込み-----------------------------
	// 法線数書き出し
	fwprintf_s(pFile, L"nnum %d\n", g_nIndexCount * 3);

	// 法線情報書き出し
	for (int i = 0; i < g_nIndexCount * 3; i++)
	{
		fwprintf_s(pFile, L"n %f,%f,%f\n",
			g_pNormal[g_pIndexNormal[i] * 3 + 0],
			g_pNormal[g_pIndexNormal[i] * 3 + 1],
			g_pNormal[g_pIndexNormal[i] * 3 + 2]);
	}
	fwprintf_s(pFile, L"\n");
	// ---------------------------------------------

	// uv情報書き込み-------------------------------
	// uv数書き出し
	if (g_nTexCount == 0)
	{
		// uvなし
	}
	else
	{
		fwprintf_s(pFile, L"uvnum %d\n", g_nIndexCount * 3);

		// uv情報書き出し
		for (int i = 0; i < g_nIndexCount * 3; i++)
		{
			fwprintf_s(pFile, L"uv %f,%f\n",
				g_pTex[g_pIndexTex[i] * 2 + 0],
				g_pTex[g_pIndexTex[i] * 2 + 1]);
		}
	}
	fwprintf_s(pFile, L"\n");
	// ---------------------------------------------
	
	fwprintf_s(pFile, L"matenum %d\n", g_nMaterialCount);

	for (int i = 0; i < g_nMaterialCount; i++)
	{
		// テクスチャ情報
		fwprintf_s(pFile, L"tex %s\n", g_pMaterial[i].sTextureName);

		// マテリアル情報
		// 拡散光
		fwprintf_s(pFile, L"dif %f,%f,%f,%f\n", 
			g_pMaterial[i].vDiffuseColor[0],
			g_pMaterial[i].vDiffuseColor[1],
			g_pMaterial[i].vDiffuseColor[2],
			g_pMaterial[i].vDiffuseColor[3]);

		// 鏡面反射光の強さ
		fwprintf_s(pFile, L"spep %f\n",
			g_pMaterial[i].fSpecularPower);

		// 鏡面反射
		fwprintf_s(pFile, L"spe %f,%f,%f\n",
			g_pMaterial[i].vSpecularColor[0],
			g_pMaterial[i].vSpecularColor[1],
			g_pMaterial[i].vSpecularColor[2]);
		
		// 発光
		fwprintf_s(pFile, L"emi %f,%f,%f\n",
			g_pMaterial[i].vEmission[0],
			g_pMaterial[i].vEmission[1],
			g_pMaterial[i].vEmission[2]);
	}

	fwprintf_s(pFile, L"\n");

	// 三角形の面の数だけマテリアルインデックスを書き出す
	for (int i = 0; i < g_nMaterialCount; i++)
	{
		fwprintf_s(pFile, L"matesurfacenum %d\n", g_pMaterialSurfaceNum[i]);
	
		for (int j = 0; j < g_pMaterialSurfaceNum[i]; j++)
		{
			fwprintf_s(pFile, L"mate %d\n",
				g_ppIndexMaterial[i][j]);
		}
	}

	fwprintf_s(pFile, L"\n");

	if (g_nBoneCount > 0)
	{
		fwprintf_s(pFile, L"bonenum %d\n", g_nBoneCount);
	}

	// ボーン情報を書き出す--------------------------------------------------
	for (int i = 0; i < g_nBoneCount; i++)
	{
		// 親子関係
		fwprintf_s(pFile, L"depth %d\n",
			g_pBoneDepth[i]);
	}

	fwprintf_s(pFile, L"\n");

	for (int i = 0; i < g_nBoneCount; i++)
	{
		// 子ボーンの数
		fwprintf_s(pFile, L"child %d\n",
			g_pBoneChild[i]);
	}

	fwprintf_s(pFile, L"\n");

	// ボーンウェイト値を書き出す
	for (int i = 0; i < g_nBoneCount; i++)
	{
		// ウェイト値の数
		fwprintf_s(pFile, L"weightsnum %d\n",
			g_nIndexCount * 3);
		
		// ウェイト値を出力
		for(int j = 0; j < g_nIndexCount * 3; j++)
		{
			fwprintf_s(pFile, L"weight %d : %f\n", j, g_ppSkinWeights[i][g_pIndexVertex[j]]);
		}

		fwprintf_s(pFile, L"matoffset %f,%f,%f,%f\n",
			g_pBoneMatrix4x4[i]._11, g_pBoneMatrix4x4[i]._12,
			g_pBoneMatrix4x4[i]._13, g_pBoneMatrix4x4[i]._14);

		fwprintf_s(pFile, L"matoffset %f,%f,%f,%f\n",
			g_pBoneMatrix4x4[i]._21, g_pBoneMatrix4x4[i]._22,
			g_pBoneMatrix4x4[i]._23, g_pBoneMatrix4x4[i]._24);

		fwprintf_s(pFile, L"matoffset %f,%f,%f,%f\n",
			g_pBoneMatrix4x4[i]._31, g_pBoneMatrix4x4[i]._32,
			g_pBoneMatrix4x4[i]._33, g_pBoneMatrix4x4[i]._34);

		fwprintf_s(pFile, L"matoffset %f,%f,%f,%f\n",
			g_pBoneMatrix4x4[i]._41, g_pBoneMatrix4x4[i]._42,
			g_pBoneMatrix4x4[i]._43, g_pBoneMatrix4x4[i]._44);

		fwprintf_s(pFile, L"invmatoffset %f,%f,%f,%f\n",
			g_pBoneInvMatrix4x4[i]._11, g_pBoneInvMatrix4x4[i]._12,
			g_pBoneInvMatrix4x4[i]._13, g_pBoneInvMatrix4x4[i]._14);

		fwprintf_s(pFile, L"invmatoffset %f,%f,%f,%f\n",
			g_pBoneInvMatrix4x4[i]._21, g_pBoneInvMatrix4x4[i]._22,
			g_pBoneInvMatrix4x4[i]._23, g_pBoneInvMatrix4x4[i]._24);

		fwprintf_s(pFile, L"invmatoffset %f,%f,%f,%f\n",
			g_pBoneInvMatrix4x4[i]._31, g_pBoneInvMatrix4x4[i]._32,
			g_pBoneInvMatrix4x4[i]._33, g_pBoneInvMatrix4x4[i]._34);

		fwprintf_s(pFile, L"invmatoffset %f,%f,%f,%f\n",
			g_pBoneInvMatrix4x4[i]._41, g_pBoneInvMatrix4x4[i]._42,
			g_pBoneInvMatrix4x4[i]._43, g_pBoneInvMatrix4x4[i]._44);

		fwprintf_s(pFile, L"\n");
	}
	// ----------------------------------------------------------------------

	// アニメーション情報書き込み(別のファイルを作成して、そこに書き込む)
	if (g_nBoneCount > 0)
	{
		wchar_t* sAniFileName;

		int nFileNameCharNum = wcslen(sFileName);
		sAniFileName = new wchar_t[nFileNameCharNum + 4];
		sAniFileName[0] = L'\0';
		
		wcsncat_s(sAniFileName, nFileNameCharNum + 4, sFileName, nFileNameCharNum - 3);
		wcscat_s(sAniFileName, nFileNameCharNum + 4, L"Ani.nx");

		CreateNXAnimationFile(sAniFileName);

		delete[] sAniFileName;
	}

	// OBB情報を書き出す
	fwprintf_s(pFile, L"obb\n");

	fwprintf_s(pFile, L"obb_scale %f,%f,%f\n",
		obb_scale.x, obb_scale.y, obb_scale.z);
	
	fwprintf_s(pFile, L"obb_pos %f,%f,%f\n",
		obb_position.x, obb_position.y, obb_position.z);
	
	fclose(pFile);

	return true;
}

// nxのアニメーションファイル
bool CreateNXAnimationFile(wchar_t* sFileName)
{
	FILE* pFile; // 書き込むファイル

	// 書き込みファイルを開く
	if (_wfopen_s(&pFile, sFileName, L"w") != 0)
	{
		// ファイルが作成できなかった場合
		wchar_t ErrorString[256];
		swprintf_s(ErrorString, 256, L"%sが作成できませんでした。", sFileName);
		wprintf_s(ErrorString);
		return false;
	}

	// アニメーションフレーム最大数を書き出す
	fwprintf_s(pFile, L"framenum %d\n",
		g_pAnimationKey[0].nEndFrame);

	// アニメーション情報を書き出す
	for (int i = 0; i < g_nBoneCount; i++)
	{
		for (int j = 0; j < g_pAnimationKey[i].nEndFrame; j++)
		{
			fwprintf_s(pFile, L"frame %d\n", j);

			// 回転行列出力
			fwprintf_s(pFile, L"rota %f,%f,%f,%f\n",
				g_pAnimationKey[i].pWorld[j].matRotation._11,
				g_pAnimationKey[i].pWorld[j].matRotation._12,
				g_pAnimationKey[i].pWorld[j].matRotation._13,
				g_pAnimationKey[i].pWorld[j].matRotation._14);

			fwprintf_s(pFile, L"rota %f,%f,%f,%f\n",
				g_pAnimationKey[i].pWorld[j].matRotation._21,
				g_pAnimationKey[i].pWorld[j].matRotation._22,
				g_pAnimationKey[i].pWorld[j].matRotation._23,
				g_pAnimationKey[i].pWorld[j].matRotation._24);

			fwprintf_s(pFile, L"rota %f,%f,%f,%f\n",
				g_pAnimationKey[i].pWorld[j].matRotation._31,
				g_pAnimationKey[i].pWorld[j].matRotation._32,
				g_pAnimationKey[i].pWorld[j].matRotation._33,
				g_pAnimationKey[i].pWorld[j].matRotation._34);

			fwprintf_s(pFile, L"rota %f,%f,%f,%f\n",
				g_pAnimationKey[i].pWorld[j].matRotation._41,
				g_pAnimationKey[i].pWorld[j].matRotation._42,
				g_pAnimationKey[i].pWorld[j].matRotation._43,
				g_pAnimationKey[i].pWorld[j].matRotation._44);

			// 拡大縮小情報
			fwprintf_s(pFile, L"scale %f,%f,%f\n",
				g_pAnimationKey[i].pWorld[j].vScale[0],
				g_pAnimationKey[i].pWorld[j].vScale[1],
				g_pAnimationKey[i].pWorld[j].vScale[2]);

			// 平行移動情報
			fwprintf_s(pFile, L"trans %f,%f,%f\n",
				g_pAnimationKey[i].pWorld[j].vPosition[0],
				g_pAnimationKey[i].pWorld[j].vPosition[1],
				g_pAnimationKey[i].pWorld[j].vPosition[2]);
		}

		fwprintf_s(pFile, L"\n");
	}

	fclose(pFile);

	return true;
}

// メモリを解放する
void MemoryRelease()
{
	DELETE_ARRAY(g_pVertex);
	DELETE_ARRAY(g_pTex);
	DELETE_ARRAY(g_pNormal);
	DELETE_ARRAY(g_pVertexColor);
	DELETE_ARRAY(g_pMaterial);
	DELETE_ARRAY(g_pIndexVertex);
	DELETE_ARRAY(g_pIndexTex);
	DELETE_ARRAY(g_pIndexNormal);
	DELETE_ARRAY(g_pIndexVertexColor);
	DELETE_ARRAY(g_pSkinWeightsCount);
	DELETE_ARRAY(g_ppSkinWeights);
	DELETE_ARRAY(g_ppSkinWeightsIndex);
	DELETE_ARRAY(g_pBoneMatrix4x4);
	DELETE_ARRAY(g_pBoneDepth);
	DELETE_ARRAY(g_pBoneChild);

	for (int i = 0; i < g_nMaterialCount; i++)
	{
		DELETE_ARRAY(g_ppIndexMaterial[i]);
	}
	DELETE_ARRAY(g_pMaterialSurfaceNum);
	DELETE_ARRAY(g_pTriangleSurfaceNumber);

	for (int i = 0; i < g_nBoneCount; i++)
	{
		DELETE_ARRAY(g_pAnimationKey[i].pWorld);
	}

	DELETE_ARRAY(g_pAnimationKey);
}