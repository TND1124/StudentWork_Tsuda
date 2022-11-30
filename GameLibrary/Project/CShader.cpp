#include "CShader.h"

#include "CDevice3D.h"
#include "CCamera.h"
#include "CLight.h"
#include "CFont.h"

#include "Window.h"

// シェーダーデータが入っているディレクトリをここで指定
// (ここで指定することでわざわざ全てのディレクトリを書かなくてもよくなる)
#define SHADER_DIRECTORY (L"Asset\\Shader\\")

using namespace std;

extern CCamera* g_pCamera;

CShader* CShader::m_pInstance = 0;

void CShader::Create()
{
	if (m_pInstance != NULL)
	{
		MessageBox(NULL, L"シェーダークラス生成失敗", L"エラー", MB_OK);
		return;
	}

	m_pInstance = new CShader;
}

CShader* CShader::GetInstance()
{
	if (m_pInstance == NULL)
	{
		MessageBox(NULL, L"シェーダークラスが生成されていません。", L"エラー", MB_OK);
		return NULL;
	}

	return m_pInstance;
}

void CShader::Destory()
{
	if (m_pInstance != NULL)
	{
		delete m_pInstance;
		m_pInstance = 0;
	}
}

bool CShader::Init()
{
	CDevice3D* pDevice3D = CDevice3D::GetInstance();
	ID3D10Device* pDevice = pDevice3D->GetDevice();

	// エフェクト作成
	CreateShader(L"Model3D.fx", ModelShader);
	CreateShader(L"Texture.fx", Texture2DShader);
	CreateShader(L"Shadow.fx", ShadowShader);

	//頂点インプットレイアウトを定義	
	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA,   0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA,	   0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D10_INPUT_PER_VERTEX_DATA,    0 },
		{ "WEIGTH", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "MATOFFSETID", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	//頂点インプットレイアウトを作成
	D3D10_PASS_DESC PassDesc;
	m_pTechnique[ModelShader]->GetPassByIndex(0)->GetDesc(&PassDesc);
	if (FAILED(pDevice->CreateInputLayout(layout, numElements, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &m_pVertexLayout[ModelShader])))
		return FALSE;

	D3D10_PASS_DESC PassDesc2;
	m_pTechnique[Texture2DShader]->GetPassByIndex(0)->GetDesc(&PassDesc2);
	if (FAILED(pDevice->CreateInputLayout(layout, numElements, PassDesc2.pIAInputSignature,
		PassDesc2.IAInputSignatureSize, &m_pVertexLayout[Texture2DShader])))
		return FALSE;

	D3D10_PASS_DESC PassDesc3;
	m_pTechnique[ShadowShader]->GetPassByIndex(0)->GetDesc(&PassDesc3);
	if (FAILED(pDevice->CreateInputLayout(layout, numElements, PassDesc3.pIAInputSignature,
		PassDesc3.IAInputSignatureSize, &m_pVertexLayout[ShadowShader])))
		return FALSE;

	return true;
}

void CShader::CreateShader(const wchar_t* ShaderFileName, ShaderNumber eShaderNumber)
{
	HRESULT hr = NULL;
	int a = 0;
	CDevice3D* pDevice3D = CDevice3D::GetInstance();

	// ディレクトリとファイル名を合わせる
	wstring wsFileName(SHADER_DIRECTORY);
	wsFileName += ShaderFileName;

	//エフェクトを作成
	hr = D3DX10CreateEffectFromFile(wsFileName.c_str(), NULL, NULL, "fx_4_0",
		D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, 0,
		pDevice3D->GetDevice(), NULL, NULL, &m_pEffect[eShaderNumber], NULL, NULL);

	if (FAILED(hr))
	{
		MessageBox(0, L"シェーダーファイルが見つかりません", L"エラー", MB_OK);
		return;
	}

	//テクニックを得る
	m_pTechnique[eShaderNumber] = m_pEffect[eShaderNumber]->GetTechniqueByName("SimpleTexture");
}

