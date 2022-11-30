#include "CSound.h"
#include "MyAssert.h"

#include <stdio.h>
#include <wchar.h>

CSound* CSound::m_pInstance = nullptr;

CSound::CSound(HWND hwnd)
{
	m_hwnd = hwnd;
}

CSound::~CSound()
{
	Delete();
}

void CSound::Create(HWND hwnd)
{
	// 複数生成を防ぐ
	if (m_pInstance != nullptr)
	{
		MessageBox(hwnd, L"CSoundを複数生成されました。",
			L"エラー", MB_OK);
		return;
	}

	m_pInstance = new CSound(hwnd);
}

CSound* CSound::GetInstance()
{
	if (m_pInstance == NULL)
	{
		MessageBox(NULL, L"サウンドクラスが生成されていません。", L"エラー", MB_OK);
		return NULL;
	}

	return m_pInstance;
}

void CSound::Destory()
{
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

// プライマリバッファとp_DirectSoundインターフェイスを解放
void CSound::ShutdownDirectSound()
{
	for (int i = 0; i < SOUND_ARRAY_NUM_MAX; i++)
	{
		if (m_Sound2dArray[i].pPrimaryBuffer)
		{
			m_Sound2dArray[i].pPrimaryBuffer->Release();
			m_Sound2dArray[i].pPrimaryBuffer = NULL;
		}

		if (m_Sound2dArray[i].pDirectSound)
		{
			m_Sound2dArray[i].pDirectSound->Release();
			m_Sound2dArray[i].pDirectSound = NULL;
		}

		if (m_Sound3dArray[i].pListener)
		{
			m_Sound3dArray[i].pListener->Release();
			m_Sound3dArray[i].pListener = NULL;
		}

		if (m_Sound3dArray[i].Sound2dData.pPrimaryBuffer)
		{
			m_Sound3dArray[i].Sound2dData.pPrimaryBuffer->Release();
			m_Sound3dArray[i].Sound2dData.pPrimaryBuffer = NULL;
		}

		if (m_Sound3dArray[i].Sound2dData.pDirectSound)
		{
			m_Sound3dArray[i].Sound2dData.pDirectSound->Release();
			m_Sound3dArray[i].Sound2dData.pDirectSound = NULL;
		}
	}

	return;
}

// 2Dサウンドを読み込む
bool CSound::LoadWave2D(const wchar_t* szFileName, unsigned int nArrayNum)
{					
	FILE* pFile;
	unsigned int nCount;
	WaveHeaderType waveFileHeader;
	WAVEFORMATEX waveFormat;
	DSBUFFERDESC bufferDesc;
	HRESULT result;
	IDirectSoundBuffer* pTempBuffer;
	unsigned char* pWaveData;
	unsigned char* pBufferPtr;
	unsigned long lBufferSize;
	IDirectSoundBuffer8** ppSecondaryBuffer;
	
	// ダイレクトサウンドインタフェイスポインタを初期化
	result = DirectSoundCreate8(NULL, &m_Sound2dArray[nArrayNum].pDirectSound, NULL);
	if (FAILED(result))
		return false;

	// 協調レベルを設定
	result = m_Sound2dArray[nArrayNum].pDirectSound->SetCooperativeLevel(m_hwnd, DSSCL_PRIORITY);
	if (FAILED(result))
		return false;

	// プライマリバッファの設定
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// サウンドバッファを作成
	result = m_Sound2dArray[nArrayNum].pDirectSound->CreateSoundBuffer(&bufferDesc, &m_Sound2dArray[nArrayNum].pPrimaryBuffer , NULL);
	if (FAILED(result))
		return false;

	// バッファ初期化
	ppSecondaryBuffer = &m_Sound2dArray[nArrayNum].pSecondaryBuffer;
	
	// バイナリファイルを開く
	if (_wfopen_s(&pFile, szFileName, L"rb") != 0)
		return false;

	// ヘッダーを読み込む
	nCount = fread(&waveFileHeader, sizeof(waveFileHeader), 1, pFile);
	if (nCount != 1)
		return false;

	// RIFFフォーマットのチェック
	if ((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') ||
		(waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
		return false;

	// WAVEフォーマットのチェック
	if ((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
		(waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
		return false;

	// chunkIDフォーマットのチェック
	if ((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
		(waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
		return false;

	// オーディオフォーマットがWAVE_FOMRAT_PCMかどうか
	if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
		return false;

	// データチャンクヘッダーをチェック
	if ((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
		(waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
		return false;

	// フォーマット設定
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = waveFileHeader.sampleRate;
	waveFormat.wBitsPerSample = waveFileHeader.bitsPerSample;
	waveFormat.nChannels = waveFileHeader.numChannels;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	result = m_Sound2dArray[nArrayNum].pPrimaryBuffer->SetFormat(&waveFormat);
	if (FAILED(result))
		return false;


	// バッファ設定
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// サウンドバッファを作成
	result = m_Sound2dArray[nArrayNum].pDirectSound->CreateSoundBuffer(&bufferDesc, &pTempBuffer, NULL);
	if (FAILED(result))
		return false;

	result = pTempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)ppSecondaryBuffer);
	if (FAILED(result))
		return false;

	// バッファ解放
	pTempBuffer->Release();
	pTempBuffer = 0;

	// ウェーブデータの先頭に移動
	fseek(pFile, sizeof(WaveHeaderType), SEEK_SET);

	// ウェーブファイルデータを保存するためにバッファを作成
	pWaveData = new unsigned char[waveFileHeader.dataSize];
	if (!pWaveData)
		return false;

	// ウェーブファイルデータをバッファに読み込む
	nCount = fread(pWaveData, 1, waveFileHeader.dataSize, pFile);
	if (nCount != waveFileHeader.dataSize)
		return false;

	// ファイルを閉じる
	if (fclose(pFile) != 0)
		return false;

	// ウェーブデータを書き込む
	result = (*ppSecondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&pBufferPtr, (DWORD*)&lBufferSize, NULL, 0, 0);
	if (FAILED(result))
		return false;

	memcpy(pBufferPtr, pWaveData, waveFileHeader.dataSize);

	result = (*ppSecondaryBuffer)->Unlock((void*)pBufferPtr, lBufferSize, NULL, 0);
	if (FAILED(result))
		return false;

	// バッファのボリュームを100%に設定
	result = m_Sound2dArray[nArrayNum].pSecondaryBuffer->SetVolume(DSBVOLUME_MAX);
	if (FAILED(result))
		return false;

	// ウェーブデータを解放
	delete[] pWaveData;
	pWaveData = 0;

	return true;
}

// 3Dサウンドを読み込む
bool CSound::LoadWave3D(const wchar_t* szFileName, unsigned int nArrayNum)
{
	FILE* pFile;
	unsigned int nCount;
	WaveHeaderType waveFileHeader;
	WAVEFORMATEX waveFormat;
	DSBUFFERDESC bufferDesc;
	HRESULT result;
	IDirectSoundBuffer* pTempBuffer;
	unsigned char* pWaveData;
	unsigned char* pBufferPtr;
	unsigned long lBufferSize;
	IDirectSoundBuffer8** ppSecondaryBuffer;
	IDirectSound3DBuffer8** ppSecondary3DBuffer;

	// ダイレクトサウンドインタフェイスポインタを初期化
	result = DirectSoundCreate8(NULL, &m_Sound3dArray[nArrayNum].Sound2dData.pDirectSound, NULL);
	if (FAILED(result))
		return false;

	// 協調レベルを設定
	result = m_Sound3dArray[nArrayNum].Sound2dData.pDirectSound->SetCooperativeLevel(m_hwnd, DSSCL_PRIORITY);
	if (FAILED(result))
		return false;

	// プライマリバッファの設定
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// サウンドバッファを作成
	result = m_Sound3dArray[nArrayNum].Sound2dData.pDirectSound->CreateSoundBuffer(&bufferDesc, &m_Sound3dArray->Sound2dData.pPrimaryBuffer, NULL);
	if (FAILED(result))
		return false;

	ppSecondaryBuffer = &m_Sound3dArray[nArrayNum].Sound2dData.pSecondaryBuffer;
	ppSecondary3DBuffer = &m_Sound3dArray[nArrayNum].pSecondary3dBuffer;

	// バイナリファイルを開く
	if (_wfopen_s(&pFile, szFileName, L"rb") != 0)
		return false;

	// ヘッダーを読み込む
	nCount = fread(&waveFileHeader, sizeof(waveFileHeader), 1, pFile);
	if (nCount != 1)
		return false;

	// RIFFフォーマットのチェック
	if ((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') ||
		(waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
		return false;

	// WAVEフォーマットのチェック
	if ((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
		(waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
		return false;

	// chunkIDフォーマットのチェック
	if ((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
		(waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
		return false;

	// オーディオフォーマットがWAVE_FOMRAT_PCMかどうか
	if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
		return false;

	// Waveファイルがモノラル形式で記録されているかどうか
	if (waveFileHeader.numChannels != 1)
		return false;

	// データチャンクヘッダーをチェック
	if ((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
		(waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
		return false;

	// フォーマット設定
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = waveFileHeader.sampleRate;
	waveFormat.wBitsPerSample = waveFileHeader.bitsPerSample;
	waveFormat.nChannels = waveFileHeader.numChannels;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// 波形形式に設定
	result = m_Sound3dArray[nArrayNum].Sound2dData.pPrimaryBuffer->SetFormat(&waveFormat);
	if (FAILED(result))
		return false;
	
	// インターフェースを取得
	result = m_Sound3dArray[nArrayNum].Sound2dData.pPrimaryBuffer->QueryInterface(IID_IDirectSound3DListener8, (LPVOID*)&m_Sound3dArray[nArrayNum].pListener);
	if (FAILED(result))
		return false;

	m_Sound3dArray[nArrayNum].pListener->SetPosition(0.f, 0.f, 0.f, DS3D_IMMEDIATE);

	// バッファ設定
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// サウンドバッファを作成
	result = m_Sound3dArray[nArrayNum].Sound2dData.pDirectSound->CreateSoundBuffer(&bufferDesc, &pTempBuffer, NULL);
	if (FAILED(result))
		return false;
	
	result = pTempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)ppSecondaryBuffer);
	if (FAILED(result))
		return false;

	// バッファ解放
	pTempBuffer->Release();
	pTempBuffer = 0;

	// ウェーブデータの先頭に移動
	fseek(pFile, sizeof(WaveHeaderType), SEEK_SET);

	// ウェーブファイルデータを保存するためにバッファを作成
	pWaveData = new unsigned char[waveFileHeader.dataSize];
	if (!pWaveData)
		return false;

	// ウェーブファイルデータをバッファに読み込む
	nCount = fread(pWaveData, 1, waveFileHeader.dataSize, pFile);
	if (nCount != waveFileHeader.dataSize)
		return false;

	// ファイルを閉じる
	if (fclose(pFile) != 0)
		return false;

	// ウェーブデータを書き込む
	result = (*ppSecondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&pBufferPtr, (DWORD*)&lBufferSize, NULL, 0, 0);
	if (FAILED(result))
		return false;

	memcpy(pBufferPtr, pWaveData, waveFileHeader.dataSize);
	
	result = (*ppSecondaryBuffer)->Unlock((void*)pBufferPtr, lBufferSize, NULL, 0);
	if (FAILED(result))
		return false;

	// 3Dインターフェイスを取得
	result = (*ppSecondaryBuffer)->QueryInterface(IID_IDirectSound3DBuffer8, (void**)ppSecondary3DBuffer);
	if (FAILED(result))
		return false;

	// バッファのボリュームを100%に設定
	result = m_Sound3dArray[nArrayNum].Sound2dData.pSecondaryBuffer->SetVolume(DSBVOLUME_MAX);
	if (FAILED(result))
		return false;

	// ウェーブデータを解放
	delete[] pWaveData;
	pWaveData = 0;

	return true;
}

// バッファを解放
void CSound::ShutdownWaveFile()
{
	for (int i = 0; i < SOUND_ARRAY_NUM_MAX; i++)
	{
		if (m_Sound2dArray[i].pSecondaryBuffer)
		{
			m_Sound2dArray[i].pSecondaryBuffer = NULL;
		}
		
		if (m_Sound3dArray[i].pSecondary3dBuffer)
		{
			m_Sound3dArray[i].pSecondary3dBuffer = NULL;
		}

		if (m_Sound3dArray[i].Sound2dData.pSecondaryBuffer)
		{
			m_Sound3dArray[i].Sound2dData.pSecondaryBuffer = NULL;
		}
	}

	return;
}

// 2Dサウンドを鳴らす
bool CSound::PlayWave2D(unsigned int nArrayNum)
{
	HRESULT result;
	if (m_Sound2dArray[nArrayNum].pSecondaryBuffer == NULL)
	{
		return false;
	}

	// サウンドバッファの線の位置に設定
	result = m_Sound2dArray[nArrayNum].pSecondaryBuffer->SetCurrentPosition(0);
	if (FAILED(result))
		return false;

	// バッファの中身を再生
	m_Sound2dArray[nArrayNum].pSecondaryBuffer->Play(0, 0, 0);
	if (FAILED(result))
		return false;

	return true;
}

// 3Dサウンドを鳴らす
bool CSound::PlayWave3D(unsigned int nArrayNum, float x, float y, float z)
{
	HRESULT result;

	if (m_Sound3dArray[nArrayNum].Sound2dData.pSecondaryBuffer == NULL)
	{
		return false;
	}

	// サウンドバッファの線の位置に設定
	result = m_Sound3dArray[nArrayNum].Sound2dData.pSecondaryBuffer->SetCurrentPosition(0);
	if (FAILED(result))
		return false;

	// 音を聞く位置をセット
	m_Sound3dArray[nArrayNum].pSecondary3dBuffer->SetPosition(x, y, z, DS3D_IMMEDIATE);

	// バッファの中身を再生
	m_Sound3dArray[nArrayNum].Sound2dData.pSecondaryBuffer->Play(0, 0, 0);
	if (FAILED(result))
		return false;

	return true;
}

// BGMを止める
void CSound::StopWave2D(unsigned int nArrayNum)
{
	if (m_Sound2dArray[nArrayNum].pSecondaryBuffer == NULL)
	{
		return;
	}

	m_Sound2dArray[nArrayNum].pSecondaryBuffer->Stop();
}

// SEをループさせる
bool CSound::PlayLoodWave2D(unsigned int nArrayNum)
{
	HRESULT result;

	if (m_Sound2dArray[nArrayNum].pSecondaryBuffer == NULL)
	{
		return false;
	}

	// サウンドバッファの線の位置に設定
	result = m_Sound2dArray[nArrayNum].pSecondaryBuffer->SetCurrentPosition(0);
	if (FAILED(result))
		return false;

	// バッファの中身を再生
	m_Sound2dArray[nArrayNum].pSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
	if (FAILED(result))
		return false;

	return true;
}

// SEの行く位置を変える(一つのSEの位置しか変えられない)
void CSound::SetListenerPosition(unsigned int nArrayNumber, float x, float y, float z)
{
	if (m_Sound3dArray[nArrayNumber].pListener == NULL)
	{
		return;
	}

	m_Sound3dArray[nArrayNumber].pListener->SetPosition(x, y, z, DS3D_IMMEDIATE);
}

// 全てのSEを聞く位置を変える
void CSound::SetListenerPosition(float x, float y, float z)
{
	for (int i = 0; i < SOUND_ARRAY_NUM_MAX; i++)
	{
		if (m_Sound3dArray[i].pListener != NULL)
		{
			m_Sound3dArray[i].pListener->SetPosition(x, y, z, DS3D_IMMEDIATE);
		}
	}
}

// 2Dサウンドのボリューム設定
// 引数1 nArrayNum		: サウンド情報が入っている配列の要素番号
// 引数2 nVolumePercent : サウンドの音量パーセント(0～100) ※初期値が100
void CSound::SetVolume2D(unsigned int nArrayNum, int nVolumePercent)
{
	if (m_Sound2dArray[nArrayNum].pSecondaryBuffer == NULL)
	{
		return;
	}

	// 0～100に合わせて、ボリュームを変える
	nVolumePercent = (nVolumePercent - 100) * 100;

	// サウンドボリュームを設定
	m_Sound2dArray[nArrayNum].pSecondaryBuffer->SetVolume(nVolumePercent);
}

// 3Dサウンドのボリューム設定
// 引数1 nArrayNum		: サウンド情報が入っている配列の要素番号
// 引数2 nVolumePercent : サウンドの音量パーセント(0～100) ※初期値が100
void CSound::SetVolume3D(unsigned int nArrayNum, int nVolumePercent)
{
	if (m_Sound3dArray[nArrayNum].Sound2dData.pSecondaryBuffer == NULL)
	{
		return;
	}

	// 0～100に合わせて、ボリュームを変える
	nVolumePercent = (nVolumePercent - 100) * 100;

	// サウンドボリュームを設定
	m_Sound3dArray[nArrayNum].Sound2dData.pSecondaryBuffer->SetVolume(nVolumePercent);
}

// 指定した番号のSEが鳴っているかどうか(2D)
bool CSound::CheckSoundsSound2D(unsigned int nArrayNum)
{
	DWORD lpDword = 0;

	if (m_Sound2dArray[nArrayNum].pSecondaryBuffer == NULL)
	{
		return false;
	}

	m_Sound2dArray[nArrayNum].pSecondaryBuffer->GetStatus(&lpDword);
	if((lpDword & DSBSTATUS_LOCSOFTWARE) == DSBSTATUS_LOCSOFTWARE)
		return true;
	
	return false;
}

// 指定した番号のSEが鳴っているかどうか(3D)
bool CSound::CheckSoundsSound3D(unsigned int nArrayNum)
{
	DWORD lpDword = 0;

	if (m_Sound3dArray[nArrayNum].Sound2dData.pSecondaryBuffer == NULL)
	{
		return false;
	}

	m_Sound3dArray[nArrayNum].Sound2dData.pSecondaryBuffer->GetStatus(&lpDword);
	if ((lpDword & DSBSTATUS_LOCSOFTWARE) == DSBSTATUS_LOCSOFTWARE)
		return true;

	return false;
}

void CSound::Delete()
{
	// プライマリバッファとインターフェイスを解放
	ShutdownDirectSound();

	// セカンダリバッファを解放
	ShutdownWaveFile();
}
