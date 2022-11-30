#pragma once

#include <Windows.h>

// �f�o�b�O
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

	void Update(); // �f�o�b�O���X�V
	void Draw();   // �f�o�b�O���̕`��

private:
	static CDebug* m_pInstance;

	// Fps��}�邽�߂Ɏg��
	DWORD m_StartTime;   // �v���J�n����
	int   m_nFpsCounter; // FPS���J�E���g����
	int   m_nFpsCount;   // ���݂�FPS���L�^����
};