// 文字描画
void CShader::StrDraw(float PosX, float PosY, float FontSize, const wchar_t* Str, const D3DXVECTOR4& vColor)
{
	CFont* pFont = CFont::GetInstance();

	// 文字数を調べる
	int nFontLen = wcslen(Str);

	// 一文字ずつ描画
	for (int i = 0; i < nFontLen; i++)
	{
		TextureDraw(PosX + FontSize * i, PosY, FontSize, FontSize, 0.f, pFont->FontGraphicCreate(Str[i]), vColor, true);
	}
}

void CShader::StrDraw(float PosX, float PosY, float FontSize, const wchar_t* Str, float fDepth)
{
	CFont* pFont = CFont::GetInstance();

	// 文字数を調べる
	int nFontLen = wcslen(Str);

	// 一文字ずつ描画
	for (int i = 0; i < nFontLen; i++)
	{
		TextureDraw(PosX + FontSize * i, PosY, FontSize, FontSize, pFont->FontGraphicCreate(Str[i]), true, fDepth);
	}
}



// モデル描画
void CShader::ShaderModelDraw(CModel* pModel, D3DXMATRIX matWorld, bool TransFlag)
{
	CDevice3D* pDevice3D = CDevice3D::GetInstance();
	ID3D10Device* pDevice = pDevice3D->GetDevice();

	// 頂点インプットレイアウトをセット
	pDevice->IASetInputLayout(m_pVertexLayout[ModelShader]);

	// モデル情報がなければ、処理終了
	if (pModel == NULL) return;

	CLightManager* pLightManager = CLightManager::GetInstance();

	// ライト情報を配列に入れる
	int nLightKindArray[RIGHT_NUM_MAX];
	D3DXVECTOR3 vLightDirction[RIGHT_NUM_MAX];
	D3DXVECTOR4 vLightPoint[RIGHT_NUM_MAX];
	float fLightIntensity[RIGHT_NUM_MAX];

	for (int i = 0; i < RIGHT_NUM_MAX; i++)
	{
		CLight* Light = pLightManager->GetLight(i);
		nLightKindArray[i] = Light->GetLightKind();
		vLightDirction[i] = Light->GetLightDirection();
		vLightPoint[i] = Light->GetLightPoint();
		fLightIntensity[i] = Light->GetLightIntensity();
	}

	ID3D10EffectScalarVariable* pShaderLightKind;
	pShaderLightKind = m_pEffect[ModelShader]->GetVariableByName("g_nLightKind")->AsScalar();
	pShaderLightKind->SetIntArray(nLightKindArray, 0, RIGHT_NUM_MAX);

	ID3D10EffectVectorVariable* pShaderLightDirection;
	pShaderLightDirection = m_pEffect[ModelShader]->GetVariableByName("g_vLightDirction")->AsVector();
	pShaderLightDirection->SetFloatVectorArray((float*)(&vLightDirction), 0, RIGHT_NUM_MAX);

	ID3D10EffectVectorVariable* pShaderLightPosition;
	pShaderLightPosition = m_pEffect[ModelShader]->GetVariableByName("g_vLightPosition")->AsVector();
	pShaderLightPosition->SetFloatVectorArray((float*)(&vLightPoint), 0, RIGHT_NUM_MAX);

	ID3D10EffectScalarVariable* pShaderLightIntensity;
	pShaderLightIntensity = m_pEffect[ModelShader]->GetVariableByName("g_fLightIntensity")->AsScalar();
	pShaderLightIntensity->SetFloatArray(fLightIntensity, 0, RIGHT_NUM_MAX);

	D3DXMATRIX matAnimationBone[100];
	for (int i = 0; i < 100; i++)
	{
		D3DXMatrixIdentity(&matAnimationBone[i]);
	}

	// ボーンアニメーション情報取得
	for (int i = 0; i < pModel->GetBoneCount(); i++)
	{
		matAnimationBone[i] = pModel->GetBone(i)->m_matAnimation;
	}

	//視線ベクトルをエフェクト（シェーダー）に通知・適用
	ID3D10EffectVectorVariable* pEyePos;
	pEyePos = m_pEffect[ModelShader]->GetVariableByName("g_vEye")->AsVector();
	pEyePos->SetFloatVector((float*)g_pCamera->GetEye());

	// ワールド行列をシェーダーに渡す
	ID3D10EffectMatrixVariable* pShaderWorld;
	pShaderWorld = m_pEffect[ModelShader]->GetVariableByName("g_mW")->AsMatrix();
	pShaderWorld->SetMatrix((float*)&(matWorld));

	//ワールド・ビュー・プロジェクション行列をシェーダーに渡す
	ID3D10EffectMatrixVariable* pShaderWorldViewProjection;
	pShaderWorldViewProjection = m_pEffect[ModelShader]->GetVariableByName("g_mWVP")->AsMatrix();
	pShaderWorldViewProjection->SetMatrix((float*)&(matWorld * (*g_pCamera->GetMatView())*(*g_pCamera->GetMatProj())));

	// ボーン行列をセット
	ID3D10EffectMatrixVariable* pShaderBoneMatrix;
	pShaderBoneMatrix = m_pEffect[ModelShader]->GetVariableByName("g_MatBone")->AsMatrix();
	pShaderBoneMatrix->SetMatrixArray((float*)&(matAnimationBone), 0, 100);

	ID3D10EffectShaderResourceVariable* pShaderTexture;
	ID3D10EffectVectorVariable* pAmbient;
	ID3D10EffectVectorVariable* pDiffuse;
	ID3D10EffectVectorVariable* pSpecularColor;
	ID3D10EffectScalarVariable* pSpecularPower;
	ID3D10EffectVectorVariable* pEmissive;

	// メッシュの数だけ回す
	for (int meshi = 0; meshi < pModel->GetMeshCount(); meshi++)
	{
		//バーテックスバッファーをセット
		UINT stride = sizeof(VertexType);
		UINT offset = 0;
		ID3D10Buffer* pVertexBuffer = pModel->GetMesh()[meshi].m_pVertexBuffer;
		pDevice->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

		// マテリアルの数だけ回す
		for (int matei = 0; matei < pModel->GetMesh()[meshi].m_nMaterialCount; matei++)
		{
			if (pModel->GetMesh()[meshi].m_pMaterialIndexCount[matei] == 0) continue;

			//シェーダー内のテクスチャを得る
			pShaderTexture = m_pEffect[ModelShader]->GetVariableByName("g_tex")->AsShaderResource();
			pAmbient = m_pEffect[ModelShader]->GetVariableByName("g_Ambient")->AsVector();
			pDiffuse = m_pEffect[ModelShader]->GetVariableByName("g_Diffuse")->AsVector();
			pSpecularColor = m_pEffect[ModelShader]->GetVariableByName("g_SpecularColor")->AsVector();
			pSpecularPower = m_pEffect[ModelShader]->GetVariableByName("g_SpecularPower")->AsScalar();
			pEmissive = m_pEffect[ModelShader]->GetVariableByName("g_Emissive")->AsVector();

			// 透過フラグがたっていなければ、透過しないようにする
			if (TransFlag == false)
			{
				pModel->GetMesh()[meshi].m_pMaterialType[matei].m_vEmission[3] = 1.f;
			}

			pShaderTexture->SetResource(pModel->GetMesh()[meshi].m_pMaterialType[matei].m_pTexture);
			pAmbient->SetFloatVector((float*)&pModel->GetMesh()[meshi].m_pMaterialType[matei].m_vAmbientColor);
			pDiffuse->SetFloatVector((float*)&pModel->GetMesh()[meshi].m_pMaterialType[matei].m_vDiffuseColor);
			pSpecularColor->SetFloatVector((float*)&pModel->GetMesh()[meshi].m_pMaterialType[matei].m_vSpecularColor);
			pSpecularPower->SetFloat(pModel->GetMesh()[meshi].m_pMaterialType[matei].m_fSpecularPower);
			pEmissive->SetFloatVector((float*)&pModel->GetMesh()[meshi].m_pMaterialType[matei].m_vEmission);

			//インデックスバッファーをセット
			stride = sizeof(int);
			offset = 0;
			pDevice->IASetIndexBuffer(pModel->GetMesh()[meshi].m_ppIndexBuffer[matei], DXGI_FORMAT_R32_UINT, 0);

			D3D10_TECHNIQUE_DESC dc;
			m_pTechnique[ModelShader]->GetDesc(&dc);

			for (UINT p = 0; p < dc.Passes; ++p)
			{
				m_pTechnique[ModelShader]->GetPassByIndex(p)->Apply(0);
				pDevice->DrawIndexed(pModel->GetMesh()[meshi].m_pMaterialIndexCount[matei], 0, 0);
			}
		}
	}
}

