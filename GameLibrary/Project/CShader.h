#pragma once

#pragma warning(push)
#pragma warning(disable : 4005)
#include "CModel.h"
#include "CTextureManager.h"
#pragma warning(pop)

// シェーダに対応する番号
enum ShaderNumber
{
	ModelShader,	 // 3Dモデル描画用
	Texture2DShader, // 2Dテクスチャ描画用
	ShadowShader,	 // 3Dモデルの影描画用
};

// 画像の切り取り位置(中に入れる数値は0.0～1.0)
struct RectUV
{
	RectUV()
	{
		m_Top = 0.f;
		m_Left = 0.f;
		m_Right = 1.f;
		m_Bottom = 1.f;
	}

	RectUV(float Top, float Left, float Right, float Bottom)
	{
		m_Top = Top;
		m_Left = Left;
		m_Right = Right;
		m_Bottom = Bottom;
	}

	float m_Left;
	float m_Top;
	float m_Right;
	float m_Bottom;
};

class CShader
{
public:
	static void Create();
	static CShader* GetInstance();
	static void Destory();

private:
	// 3Dモデルを描画するのに必要なデータ
	struct DrawModel3D
	{
		CModel* pModel;
		D3DXMATRIX matWorld;
	};

	// 2Dモデルを描画するのに必要なデータ
	struct DrawModel2D
	{
		float PosX;
		float PosY;
		float SizeX;
		float SizeY;
		float RotaDegree;
		D3DXVECTOR4 vColor;
		RectUV Rect;
		Texture TextureData;
	};

private:
	CShader() {}
	~CShader() {}
	CShader(const CShader&) {}

	// シェーダーを使って3Dモデルを描画
	void ShaderModelDraw(CModel* pModel, D3DXMATRIX matWorld, bool TransFlag);
	// シェーダーを使って2D画像を描画
	void ShaderTextureDraw(float PosX, float PosY, float SizeX, float SizeY, float RotaDegree,
		Texture pTexture, const D3DXVECTOR4& vColor, const RectUV& Rect, bool TransFlag);

	void ShaderTextureDraw(float PosX, float PosY, float SizeX, float SizeY,
		Texture pTexture,bool TransFlag, float fDepth);

public:
	bool Init();

	// 透過情報が入ったモデルを描画する
	//(透過モデルのレンダリング順番を変えるための関数なので、通常のモデルの描画はModelDraw関数で行ってください)
	void TransModelDraw();

	// シェーダーを切り替える
	void CreateShader(const wchar_t* ShaderFileName, ShaderNumber eShaderNumber);

	// モデル描画
	// 引数1 : CModel*    モデルデータ
	// 引数2 : D3DXMATRIX ワールド行列
	// 引数3 : bool       透過フラグ
	void ModelDraw(CModel* pModel, const D3DXMATRIX& matWorld, bool TransFlag);

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
	void TextureDraw(float PosX, float PosY, float SizeX, float SizeY, float RotaDegree, Texture pTexture, const D3DXVECTOR4& Color, const RectUV& Rect, bool TransFlag);
	void TextureDraw(float PosX, float PosY, float SizeX, float SizeY, Texture pTexture, const RectUV& Rect, bool TransFlag) { TextureDraw(PosX, PosY, SizeX, SizeY, 0.f, pTexture, D3DXVECTOR4(1.f, 1.f, 1.f, 1.f), Rect, TransFlag); }
	void TextureDraw(float PosX, float PosY, float SizeX, float SizeY, float RotaDegree, Texture pTexture, const RectUV& Rect, bool TransFlag) { TextureDraw(PosX, PosY, SizeX, SizeY, RotaDegree, pTexture, D3DXVECTOR4(1.f, 1.f, 1.f, 1.f), Rect, TransFlag); }
	void TextureDraw(float PosX, float PosY, float SizeX, float SizeY, float RotaDegree, Texture pTexture, const D3DXVECTOR4& Color, bool TransFlag) { TextureDraw(PosX, PosY, SizeX, SizeY, RotaDegree, pTexture, Color, RectUV(), TransFlag); }
	void TextureDraw(float PosX, float PosY, float SizeX, float SizeY, float RotaDegree, Texture pTexture, bool TransFlag) { TextureDraw(PosX, PosY, SizeX, SizeY, RotaDegree, pTexture, D3DXVECTOR4(1.f, 1.f, 1.f, 1.f), TransFlag); }
	void TextureDraw(float PosX, float PosY, float SizeX, float SizeY, Texture pTexture, bool TransFlag) { TextureDraw(PosX, PosY, SizeX, SizeY, 0.f, pTexture, D3DXVECTOR4(1.f, 1.f, 1.f, 1.f), TransFlag); }
	void TextureDraw(float PosX, float PosY, float SizeX, float SizeY, Texture pTexture, bool TransFlag, float fDepth);

	// 文字描画
	// 引数1 : float		X位置情報
	// 引数2 : float		Y位置情報
	// 引数3 : float		大きさ情報
	// 引数5 : float		回転角度(度数法)
	// 引数6 : wchar_t*		表示させる文字列
	// 引数7 : D3DXVECTOR4& 色データ
	void StrDraw(float PosX, float PosY, float FontSize, const wchar_t* Str, const D3DXVECTOR4& Color);
	void StrDraw(float PosX, float PosY, float FontSize, const wchar_t* Str) { StrDraw(PosX, PosY, FontSize, Str, D3DXVECTOR4(1.f, 1.f, 1.f, 1.f)); }
	void StrDraw(float PosX, float PosY, float FontSize, const wchar_t* Str, float fDepth);

	// モデルの影描画
	void ModelShadowDraw(const CModel* pModel, const D3DXMATRIX& matWorld);


private:
	static CShader* m_pInstance;

	ID3D10Effect*           m_pEffect[3];
	ID3D10EffectTechnique*  m_pTechnique[3];
	ID3D10InputLayout*      m_pVertexLayout[3];

	// Zバッファとアルファブレンドの関係で後で描画する透過モデルを入れるリスト
	std::list<DrawModel3D> m_DrawModel3D;
	std::list<DrawModel2D> m_DrawModel2D;
};
