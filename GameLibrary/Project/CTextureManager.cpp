// 画像データが入っているディレクトリをここで指定
// (ここで指定することでわざわざ全てのディレクトリを書かなくてもよくなる)
#define GRAPHIC_DIRECTORY (L"Asset\\Graphic\\")

#include "CTextureManager.h"
#include "CDevice3D.h"

using namespace std;

CTextureManager* CTextureManager::m_pInstance = 0;

void CTextureManager::Create()
{
	if (m_pInstance != NULL)
	{
		MessageBox(NULL, L"テクスチャ管理クラス作成失敗", L"エラー", MB_OK);
		return;
	}

	m_pInstance = new CTextureManager;
}

CTextureManager* CTextureManager::GetInstance()
{
	if (m_pInstance == NULL)
	{
		MessageBox(NULL, L"テクスチャ管理クラスが生成されていません。", L"エラー", MB_OK);
		return NULL;
	}

	return m_pInstance;
}

void CTextureManager::Destory()
{
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

// 画像作成
// 引数1 const wchar_t* : Asset\Graphicに入っているファイル名
void CTextureManager::CreateTexture(const wchar_t* FileName)
{
	CDevice3D* pDevice = CDevice3D::GetInstance();
	Texture pTexture;

	// ディレクトリとファイル名を合わせる
	wstring wsFileName(GRAPHIC_DIRECTORY);
	wsFileName += FileName;

	HRESULT hr;
	//テクスチャ用画像を読み込みテクスチャを作成
	hr = D3DX10CreateShaderResourceViewFromFile(pDevice->GetDevice(), wsFileName.c_str(), NULL, NULL, &pTexture, NULL);
	if (FAILED(hr))
	{
		wchar_t str[256];
		wcscpy_s(str, wsFileName.c_str());
		wcscat_s(str, L"が見つかりませんでした。");
		MessageBox(NULL, str, L"エラー", MB_OK);
		return;
	}
	
	// テクスチャ情報追加
	m_TextureMap.insert(make_pair(wsFileName, pTexture));
}

// 画像データ取得
Texture CTextureManager::GetTexture(const wchar_t* FileName)
{
	unordered_map<wstring, Texture>::iterator itr;

	// ディレクトリとファイル名を合わせる
	wstring wsFileName(GRAPHIC_DIRECTORY);
	wsFileName += FileName;

	// 画像データを探す
	itr = m_TextureMap.find(wsFileName);

	// データが見つかった場合
	if (itr != m_TextureMap.end())
	{
		// 画像データを返す
		return itr->second;
	}

	// データがなかった場合
	wchar_t str[256];
	wcscpy_s(str, wsFileName.c_str());
	wcscat_s(str, L"が作成されていません。");
	MessageBox(NULL, str, L"エラー", MB_OK);
	return NULL;
}
