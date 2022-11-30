#pragma once

// �V�[���p���p
class CScene
{
public:
	virtual ~CScene() {}
	// �߂�l CScene* : ���̃V�[���|�C���^(�V�[���`�F���W���Ȃ��ꍇ�́Athis�|�C���^��Ԃ�)
	virtual CScene* Update() = 0;
};

// �V�[���Ǘ�
class CSceneManager
{
private:
	CSceneManager(CScene* pScene);
	CSceneManager(const CSceneManager&) {}
	~CSceneManager();

public:
	static void Create(CScene* pScene);
	static CSceneManager* GetInstance();
	static void Destory();

	void Update();

private:
	static CSceneManager* m_pInstance;

	// ���݂̃V�[���|�C���^
	CScene* m_pScene;
};
