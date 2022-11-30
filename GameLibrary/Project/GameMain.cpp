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

// �f�o�b�O�p���f��
CModel* g_pDebugBoxModel;
CModel* g_pDebugSphereModel;

bool Init();
void UnInit();
void Update();
void Draw();

// �������֐�
bool Init()
{
	// �K�v�ȃI�u�W�F�N�g���쐬
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

	// �J�������ݒ�
	g_pCamera = new CCamera;
	D3DXVECTOR3 Eye(0.0f, 0.0f, -10.0f);
	D3DXVECTOR3 At(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 Up(0.0f, 1.0f, 0.0f);
	g_pCamera->Init(Eye, At, Up);

	// ���C�g�ݒ�
	// ���C�g0�Ԃ𕽍s���C�g�ɐݒ�
	CLight* Light1 = pLightManager->GetLight(0);
	Light1->SetLightKind(Light_Directional);
	Light1->SetLightDirectional(D3DXVECTOR3(1.f, -0.3f, 0.f));
	// ���C�g1�Ԃ𕽍s���C�g�ɐݒ�
	CLight* Light2 = pLightManager->GetLight(1);
	Light2->SetLightKind(Light_Directional);
	Light2->SetLightDirectional(D3DXVECTOR3(-1.f, -0.3f, 0.f));
	
	CModelManager* pModelManager = CModelManager::GetInstance();

	// �����f���쐬
	pModelManager->CreateModel(L"sword.nx");
	// �v���C���[���f���쐬
	pModelManager->CreateModel(L"player.nx");

	CModel* pModel = pModelManager->GetModel(L"player.nx");
	
	pModel->LoadNXAnimation(L"Asset\\Model\\playerWalkAni.nx",   0);
	pModel->LoadNXAnimation(L"Asset\\Model\\playerRunAni.nx",    1);
	pModel->LoadNXAnimation(L"Asset\\Model\\playerIdle2Ani.nx",  2);
	pModel->LoadNXAnimation(L"Asset\\Model\\playerImpactAni.nx", 3);
	pModel->LoadNXAnimation(L"Asset\\Model\\playerSlashAni.nx",  4);
	
	// �n�ʃ��f���쐬
	pModelManager->CreateModel(L"zimen.nx");
	// �󃂃f���쐬
	pModelManager->CreateModel(L"sky.nx");
	// �G���f���쐬
	pModelManager->CreateModel(L"JumpEnemy.nx");

	// �f�o�b�O�p���f���쐬
	g_pDebugBoxModel = new CModel;
	g_pDebugBoxModel->LoadNXModel(L"Asset\\Model\\DebugBox.nx");
	g_pDebugSphereModel = new CModel;
	g_pDebugSphereModel->LoadNXModel(L"Asset\\Model\\DebugSphere.nx");

	return true;
}

// �폜�֐�
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

// �X�V�֐�
void Update()
{
	// �f�o�b�O���X�V
	CDebug* pDebug = CDebug::GetInstance();
	pDebug->Update();

	// �J�������X�V
	g_pCamera->Update();

	// ���̓C�x���g�X�V
	CInput* pInput = CInput::GetInstance();
	pInput->Update();

	CSceneManager* pCSceneManager = CSceneManager::GetInstance();
	pCSceneManager->Update();

	// �^�X�N�V�X�e���X�V
	CTaskSystem* pTaskSystem = CTaskSystem::GetInstance();
	pTaskSystem->Update();
}

//�����_�����O����֐�
void Draw()
{
	CDevice3D* pDevice3D = CDevice3D::GetInstance();
	
	// �o�b�N�o�b�t�@���N���A
	float ClearColor[4] = { 0,0,0,1 };
	pDevice3D->GetDevice()->ClearRenderTargetView(pDevice3D->GetRenderTargetView(), ClearColor);//��ʃN���A 

	// �[�x�X�e���V���r���[���N���A
	pDevice3D->GetDevice()->ClearDepthStencilView(pDevice3D->GetDepthStencilView(),
		(UINT)(D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL), 1.f, (UINT8)0.f);

	// �V�[���̕`��J�n
	CTaskSystem* pTaskSystem = CTaskSystem::GetInstance();
	pTaskSystem->Draw();

	// �f�o�b�O���`��
	CDebug* pDebug = CDebug::GetInstance();
	pDebug->Draw();
	
	// ���߃I�u�W�F�N�g�`��
	CShader* pShader = CShader::GetInstance();
	pShader->TransModelDraw();

	// �`��I��
	pDevice3D->GetSwapChain()->Present(0, 0);//��ʍX�V�i�o�b�N�o�b�t�@���t�����g�o�b�t�@�Ɂj
}
