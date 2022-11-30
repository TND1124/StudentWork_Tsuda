#pragma once

#include <Windows.h>

// 入力情報を取得するクラス(シングルトン)
class CInput
{
private:
	// 入力情報
	enum InputKind
	{
		NoPush, // 押されていない
		Push,   // 押した瞬間
		HoldPush// 押しっぱなし
	};

private:
	CInput();
	CInput(const CInput&) {}
	~CInput() {}

public:
	static void Create();
	static CInput* GetInstance();
	static void Destory();
	
	// プロシージャのメッセージで入力情報を受け取る
	void MessageUpdate(UINT iMsg, WPARAM wParam, LPARAM lParam);
	void Update();

	// キーを押している間は常に(連射状態)
	bool GetKey(int key_code);	  
	// キー・マウスボタンのプッシュ確認(キーが押された時のフレームだけ呼ばれる)
	bool GetKeyDown(int key_code);

	// マウスの位置情報取得
	int GetMousePositionX() { return m_nMousePositionX; }
	int GetMousePositionY() { return m_nMousePositionY; }

	// キー入力情報初期化(シーン遷移の際にキー入力情報を初期化するときに使う)
	void KeyInit();

private:
	static CInput* m_pInstance;
	
	int m_nMousePositionX;	   // マウスの位置X
	int m_nMousePositionY;	   // マウスの位置Y
	bool m_isDownKey[256];	   // 現在キーを押されているかどうかの確認用
	InputKind m_InputKey[256]; // 現在キーを押されているかどうかの確認用
};
