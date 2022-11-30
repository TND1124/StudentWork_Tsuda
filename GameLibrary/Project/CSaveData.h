#pragma once

// セーブデータを保存するクラス
class CSaveData
{
private:
	CSaveData() {}
	CSaveData(const CSaveData&) {}
	~CSaveData() {}

public:
	static void Create();
	static CSaveData* GetInstance();
	static void Destory();

	void Init(void* data, int data_size); // メンバ初期化
	void Save();						  // データの保存
	void Load();						  // データのロード
	void* GetData() { return m_pData; }	  // セーブデータを返す

private:
	static CSaveData* m_pInstance;

	void* m_pData;   // セーブしたデータ
	int m_nDataSize; // セーブしたデータのサイズ
};
