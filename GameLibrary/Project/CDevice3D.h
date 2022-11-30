#pragma once

#include "CModel.h"
#include "CTextureManager.h"

#pragma warning(push)
#pragma warning(disable : 4005)
#include <d3dx10.h>
#pragma warning(pop)

#pragma comment(lib,"d3d10.lib")
#pragma comment(lib,"d3dx10.lib")

// �f�o�C�X���Ǘ�����N���X(�V���O���g��)
class CDevice3D
{
private:
	CDevice3D() {}
	CDevice3D(const CDevice3D&) {}
	~CDevice3D();

public:
	static void Create();
	static CDevice3D* GetInstance();
	static void Destory();

	bool Init();
	
	ID3D10Device*			GetDevice()			  const { return m_pDevice; }
	IDXGISwapChain*			GetSwapChain()		  const { return m_pSwapChain; }
	ID3D10RenderTargetView* GetRenderTargetView() const { return m_pRenderTargetView; }
	ID3D10DepthStencilView* GetDepthStencilView() const { return m_pDepthStencilView; }

private:
	static CDevice3D* m_pInstance;

	ID3D10Device*			m_pDevice;			 // �f�o�C�X
	IDXGISwapChain*			m_pSwapChain;		 // �X���b�v�`�F�[��
	ID3D10RenderTargetView* m_pRenderTargetView; // �����_�����O�ɃA�N�Z�X���郌���_�[�^�[�Q�b�g
	ID3D10DepthStencilView* m_pDepthStencilView; // �[�x�o�b�t�@
};