void CShader::ShaderTextureDraw(float PosX, float PosY, float SizeX, float SizeY,
	Texture pTexture, bool TransFlag, float fDepth)
{
	CDevice3D* pDevice3D = CDevice3D::GetInstance();
	ID3D10Device* pDevice = pDevice3D->GetDevice();

	// 頂点インプットレイアウトをセット
	pDevice->IASetInputLayout(m_pVertexLayout[Texture2DShader]);

	D3DXMATRIX matWorld, matScale, matRota, matTrans;

	D3DXMatrixScaling(&matScale, SizeX, SizeY, 1.f);
	D3DXMatrixRotationZ(&matRota, 0.f / 180.f * 3.14f);
	D3DXMatrixTranslation(&matTrans, PosX + SizeX * 0.5f, PosY + SizeY * 0.5f, fDepth);

	matWorld = matScale * matRota * matTrans;

	// スクリーン座標に変換する行列
	D3DXMATRIX matProj;
	matProj = D3DXMATRIX(
		2.f / (float)SCREEN_WIDTH, 0.f, 0.f, 0.f,
		0.f, -2.f / (float)SCREEN_HEIGHT, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		-1.f, 1.f, 0.f, 1.f);

	// 2D用板ポリゴンモデル作成
	CModel Model;
	Model.CreateQuadrangle2D();

	// フォント画像をシェーダーに送る
	ID3D10EffectShaderResourceVariable* pShaderTexture;
	pShaderTexture = m_pEffect[Texture2DShader]->GetVariableByName("g_tex")->AsShaderResource();
	pShaderTexture->SetResource(pTexture);

	ID3D10EffectMatrixVariable* pShaderWorldViewProjection;
	pShaderWorldViewProjection = m_pEffect[Texture2DShader]->GetVariableByName("g_mWVP")->AsMatrix();
	pShaderWorldViewProjection->SetMatrix((float*)&(matWorld * matProj));

	ID3D10EffectVectorVariable* pShaderColor;
	pShaderColor = m_pEffect[Texture2DShader]->GetVariableByName("g_vColor")->AsVector();
	D3DXVECTOR4 vColor = D3DXVECTOR4(1.f, 1.f, 1.f, 1.f);
	pShaderColor->SetFloatVector((float*)&(vColor));

	ID3D10EffectScalarVariable* pShaderRectUvTop;
	pShaderRectUvTop = m_pEffect[Texture2DShader]->GetVariableByName("g_RectUvTop")->AsScalar();
	pShaderRectUvTop->SetFloat(0.f);

	ID3D10EffectScalarVariable* pShaderRectUvLeft;
	pShaderRectUvLeft = m_pEffect[Texture2DShader]->GetVariableByName("g_RectUvLeft")->AsScalar();
	pShaderRectUvLeft->SetFloat(0.f);

	ID3D10EffectScalarVariable* pShaderRectUvRight;
	pShaderRectUvRight = m_pEffect[Texture2DShader]->GetVariableByName("g_RectUvRight")->AsScalar();
	pShaderRectUvRight->SetFloat(1.f);

	ID3D10EffectScalarVariable* pShaderRectUvBottom;
	pShaderRectUvBottom = m_pEffect[Texture2DShader]->GetVariableByName("g_RectUvBottom")->AsScalar();
	pShaderRectUvBottom->SetFloat(1.f);

	UINT stride = sizeof(VertexType);
	UINT offset = 0;
	ID3D10Buffer* pVertexBuffer = Model.GetMesh()[0].m_pVertexBuffer;
	pDevice->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

	//インデックスバッファーをセット
	stride = sizeof(int);
	offset = 0;
	pDevice->IASetIndexBuffer(Model.GetMesh()[0].m_ppIndexBuffer[0], DXGI_FORMAT_R32_UINT, 0);

	D3D10_TECHNIQUE_DESC dc;
	m_pTechnique[Texture2DShader]->GetDesc(&dc);

	for (UINT p = 0; p < dc.Passes; ++p)
	{
		m_pTechnique[Texture2DShader]->GetPassByIndex(p)->Apply(0);
		pDevice->DrawIndexed(Model.GetMesh()[0].m_pMaterialIndexCount[0], 0, 0);
	}
}

