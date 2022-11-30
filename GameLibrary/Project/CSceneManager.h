#pragma once

// シーン継承用
class CScene
{
public:
	virtual ~CScene() {}
	// 戻り値 CScene* : 次のシーンポインタ(シーンチェンジしない場合は、thisポインタを返す)
	virtual CScene* Update() = 0;
};

// シーン管理
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

	// 現在のシーンポインタ
	CScene* m_pScene;
};
