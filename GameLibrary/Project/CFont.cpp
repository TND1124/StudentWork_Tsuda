// フォント画像を表示する方法は
// まるぺけつくろー様のサイトから持ってきました。
// http://marupeke296.com/DX10_No5_FontTexture.html

#include "CFont.h"
#include "CDevice3D.h"

#pragma warning(push)
#pragma warning(disable : 4005)
#include <d3dx10.h>

#pragma comment(lib,"d3d10.lib")
#pragma comment(lib,"d3dx10.lib")
#pragma warning(pop)

using namespace std;

CFont* CFont::m_pInstance = 0;

void CFont::Create()
{
	if (m_pInstance != NULL)
	{
		MessageBox(NULL, L"フォントクラス作成失敗", L"エラー", MB_OK);
		return;
	}

	m_pInstance = new CFont;
}

CFont* CFont::GetInstance()
{ 
	if (m_pInstance == NULL)
	{
		MessageBox(NULL, L"フォントクラスが生成されていません。", L"エラー", MB_OK);
		return NULL;
	}

	return m_pInstance;
}

void CFont::Destory()
{
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

CFont::CFont()
{
	// フォントの生成
	LOGFONT lf = { 64, 0, 0, 0, 0, 0, 0, 0, SHIFTJIS_CHARSET, OUT_TT_ONLY_PRECIS,
	CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, L"ＭＳ 明朝" };
	if (!(m_hFont = CreateFontIndirect(&lf)))
	{
		MessageBox(NULL, L"フォント生成失敗", L"エラー", MB_OK);
		return;
	}

	// デバイスコンテキスト取得
	// デバイスにフォントを持たせないとGetGlyphOutline関数はエラーとなる
	m_hdc = GetDC(NULL);
	m_oldFont = (HFONT)SelectObject(m_hdc, m_hFont);

	// フォントビットマップ取得
	GetTextMetrics(m_hdc, &m_TM);
}

CFont::~CFont()
{
	//これらGDIオブジェクトを破棄
	DeleteObject(m_oldFont);
	DeleteObject(m_hFont);
	//ディスプレイデバイスコンテキストハンドル解放
	ReleaseDC(NULL, m_hdc);
}

// フォント画像を作成
ID3D10ShaderResourceView* CFont::FontGraphicCreate(wchar_t Char)
{
	unordered_map< wchar_t, ID3D10ShaderResourceView* > ::iterator itr;

	itr = m_TextureMap.find(Char);
	
	if (itr != m_TextureMap.end())
	{
		return itr->second;
	}

	// 文字コード取得
	UINT code = 0;
	code = (UINT)Char;
	
	GLYPHMETRICS GM;
	CONST MAT2 Mat = { {0,1}, {0,0}, {0,0}, {0,1} };
	DWORD size = GetGlyphOutline(m_hdc, code, GGO_GRAY4_BITMAP, &GM, 0, NULL, &Mat);
	BYTE* ptr = new BYTE[size];
	GetGlyphOutline(m_hdc, code, GGO_GRAY4_BITMAP, &GM, size, ptr, &Mat);

	// テクスチャ作成
	D3D10_TEXTURE2D_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Width = GM.gmCellIncX;
	desc.Height = m_TM.tmHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// RGBA(255,255,255,255)タイプ
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D10_USAGE_DYNAMIC;			// 動的（書き込みするための必須条件）
	desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;	// シェーダリソースとして使う
	desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;	// CPUからアクセスして書き込みOK

	ID3D10Texture2D* pTexture2D = NULL;
	CDevice3D* pDevice3D = CDevice3D::GetInstance();
	pDevice3D->GetDevice()->CreateTexture2D(&desc, 0, &pTexture2D);

	D3DXVECTOR2 wh;
	wh.x = (float)desc.Width;
	wh.y = (float)desc.Height;

	// テクスチャに書き込み
	// テクスチャをマップ（＝ロック）すると、
	// メモリにアクセスするための情報がD3D10_MAPPED_TEXTURE2Dに格納されます。
	D3D10_MAPPED_TEXTURE2D mapped;
	pTexture2D->Map(D3D10CalcSubresource(0, 0, 1),
		D3D10_MAP_WRITE_DISCARD,
		0,
		&mapped);

	BYTE* pBits = (BYTE*)mapped.pData;

	// フォント情報の書き込み
	// iOfs_x, iOfs_y : 書き出し位置(左上)
	// iBmp_w, iBmp_h : フォントビットマップの幅高
	// Level : α値の段階 (GGO_GRAY4_BITMAPなので17段階)
	int iOfs_x = GM.gmptGlyphOrigin.x;
	int iOfs_y = m_TM.tmAscent - GM.gmptGlyphOrigin.y;
	int iBmp_w = GM.gmBlackBoxX + (4 - (GM.gmBlackBoxX % 4)) % 4;
	int iBmp_h = GM.gmBlackBoxY;
	int Level = 17;
	int x, y;
	DWORD Alpha, Color;
	memset(pBits, 0, mapped.RowPitch * m_TM.tmHeight);
	for (y = iOfs_y; y < iOfs_y + iBmp_h; y++)
	{
		for (x = iOfs_x; x < iOfs_x + iBmp_w; x++)
		{
			Alpha = (255 * ptr[x - iOfs_x + iBmp_w * (y - iOfs_y)]) / (Level - 1);
			Color = 0x00ffffff | (Alpha << 24);
			memcpy((BYTE*)pBits + mapped.RowPitch * y + 4 * x, &Color, sizeof(DWORD));
		}
	}

	pTexture2D->Unmap(D3D10CalcSubresource(0, 0, 1));

	ID3D10ShaderResourceView* pTexture;
	pDevice3D->GetDevice()->CreateShaderResourceView(pTexture2D, NULL, &pTexture);

	// フォント画像データを保存
	m_TextureMap.insert(make_pair(Char, pTexture));

	delete[] ptr;

	return pTexture;
}