void CShader::ShaderTextureDraw(float PosX, float PosY, float SizeX, float SizeY, float RotaDegree,
	Texture pTexture, const D3DXVECTOR4& vColor, const RectUV& Rect, bool TransFlag)
{
	CDevice3D* pDevice3D = CDevice3D::GetInstance();
	ID3D10Device* pDevice = pDevice3D->GetDevice();

	// 頂点インプットレイアウトをセット
	pDevice->IASetInputLayout(m_pVertexLayout[Texture2DShader]);

	D3DXMATRIX matWorld, matScale, matRota, matTrans;

	D3DXMatrixScaling(&matScale, SizeX, SizeY, 1.f);
	D3DXMatrixRotationZ(&matRota, RotaDegree / 180.f * 3.14f);
	D3DXMatrixTranslation(&matTrans, PosX + SizeX * 0.5f, PosY + SizeY * 0.5f, 0.f);

	matWorld = matScale * matRota * matTrans;

	// スクリーン座標に変換する行列
	D3DXMATRIX matProj;
	matProj = D3DXMATRIX(
		2.f / (float)SCREEN_WIDTH, 0.f, 0.f, 0.f,
		0.f, -2.f / (float)SCREEN_HEIGHT, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		-1.f, 1.f, 0.f, 1.f);

	// 2D用板ポリゴンモデル作成
	CModel Model;
	Model.CreateQuadrangle2D();

	// フォント画像をシェーダーに送る
	ID3D10EffectShaderResourceVariable* pShaderTexture;
	pShaderTexture = m_pEffect[Texture2DShader]->GetVariableByName("g_tex")->AsShaderResource();
	pShaderTexture->SetResource(pTexture);

	ID3D10EffectMatrixVariable* pShaderWorldViewProjection;
	pShaderWorldViewProjection = m_pEffect[Texture2DShader]->GetVariableByName("g_mWVP")->AsMatrix();
	pShaderWorldViewProjection->SetMatrix((float*)&(matWorld * matProj));

	ID3D10EffectVectorVariable* pShaderColor;
	pShaderColor = m_pEffect[Texture2DShader]->GetVariableByName("g_vColor")->AsVector();
	pShaderColor->SetFloatVector((float*)&(vColor));

	ID3D10EffectScalarVariable* pShaderRectUvTop;
	pShaderRectUvTop = m_pEffect[Texture2DShader]->GetVariableByName("g_RectUvTop")->AsScalar();
	pShaderRectUvTop->SetFloat(Rect.m_Top);

	ID3D10EffectScalarVariable* pShaderRectUvLeft;
	pShaderRectUvLeft = m_pEffect[Texture2DShader]->GetVariableByName("g_RectUvLeft")->AsScalar();
	pShaderRectUvLeft->SetFloat(Rect.m_Left);

	ID3D10EffectScalarVariable* pShaderRectUvRight;
	pShaderRectUvRight = m_pEffect[Texture2DShader]->GetVariableByName("g_RectUvRight")->AsScalar();
	pShaderRectUvRight->SetFloat(Rect.m_Right);

	ID3D10EffectScalarVariable* pShaderRectUvBottom;
	pShaderRectUvBottom = m_pEffect[Texture2DShader]->GetVariableByName("g_RectUvBottom")->AsScalar();
	pShaderRectUvBottom->SetFloat(Rect.m_Bottom);

	UINT stride = sizeof(VertexType);
	UINT offset = 0;
	ID3D10Buffer* pVertexBuffer = Model.GetMesh()[0].m_pVertexBuffer;
	pDevice->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

	//インデックスバッファーをセット
	stride = sizeof(int);
	offset = 0;
	pDevice->IASetIndexBuffer(Model.GetMesh()[0].m_ppIndexBuffer[0], DXGI_FORMAT_R32_UINT, 0);

	D3D10_TECHNIQUE_DESC dc;
	m_pTechnique[Texture2DShader]->GetDesc(&dc);

	for (UINT p = 0; p < dc.Passes; ++p)
	{
		m_pTechnique[Texture2DShader]->GetPassByIndex(p)->Apply(0);
		pDevice->DrawIndexed(Model.GetMesh()[0].m_pMaterialIndexCount[0], 0, 0);
	}
}

