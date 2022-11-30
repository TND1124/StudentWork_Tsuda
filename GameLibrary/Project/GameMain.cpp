#include "CSound.h"
#include "CInput.h"
#include "CModel.h"
#include "CCamera.h"
#include "CLight.h"
#include "CDevice3D.h"
#include "CShader.h"
#include "CFont.h"
#include "CTaskSystem.h"
#include "CTextureManager.h"
#include "CSceneManager.h"
#include "CModelManager.h"
#include "CDebug.h"

#include "CPlayer.h"
#include "Cyuka.h"
#include "CSky.h"
#include "CEnemy.h"
#include "CHpGauge.h"
#include "CTitleBackGround.h"
#include "CSceneTitle.h"
#include "CSceneMain.h"

#include "Window.h"

extern HWND g_hWnd;

CCamera* g_pCamera = NULL;

// デバッグ用モデル
CModel* g_pDebugBoxModel;
CModel* g_pDebugSphereModel;

bool Init();
void UnInit();
void Update();
void Draw();

// 初期化関数
bool Init()
{
	// 必要なオブジェクトを作成
	CDevice3D::Create();
	CDevice3D* pDevice = CDevice3D::GetInstance();
	pDevice->Init();
	CShader::Create();
	CShader* pShader = CShader::GetInstance();
	pShader->Init();
	CSound::Create(g_hWnd);
	CLightManager::Create();
	CFont::Create();
	CTaskSystem::Create();
	CLightManager* pLightManager = CLightManager::GetInstance();
	CModelManager::Create();
	CTextureManager::Create();
	CSceneManager::Create(new CSceneTitle);
	CDebug::Create();

	// カメラ情報設定
	g_pCamera = new CCamera;
	D3DXVECTOR3 Eye(0.0f, 0.0f, -10.0f);
	D3DXVECTOR3 At(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 Up(0.0f, 1.0f, 0.0f);
	g_pCamera->Init(Eye, At, Up);

	// ライト設定
	// ライト0番を平行ライトに設定
	CLight* Light1 = pLightManager->GetLight(0);
	Light1->SetLightKind(Light_Directional);
	Light1->SetLightDirectional(D3DXVECTOR3(1.f, -0.3f, 0.f));
	// ライト1番を平行ライトに設定
	CLight* Light2 = pLightManager->GetLight(1);
	Light2->SetLightKind(Light_Directional);
	Light2->SetLightDirectional(D3DXVECTOR3(-1.f, -0.3f, 0.f));
	
	CModelManager* pModelManager = CModelManager::GetInstance();

	// 剣モデル作成
	pModelManager->CreateModel(L"sword.nx");
	// プレイヤーモデル作成
	pModelManager->CreateModel(L"player.nx");

	CModel* pModel = pModelManager->GetModel(L"player.nx");
	
	pModel->LoadNXAnimation(L"Asset\\Model\\playerWalkAni.nx",   0);
	pModel->LoadNXAnimation(L"Asset\\Model\\playerRunAni.nx",    1);
	pModel->LoadNXAnimation(L"Asset\\Model\\playerIdle2Ani.nx",  2);
	pModel->LoadNXAnimation(L"Asset\\Model\\playerImpactAni.nx", 3);
	pModel->LoadNXAnimation(L"Asset\\Model\\playerSlashAni.nx",  4);
	
	// 地面モデル作成
	pModelManager->CreateModel(L"zimen.nx");
	// 空モデル作成
	pModelManager->CreateModel(L"sky.nx");
	// 敵モデル作成
	pModelManager->CreateModel(L"JumpEnemy.nx");

	// デバッグ用モデル作成
	g_pDebugBoxModel = new CModel;
	g_pDebugBoxModel->LoadNXModel(L"Asset\\Model\\DebugBox.nx");
	g_pDebugSphereModel = new CModel;
	g_pDebugSphereModel->LoadNXModel(L"Asset\\Model\\DebugSphere.nx");

	return true;
}

// 削除関数
void UnInit()
{
	CDebug::Destory();
	CSceneManager::Destory();
	CTextureManager::Destory();
	CTaskSystem::Destory();
	CModelManager::Destory();
	CLightManager::Destory();
	CFont::Destory();
	CInput::Destory();
	CSound::Destory();
	CShader::Destory();
	CDevice3D::Destory();
	
	delete g_pCamera;
	delete g_pDebugBoxModel;
	delete g_pDebugSphereModel;
}

// 更新関数
void Update()
{
	// デバッグ情報更新
	CDebug* pDebug = CDebug::GetInstance();
	pDebug->Update();

	// カメラ情報更新
	g_pCamera->Update();

	// 入力イベント更新
	CInput* pInput = CInput::GetInstance();
	pInput->Update();

	CSceneManager* pCSceneManager = CSceneManager::GetInstance();
	pCSceneManager->Update();

	// タスクシステム更新
	CTaskSystem* pTaskSystem = CTaskSystem::GetInstance();
	pTaskSystem->Update();
}

//レンダリングする関数
void Draw()
{
	CDevice3D* pDevice3D = CDevice3D::GetInstance();
	
	// バックバッファをクリア
	float ClearColor[4] = { 0,0,0,1 };
	pDevice3D->GetDevice()->ClearRenderTargetView(pDevice3D->GetRenderTargetView(), ClearColor);//画面クリア 

	// 深度ステンシルビューをクリア
	pDevice3D->GetDevice()->ClearDepthStencilView(pDevice3D->GetDepthStencilView(),
		(UINT)(D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL), 1.f, (UINT8)0.f);

	// シーンの描画開始
	CTaskSystem* pTaskSystem = CTaskSystem::GetInstance();
	pTaskSystem->Draw();

	// デバッグ情報描画
	CDebug* pDebug = CDebug::GetInstance();
	pDebug->Draw();
	
	// 透過オブジェクト描画
	CShader* pShader = CShader::GetInstance();
	pShader->TransModelDraw();

	// 描画終了
	pDevice3D->GetSwapChain()->Present(0, 0);//画面更新（バックバッファをフロントバッファに）
}
