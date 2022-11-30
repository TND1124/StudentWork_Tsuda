#pragma once

#pragma warning(push)
#pragma warning(disable : 4005)
#include <d3dx10.h>
#include <unordered_map>
#pragma warning(pop)

class CFont
{
private:
	CFont();
	CFont(const CFont&) {}
	~CFont();

public:
	static void Create();
	static CFont* GetInstance();
	static void Destory();

	// �t�H���g�摜�쐬
	ID3D10ShaderResourceView* FontGraphicCreate(wchar_t Char);

private:
	static CFont* m_pInstance;

	std::unordered_map<wchar_t, ID3D10ShaderResourceView*> m_TextureMap;
	HFONT		m_hFont;	//�t�H���g�n���h���F�_���t�H���g(GDI�I�u�W�F�N�g)
	HDC			m_hdc;		//�f�B�X�v���C�f�o�C�X�R���e�L�X�g�̃n���h��
	HFONT		m_oldFont;	//�t�H���g�n���h���F�����t�H���g(GDI�I�u�W�F�N�g)
	TEXTMETRIC	m_TM;		//�t�H���g���i�[�p
};