// 画像描画
// 引数1 : float		X位置情報
// 引数2 : float		Y位置情報
// 引数3 : float		X大きさ情報
// 引数4 : float		Y大きさ情報
// 引数5 : float		回転角度(度数法)
// 引数6 : Texture		画像データ
// 引数7 : D3DXVECTOR4& 色データ
// 引数8 : Rect			画像の切り取り位置
// 引数9 : bool			透過フラグ
void CShader::TextureDraw(float PosX, float PosY, float SizeX, float SizeY, float RotaDegree, Texture pTexture, const D3DXVECTOR4& vColor, const RectUV& Rect, bool TransFlag)
{
	if (TransFlag == false)
	{
		// 透過情報がなければ、そのまま描画
		ShaderTextureDraw(PosX, PosY, SizeX, SizeY, RotaDegree,
			pTexture, vColor, Rect, TransFlag);
	}
	else
	{
		// 透過情報があれば、リストに情報を残して、後で描画する
		DrawModel2D DrawModelData;
		DrawModelData.PosX = PosX;
		DrawModelData.PosY = PosY;
		DrawModelData.RotaDegree = RotaDegree;
		DrawModelData.SizeX = SizeX;
		DrawModelData.SizeY = SizeY;
		DrawModelData.TextureData = pTexture;
		DrawModelData.Rect = Rect;
		DrawModelData.vColor = vColor;
		m_DrawModel2D.push_back(DrawModelData);
	}
}

