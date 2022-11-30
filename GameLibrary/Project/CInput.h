#pragma once

#include <Windows.h>

// ���͏����擾����N���X(�V���O���g��)
class CInput
{
private:
	// ���͏��
	enum InputKind
	{
		NoPush, // ������Ă��Ȃ�
		Push,   // �������u��
		HoldPush// �������ςȂ�
	};

private:
	CInput();
	CInput(const CInput&) {}
	~CInput() {}

public:
	static void Create();
	static CInput* GetInstance();
	static void Destory();
	
	// �v���V�[�W���̃��b�Z�[�W�œ��͏����󂯎��
	void MessageUpdate(UINT iMsg, WPARAM wParam, LPARAM lParam);
	void Update();

	// �L�[�������Ă���Ԃ͏��(�A�ˏ��)
	bool GetKey(int key_code);	  
	// �L�[�E�}�E�X�{�^���̃v�b�V���m�F(�L�[�������ꂽ���̃t���[�������Ă΂��)
	bool GetKeyDown(int key_code);

	// �}�E�X�̈ʒu���擾
	int GetMousePositionX() { return m_nMousePositionX; }
	int GetMousePositionY() { return m_nMousePositionY; }

	// �L�[���͏�񏉊���(�V�[���J�ڂ̍ۂɃL�[���͏�������������Ƃ��Ɏg��)
	void KeyInit();

private:
	static CInput* m_pInstance;
	
	int m_nMousePositionX;	   // �}�E�X�̈ʒuX
	int m_nMousePositionY;	   // �}�E�X�̈ʒuY
	bool m_isDownKey[256];	   // ���݃L�[��������Ă��邩�ǂ����̊m�F�p
	InputKind m_InputKey[256]; // ���݃L�[��������Ă��邩�ǂ����̊m�F�p
};
