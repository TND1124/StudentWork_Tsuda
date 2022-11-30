#pragma once

#include <d3dx10.h>

#pragma comment(lib,"d3dx10.lib")

// マテリアル情報
struct Material
{
	wchar_t sMaterialName[256]; // マテリアル名
	wchar_t sTextureName[256];  // テクスチャ名
	float vAmbientColor[4];	    // 環境光
	float vDiffuseColor[4];	    // 拡散光
	float vSpecularColor[3];    // 鏡面反射
	float fSpecularPower;	    // 鏡面反射光の強さ
	float vEmission[3];		    // 発光
};

// ワールド情報
struct World
{
	float qRotation[4];			// クォータニオン
	D3DXMATRIX matRotation;		// 回転行列
	float vScale[3];			// 拡大縮小
	float vPosition[3];			// 位置情報
};

// アニメーションキー
struct AnimationKey
{
	int nEndFrame;				// 最大フレーム数
	World* pWorld;				// 各フレームでのアニメーション情報
};