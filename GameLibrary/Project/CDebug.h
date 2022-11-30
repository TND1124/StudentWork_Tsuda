#pragma once

#include <Windows.h>

// デバッグ
class CDebug
{
private:
	CDebug();
	CDebug(const CDebug&) {}
	~CDebug() {}

public:
	static void Create();
	static CDebug* GetInstance();
	static void Destory();

	void Update(); // デバッグ情報更新
	void Draw();   // デバッグ情報の描画

private:
	static CDebug* m_pInstance;

	// Fpsを図るために使う
	DWORD m_StartTime;   // 計測開始時刻
	int   m_nFpsCounter; // FPSをカウントする
	int   m_nFpsCount;   // 現在のFPSを記録する
};
