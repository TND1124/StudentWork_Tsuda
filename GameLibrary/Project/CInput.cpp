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
	// ����������h��
	if (m_pInstance != nullptr)
	{
		MessageBox(NULL, L"CInput��������������܂����B",
			L"�G���[", MB_OK);
		return;
	}

	m_pInstance = new CInput;
}

CInput* CInput::GetInstance()
{
	if (m_pInstance == NULL)
	{
		MessageBox(NULL, L"CInput�N���X����������Ă��܂���", L"�G���[", MB_OK);
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

// �E�C���h�E�v���V�[�W���[���g���ă}�E�X�̈ʒu�ƃL�[���͂��擾
void CInput::MessageUpdate(UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
		// �}�E�X�̈ʒu�擾
		case WM_MOUSEMOVE:
		{
			m_nMousePositionX = LOWORD(lParam); 
			m_nMousePositionY = HIWORD(lParam);
			break;
		}
		// �L�[���͎擾
		// ������Ă���ꍇ--------------------------------
		case WM_KEYDOWN:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		{
			m_InputKey[wParam] = Push;
			break;
		}
		// ------------------------------------------------

		// ������Ă��Ȃ��ꍇ-----------------------------------
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

// ���͏��̍X�V(�������ςȂ�����p)
void CInput::Update()
{
	for (int i = 0; i < 256; i++)
	{
		if (m_InputKey[i] == Push)
		{
			// �������ςȂ�����
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

// �L�[�������Ă���Ԃ͏��(�A�ˏ��)
bool CInput::GetKey(int keycode)
{
	return m_InputKey[keycode] == HoldPush || m_InputKey[keycode] == Push;
}

// �L�[�E�}�E�X�{�^���̃v�b�V���m�F(�L�[�������ꂽ���̃t���[�������Ă΂��)
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
