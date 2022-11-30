#pragma once

#pragma warning(push)
#pragma warning(disable : 4005)
#include "CModel.h"
#include "CTextureManager.h"
#pragma warning(pop)

// �V�F�[�_�ɑΉ�����ԍ�
enum ShaderNumber
{
	ModelShader,	 // 3D���f���`��p
	Texture2DShader, // 2D�e�N�X�`���`��p
	ShadowShader,	 // 3D���f���̉e�`��p
};

// �摜�̐؂���ʒu(���ɓ���鐔�l��0.0�`1.0)
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
	// 3D���f����`�悷��̂ɕK�v�ȃf�[�^
	struct DrawModel3D
	{
		CModel* pModel;
		D3DXMATRIX matWorld;
	};

	// 2D���f����`�悷��̂ɕK�v�ȃf�[�^
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

	// �V�F�[�_�[���g����3D���f����`��
	void ShaderModelDraw(CModel* pModel, D3DXMATRIX matWorld, bool TransFlag);
	// �V�F�[�_�[���g����2D�摜��`��
	void ShaderTextureDraw(float PosX, float PosY, float SizeX, float SizeY, float RotaDegree,
		Texture pTexture, const D3DXVECTOR4& vColor, const RectUV& Rect, bool TransFlag);

	void ShaderTextureDraw(float PosX, float PosY, float SizeX, float SizeY,
		Texture pTexture,bool TransFlag, float fDepth);

public:
	bool Init();

	// ���ߏ�񂪓��������f����`�悷��
	//(���߃��f���̃����_�����O���Ԃ�ς��邽�߂̊֐��Ȃ̂ŁA�ʏ�̃��f���̕`���ModelDraw�֐��ōs���Ă�������)
	void TransModelDraw();

	// �V�F�[�_�[��؂�ւ���
	void CreateShader(const wchar_t* ShaderFileName, ShaderNumber eShaderNumber);

	// ���f���`��
	// ����1 : CModel*    ���f���f�[�^
	// ����2 : D3DXMATRIX ���[���h�s��
	// ����3 : bool       ���߃t���O
	void ModelDraw(CModel* pModel, const D3DXMATRIX& matWorld, bool TransFlag);

	// �摜�`��
	// ����1 : float		X�ʒu���
	// ����2 : float		Y�ʒu���
	// ����3 : float		X�傫�����
	// ����4 : float		Y�傫�����
	// ����5 : float		��]�p�x(�x���@)
	// ����6 : Texture		�摜�f�[�^
	// ����7 : D3DXVECTOR4& �F�f�[�^
	// ����8 : Rect			�摜�̐؂���ʒu
	// ����9 : bool			���߃t���O
	void TextureDraw(float PosX, float PosY, float SizeX, float SizeY, float RotaDegree, Texture pTexture, const D3DXVECTOR4& Color, const RectUV& Rect, bool TransFlag);
	void TextureDraw(float PosX, float PosY, float SizeX, float SizeY, Texture pTexture, const RectUV& Rect, bool TransFlag) { TextureDraw(PosX, PosY, SizeX, SizeY, 0.f, pTexture, D3DXVECTOR4(1.f, 1.f, 1.f, 1.f), Rect, TransFlag); }
	void TextureDraw(float PosX, float PosY, float SizeX, float SizeY, float RotaDegree, Texture pTexture, const RectUV& Rect, bool TransFlag) { TextureDraw(PosX, PosY, SizeX, SizeY, RotaDegree, pTexture, D3DXVECTOR4(1.f, 1.f, 1.f, 1.f), Rect, TransFlag); }
	void TextureDraw(float PosX, float PosY, float SizeX, float SizeY, float RotaDegree, Texture pTexture, const D3DXVECTOR4& Color, bool TransFlag) { TextureDraw(PosX, PosY, SizeX, SizeY, RotaDegree, pTexture, Color, RectUV(), TransFlag); }
	void TextureDraw(float PosX, float PosY, float SizeX, float SizeY, float RotaDegree, Texture pTexture, bool TransFlag) { TextureDraw(PosX, PosY, SizeX, SizeY, RotaDegree, pTexture, D3DXVECTOR4(1.f, 1.f, 1.f, 1.f), TransFlag); }
	void TextureDraw(float PosX, float PosY, float SizeX, float SizeY, Texture pTexture, bool TransFlag) { TextureDraw(PosX, PosY, SizeX, SizeY, 0.f, pTexture, D3DXVECTOR4(1.f, 1.f, 1.f, 1.f), TransFlag); }
	void TextureDraw(float PosX, float PosY, float SizeX, float SizeY, Texture pTexture, bool TransFlag, float fDepth);

	// �����`��
	// ����1 : float		X�ʒu���
	// ����2 : float		Y�ʒu���
	// ����3 : float		�傫�����
	// ����5 : float		��]�p�x(�x���@)
	// ����6 : wchar_t*		�\�������镶����
	// ����7 : D3DXVECTOR4& �F�f�[�^
	void StrDraw(float PosX, float PosY, float FontSize, const wchar_t* Str, const D3DXVECTOR4& Color);
	void StrDraw(float PosX, float PosY, float FontSize, const wchar_t* Str) { StrDraw(PosX, PosY, FontSize, Str, D3DXVECTOR4(1.f, 1.f, 1.f, 1.f)); }
	void StrDraw(float PosX, float PosY, float FontSize, const wchar_t* Str, float fDepth);

	// ���f���̉e�`��
	void ModelShadowDraw(const CModel* pModel, const D3DXMATRIX& matWorld);


private:
	static CShader* m_pInstance;

	ID3D10Effect*           m_pEffect[3];
	ID3D10EffectTechnique*  m_pTechnique[3];
	ID3D10InputLayout*      m_pVertexLayout[3];

	// Z�o�b�t�@�ƃA���t�@�u�����h�̊֌W�Ō�ŕ`�悷�铧�߃��f�������郊�X�g
	std::list<DrawModel3D> m_DrawModel3D;
	std::list<DrawModel2D> m_DrawModel2D;
};
