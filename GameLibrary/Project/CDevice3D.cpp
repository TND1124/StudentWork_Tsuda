#include "CDevice3D.h"
#include "Window.h"

extern HWND g_hWnd;

CDevice3D* CDevice3D::m_pInstance = 0;

void CDevice3D::Create()
{
	if (m_pInstance != NULL)
	{
		MessageBox(NULL, L"デバイスクラス生成失敗", L"エラー", MB_OK);
		return;
	}

	m_pInstance = new CDevice3D;
}

CDevice3D* CDevice3D::GetInstance()
{
	if (m_pInstance == NULL)
	{
		MessageBox(NULL, L"デバイスクラスが生成されていません。", L"エラー", MB_OK);
		return NULL;
	}

	return m_pInstance;
}

void CDevice3D::Destory()
{
	if (m_pInstance != NULL)
	{
		delete m_pInstance;
		m_pInstance = 0;
	}
}

CDevice3D::~CDevice3D()
{
	m_pSwapChain->Release();
	m_pRenderTargetView->Release();
	m_pDevice->Release();
}

bool CDevice3D::Init()
{
	HRESULT hr = NULL;
	// デバイスとスワップチェーンの作成
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = SCREEN_WIDTH;
	sd.BufferDesc.Height = SCREEN_HEIGHT;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	if (FAILED(D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL,
		0, D3D10_SDK_VERSION, &sd, &m_pSwapChain, &m_pDevice)))
	{
		return FALSE;
	}
	//レンダーターゲットビューの作成
	ID3D10Texture2D *pBackBuffer;
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&pBackBuffer);
	m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView);
	pBackBuffer->Release();
	m_pDevice->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);

	//ビューポートの設定
	D3D10_VIEWPORT vp;
	vp.Width = SCREEN_WIDTH;
	vp.Height = SCREEN_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_pDevice->RSSetViewports(1, &vp);

	//プリミティブ・トポロジーをセット
	m_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//ラスタライズ設定
	D3D10_RASTERIZER_DESC rdc;
	ZeroMemory(&rdc, sizeof(rdc));
	rdc.CullMode = D3D10_CULL_BACK;
	rdc.FillMode = D3D10_FILL_SOLID;

	ID3D10RasterizerState* pIr = NULL;
	m_pDevice->CreateRasterizerState(&rdc, &pIr);
	m_pDevice->RSSetState(pIr);

	D3D10_TEXTURE2D_DESC DescDepth;
	DescDepth.Width = SCREEN_WIDTH;
	DescDepth.Height = SCREEN_HEIGHT;
	DescDepth.MipLevels = 1;
	DescDepth.ArraySize = 1;
	DescDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DescDepth.SampleDesc.Count = 1;
	DescDepth.SampleDesc.Quality = 0;
	DescDepth.Usage = D3D10_USAGE_DEFAULT;
	DescDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	DescDepth.CPUAccessFlags = 0;
	DescDepth.MiscFlags = 0;

	// 深度ステンシルテクスチャを作成
	ID3D10Texture2D* pDepthTexture = NULL;
	m_pDevice->CreateTexture2D(&DescDepth, NULL, &pDepthTexture);

	// 深度ステンシルビュー作成
	D3D10_DEPTH_STENCIL_VIEW_DESC DepthViewDesc;
	DepthViewDesc.Format = DescDepth.Format;
	DepthViewDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	DepthViewDesc.Texture2D.MipSlice = 0;

	m_pDevice->CreateDepthStencilView(pDepthTexture, &DepthViewDesc, &m_pDepthStencilView);

	// 深度ステンシルビューをレンダーターゲットに設定する
	m_pDevice->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	// ブレンドステート設定
	D3D10_BLEND_DESC BlendStateDesc;
	memset(&BlendStateDesc, 0, sizeof(BlendStateDesc));
	BlendStateDesc.AlphaToCoverageEnable = FALSE;
	BlendStateDesc.BlendEnable[0] = TRUE;
	BlendStateDesc.SrcBlend = D3D10_BLEND_SRC_ALPHA;
	BlendStateDesc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
	BlendStateDesc.BlendOp = D3D10_BLEND_OP_ADD;
	BlendStateDesc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
	BlendStateDesc.SrcBlendAlpha = D3D10_BLEND_ONE;
	BlendStateDesc.DestBlendAlpha = D3D10_BLEND_ZERO;
	BlendStateDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;

	ID3D10BlendState* pBlendState = 0;
	HRESULT hRes = m_pDevice->CreateBlendState(&BlendStateDesc, &pBlendState);
	m_pDevice->OMSetBlendState(pBlendState, D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

	return true;
}
