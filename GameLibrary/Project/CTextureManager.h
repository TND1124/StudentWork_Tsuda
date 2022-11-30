#pragma once

#pragma warning(push)
#pragma warning(disable : 4005)
#include <d3dx10.h>
#include <unordered_map>
#pragma warning(pop)

// わかりにくいので別名で宣言
using Texture = ID3D10ShaderResourceView*;

// 使用する画像を管理するクラス(シングルトン)
class CTextureManager
{
private:
	CTextureManager() {}
	CTextureManager(const CTextureManager&) {}
	~CTextureManager() {}

public:
	static void Create();
	static CTextureManager* GetInstance();
	static void Destory();
	void CreateTexture(const wchar_t* FileName); // 画像データ作成
	Texture GetTexture(const wchar_t* FileName); // 画像データ取得
	
private:
	static CTextureManager* m_pInstance;

	// 画像データを保存する変数
	std::unordered_map<std::wstring, Texture> m_TextureMap; 
};
