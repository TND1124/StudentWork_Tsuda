//必要なヘッダーファイルのインクルード
#pragma warning(push)
#pragma warning(disable : 4005)
#include <windows.h>
#include <d3dx10.h>
#include <crtdbg.h>
#pragma warning(pop)

#include "CInput.h"

#include "Window.h"

//必要なライブラリファイルのロード
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"d3d10.lib")
#pragma comment(lib,"d3dx10.lib")

HWND g_hWnd = NULL;

//関数プロトタイプの宣言
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

bool Init();
void UnInit();
void Update();
void Draw();

//アプリケーションのエントリー関数
INT WINAPI WinMain( HINSTANCE hInst,HINSTANCE hPrevInst,LPSTR szStr,INT iCmdShow)
{
	// アプリ名
	WCHAR GameName[] = L"ゲームライブラリ";
	// ウィンドウ情報
	WNDCLASSEX wndclass;

	// ウィンドウプロシージャで入力クラスの関数が使われるので、先に作成
	CInput::Create();

	// ウィンドウ設定
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

	// ウィンドウ作成
	g_hWnd = CreateWindow (GameName, GameName,/*WS_OVERLAPPEDWINDOW*/WS_POPUP,
				0,0,SCREEN_WIDTH + 17, SCREEN_HEIGHT + 40,NULL,NULL,hInst,NULL);
    ShowWindow (g_hWnd,SW_SHOW);
    UpdateWindow (g_hWnd);

	Init();

	DWORD StartTime = timeGetTime();// 開始時間
	DWORD NowTime = (DWORD)0;		// 経過時間
	DWORD ElapsedTime = (DWORD)0;	// 現在時間

	// メッセージループ
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
			// エスケープキーを押すと処理終了
			if (pInput->GetKeyDown(VK_ESCAPE)) break;

			NowTime = timeGetTime();
			ElapsedTime = NowTime - StartTime;

			// FPS設定
			if (ElapsedTime > (DWORD)(1000 / FPS_RATE))
			{
				StartTime = NowTime;
				Update();
				Draw();
			}
		}				
	}

	UnInit();

	_CrtDumpMemoryLeaks();	// この時点で開放されていないメモリの情報の表示

	return (INT)msg.wParam;
}

// ウィンドウプロシージャー関数
LRESULT CALLBACK WndProc(HWND hWnd,UINT iMsg,WPARAM wParam,LPARAM lParam)
{	
	CInput* pInput = CInput::GetInstance();
	// 入力情報取得
	pInput->MessageUpdate( iMsg, wParam, lParam );

	switch(iMsg)
	{	
		case WM_DESTROY:
			PostQuitMessage(0);
			break;		
	}
	return DefWindowProc (hWnd, iMsg, wParam, lParam);	
}
