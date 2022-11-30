//�K�v�ȃw�b�_�[�t�@�C���̃C���N���[�h
#pragma warning(push)
#pragma warning(disable : 4005)
#include <windows.h>
#include <d3dx10.h>
#include <crtdbg.h>
#pragma warning(pop)

#include "CInput.h"

#include "Window.h"

//�K�v�ȃ��C�u�����t�@�C���̃��[�h
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"d3d10.lib")
#pragma comment(lib,"d3dx10.lib")

HWND g_hWnd = NULL;

//�֐��v���g�^�C�v�̐錾
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

bool Init();
void UnInit();
void Update();
void Draw();

//�A�v���P�[�V�����̃G���g���[�֐�
INT WINAPI WinMain( HINSTANCE hInst,HINSTANCE hPrevInst,LPSTR szStr,INT iCmdShow)
{
	// �A�v����
	WCHAR GameName[] = L"�Q�[�����C�u����";
	// �E�B���h�E���
	WNDCLASSEX wndclass;

	// �E�B���h�E�v���V�[�W���œ��̓N���X�̊֐����g����̂ŁA��ɍ쐬
	CInput::Create();

	// �E�B���h�E�ݒ�
	wndclass.cbSize		   = sizeof (wndclass);
	wndclass.style		   = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc   = WndProc;
	wndclass.cbClsExtra	   = 0;
	wndclass.cbWndExtra	   = 0;
	wndclass.hInstance	   = hInst;
	wndclass.hIcon		   = LoadIcon (NULL,  IDI_APPLICATION);
	wndclass.hCursor	   = LoadCursor (NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
	wndclass.lpszMenuName  = NULL;
	wndclass.lpszClassName = GameName;
	wndclass.hIconSm	   = LoadIcon (NULL,IDI_ASTERISK);
	RegisterClassEx (&wndclass);

	// �E�B���h�E�쐬
	g_hWnd = CreateWindow (GameName, GameName,/*WS_OVERLAPPEDWINDOW*/WS_POPUP,
				0,0,SCREEN_WIDTH + 17, SCREEN_HEIGHT + 40,NULL,NULL,hInst,NULL);
    ShowWindow (g_hWnd,SW_SHOW);
    UpdateWindow (g_hWnd);

	Init();

	DWORD StartTime = timeGetTime();// �J�n����
	DWORD NowTime = (DWORD)0;		// �o�ߎ���
	DWORD ElapsedTime = (DWORD)0;	// ���ݎ���

	// ���b�Z�[�W���[�v
	MSG msg;
    ZeroMemory( &msg, sizeof(msg) );
    while( msg.message!=WM_QUIT )
	{
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
	    {
			TranslateMessage( &msg );
		    DispatchMessage( &msg );
	    }
	    else
		{
			CInput* pInput = CInput::GetInstance();
			// �G�X�P�[�v�L�[�������Ə����I��
			if (pInput->GetKeyDown(VK_ESCAPE)) break;

			NowTime = timeGetTime();
			ElapsedTime = NowTime - StartTime;

			// FPS�ݒ�
			if (ElapsedTime > (DWORD)(1000 / FPS_RATE))
			{
				StartTime = NowTime;
				Update();
				Draw();
			}
		}				
	}

	UnInit();

	_CrtDumpMemoryLeaks();	// ���̎��_�ŊJ������Ă��Ȃ��������̏��̕\��

	return (INT)msg.wParam;
}

// �E�B���h�E�v���V�[�W���[�֐�
LRESULT CALLBACK WndProc(HWND hWnd,UINT iMsg,WPARAM wParam,LPARAM lParam)
{	
	CInput* pInput = CInput::GetInstance();
	// ���͏��擾
	pInput->MessageUpdate( iMsg, wParam, lParam );

	switch(iMsg)
	{	
		case WM_DESTROY:
			PostQuitMessage(0);
			break;		
	}
	return DefWindowProc (hWnd, iMsg, wParam, lParam);	
}
