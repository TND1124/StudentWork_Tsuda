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

	// フォント画像作成
	ID3D10ShaderResourceView* FontGraphicCreate(wchar_t Char);

private:
	static CFont* m_pInstance;

	std::unordered_map<wchar_t, ID3D10ShaderResourceView*> m_TextureMap;
	HFONT		m_hFont;	//フォントハンドル：論理フォント(GDIオブジェクト)
	HDC			m_hdc;		//ディスプレイデバイスコンテキストのハンドル
	HFONT		m_oldFont;	//フォントハンドル：物理フォント(GDIオブジェクト)
	TEXTMETRIC	m_TM;		//フォント情報格納用
};