void CShader::TextureDraw(float PosX, float PosY, float SizeX, float SizeY, Texture pTexture, bool TransFlag, float fDepth)
{
	RectUV Rect;
	Rect.m_Top = 0.f;
	Rect.m_Left = 0.f;
	Rect.m_Right = 1.f;
	Rect.m_Bottom = 1.f;

	if (TransFlag == false)
	{
		// 透過情報がなければ、そのまま描画
		ShaderTextureDraw(PosX, PosY, SizeX, SizeY,
			pTexture, TransFlag, fDepth);
	}
	else
	{
		// 透過情報があれば、リストに情報を残して、後で描画する
		DrawModel2D DrawModelData;
		DrawModelData.PosX = PosX;
		DrawModelData.PosY = PosY;
		DrawModelData.RotaDegree = 0.f;
		DrawModelData.SizeX = SizeX;
		DrawModelData.SizeY = SizeY;
		DrawModelData.TextureData = pTexture;
		DrawModelData.Rect = Rect;
		DrawModelData.vColor = D3DXVECTOR4(1.f, 1.f, 1.f, 1.f);
		m_DrawModel2D.push_back(DrawModelData);
	}
}

// モデル描画
// 引数1 モデルデータ
// 引数2 ワールド行列
// 引数3 透過フラグ
void CShader::ModelDraw(CModel* pModel, const D3DXMATRIX& matWorld, bool TransFlag)
{
	if (TransFlag == false)
	{
		// 透過情報がなければ、そのまま描画
		ShaderModelDraw(pModel, matWorld, TransFlag);
	}
	else
	{
		// 透過情報があれば、リストに情報を残して、後で描画する
		DrawModel3D DrawModelData;
		DrawModelData.pModel = pModel;
		DrawModelData.matWorld = matWorld;
		m_DrawModel3D.push_back(DrawModelData);
	}
}

// 透過情報が入ったモデルを描画する
void CShader::TransModelDraw()
{
	for (auto itr = m_DrawModel3D.begin(); itr != m_DrawModel3D.end(); itr++)
	{
		// 描画
		ShaderModelDraw(itr->pModel, itr->matWorld, true);
	}

	for (auto itr = m_DrawModel2D.begin(); itr != m_DrawModel2D.end(); itr++)
	{
		// 描画
		ShaderTextureDraw(itr->PosX, itr->PosY, itr->SizeX, itr->SizeY, itr->RotaDegree, itr->TextureData, itr->vColor, itr->Rect, true);
	}

	// 描画し終えたので削除
	m_DrawModel3D.clear();
	m_DrawModel2D.clear();
}

