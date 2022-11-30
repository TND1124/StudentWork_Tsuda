#include "CInput.h"

CInput* CInput::m_pInstance = 0;

CInput::CInput()
{
	m_nMousePositionX = 0;
	m_nMousePositionY = 0;

	memset(m_InputKey, 0x00, sizeof(m_InputKey));

	for (int i = 0; i < 256; i++)
	{
		m_isDownKey[i] = false;
	}
}

void CInput::Create()
{
	// 複数生成を防ぐ
	if (m_pInstance != nullptr)
	{
		MessageBox(NULL, L"CInputが複数生成されました。",
			L"エラー", MB_OK);
		return;
	}

	m_pInstance = new CInput;
}

CInput* CInput::GetInstance()
{
	if (m_pInstance == NULL)
	{
		MessageBox(NULL, L"CInputクラスが生成されていません", L"エラー", MB_OK);
		return NULL;
	}
	return m_pInstance;
}

void CInput::Destory()
{
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

// ウインドウプロシージャーを使ってマウスの位置とキー入力を取得
void CInput::MessageUpdate(UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
		// マウスの位置取得
		case WM_MOUSEMOVE:
		{
			m_nMousePositionX = LOWORD(lParam); 
			m_nMousePositionY = HIWORD(lParam);
			break;
		}
		// キー入力取得
		// 押されている場合--------------------------------
		case WM_KEYDOWN:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		{
			m_InputKey[wParam] = Push;
			break;
		}
		// ------------------------------------------------

		// 押されていない場合-----------------------------------
		case WM_KEYUP:
		{
			m_InputKey[wParam] = NoPush;
			break;
		}
		case WM_LBUTTONUP:
		{
			m_InputKey[VK_LBUTTON] = NoPush;
			break;
		}
		case WM_RBUTTONUP:
		{
			m_InputKey[VK_RBUTTON] = NoPush;
			break;
		}
		case WM_MBUTTONUP:
		{
			m_InputKey[VK_MBUTTON] = NoPush;
			break;
		}
		// ----------------------------------------------------
	}
}

// 入力情報の更新(押しっぱなし判定用)
void CInput::Update()
{
	for (int i = 0; i < 256; i++)
	{
		if (m_InputKey[i] == Push)
		{
			// 押しっぱなし判定
			if (m_isDownKey[i] == true)
			{
				m_InputKey[i] = HoldPush;
			}

			m_isDownKey[i] = true;
		}
		else if (m_InputKey[i] == NoPush)
		{
			m_isDownKey[i] = false;
		}
	}
}

// キーを押している間は常に(連射状態)
bool CInput::GetKey(int keycode)
{
	return m_InputKey[keycode] == HoldPush || m_InputKey[keycode] == Push;
}

// キー・マウスボタンのプッシュ確認(キーが押された時のフレームだけ呼ばれる)
bool CInput::GetKeyDown(int keycode)
{
	return m_InputKey[keycode] == Push;
}

void CInput::KeyInit()
{
	memset(m_InputKey, 0x00, sizeof(m_InputKey));

	for (int i = 0; i < 256; i++)
	{
		m_isDownKey[i] = false;
	}
}