// 影描画
void CShader::ModelShadowDraw(const CModel* pModel, const D3DXMATRIX& matWorld)
{
	CDevice3D* pDevice3D = CDevice3D::GetInstance();
	ID3D10Device* pDevice = pDevice3D->GetDevice();

	// 頂点インプットレイアウトをセット
	pDevice->IASetInputLayout(m_pVertexLayout[ShadowShader]);

	// モデル情報がなければ、処理終了
	if (pModel == NULL) return;

	CLightManager* pLightManager = CLightManager::GetInstance();

	// ライト情報を配列に入れる
	int nLightKindArray[RIGHT_NUM_MAX];
	D3DXVECTOR3 vLightDirction[RIGHT_NUM_MAX];
	D3DXVECTOR4 vLightPoint[RIGHT_NUM_MAX];
	float fLightIntensity[RIGHT_NUM_MAX];

	for (int i = 0; i < RIGHT_NUM_MAX; i++)
	{
		CLight* Light = pLightManager->GetLight(i);
		nLightKindArray[i] = Light->GetLightKind();
		vLightDirction[i] = Light->GetLightDirection();
		vLightPoint[i] = Light->GetLightPoint();
		fLightIntensity[i] = Light->GetLightIntensity();
	}

	D3DXMATRIX matShadow;
	D3DXVECTOR4 vLight = D3DXVECTOR4(-vLightDirction[0].x, -vLightDirction[0].y, -vLightDirction[0].z, 0.f);
	D3DXPLANE Plane = D3DXPLANE(0.f, 1.f, 0.f, -0.1f);
	D3DXMatrixShadow(&matShadow, &vLight, &Plane);

	D3DXMATRIX matAnimationBone[100];
	for (int i = 0; i < 100; i++)
	{
		D3DXMatrixIdentity(&matAnimationBone[i]);
	}

	// ボーンアニメーション情報取得
	for (int i = 0; i < pModel->GetBoneCount(); i++)
	{
		matAnimationBone[i] = pModel->GetBone(i)->m_matAnimation;
	}

	//ワールド・ビュー・プロジェクション行列をシェーダーに渡す
	ID3D10EffectMatrixVariable* pShaderWorldViewProjection;
	pShaderWorldViewProjection = m_pEffect[ShadowShader]->GetVariableByName("g_mWVP")->AsMatrix();
	pShaderWorldViewProjection->SetMatrix((float*)&((matWorld * matShadow) * (*g_pCamera->GetMatView())*(*g_pCamera->GetMatProj())));

	// ボーン行列をセット
	ID3D10EffectMatrixVariable* pShaderBoneMatrix;
	pShaderBoneMatrix = m_pEffect[ShadowShader]->GetVariableByName("g_MatBone")->AsMatrix();
	pShaderBoneMatrix->SetMatrixArray((float*)&(matAnimationBone), 0, 100);

	// メッシュの数だけ回す
	for (int meshi = 0; meshi < pModel->GetMeshCount(); meshi++)
	{
		//バーテックスバッファーをセット
		UINT stride = sizeof(VertexType);
		UINT offset = 0;
		ID3D10Buffer* pVertexBuffer = pModel->GetMesh()[meshi].m_pVertexBuffer;
		pDevice->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

		// マテリアルの数だけ回す
		for (int matei = 0; matei < pModel->GetMesh()[meshi].m_nMaterialCount; matei++)
		{
			if (pModel->GetMesh()[meshi].m_pMaterialIndexCount[matei] == 0) continue;

			//インデックスバッファーをセット
			stride = sizeof(int);
			offset = 0;
			pDevice->IASetIndexBuffer(pModel->GetMesh()[meshi].m_ppIndexBuffer[matei], DXGI_FORMAT_R32_UINT, 0);

			D3D10_TECHNIQUE_DESC dc;
			m_pTechnique[ShadowShader]->GetDesc(&dc);

			for (UINT p = 0; p < dc.Passes; ++p)
			{
				m_pTechnique[ShadowShader]->GetPassByIndex(p)->Apply(0);
				pDevice->DrawIndexed(pModel->GetMesh()[meshi].m_pMaterialIndexCount[matei], 0, 0);
			}
		}
	}
}
