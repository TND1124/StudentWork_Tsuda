#pragma once

// �o�^�ł��鐔
#define SOUND_ARRAY_NUM_MAX (32)

#pragma warning(push)
#pragma warning(disable : 4005)
#pragma comment (lib, "dsound.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "winmm.lib")
#pragma warning(pop)

// �C���N���[�h
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>

// �T�E���h�N���X
class CSound
{
private:
	// wav�t�@�C���p�̃I�[�f�B�I�f�[�^
	struct WaveHeaderType
	{
		char chunkId[4];
		unsigned long chunkSize;
		char format[4];
		char subChunkId[4];
		unsigned long subChunkSize;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
		char dataChunkId[4];
		unsigned long dataSize;
	};

	// 2D�T�E���h�ɕK�v�ȏ��
	struct Sound2DData
	{
		Sound2DData()
		{
			pDirectSound	 = NULL;
			pPrimaryBuffer	 = NULL;
			pSecondaryBuffer = NULL;
		}

		IDirectSound8* pDirectSound;
		IDirectSoundBuffer* pPrimaryBuffer;
		IDirectSoundBuffer8* pSecondaryBuffer;
	};

	// 3D�T�E���h�ɕK�v�ȏ��
	struct Sound3DData
	{
		Sound3DData()
		{
			pSecondary3dBuffer = NULL;
			pListener		   = NULL;
		}

		Sound2DData Sound2dData;
		IDirectSound3DBuffer8* pSecondary3dBuffer;
		IDirectSound3DListener8* pListener;
	};

private:
	CSound(HWND hwnd);
	CSound(const CSound&) {}
	~CSound();

public:
	static void Create(HWND hwnd);
	static CSound* GetInstance();
	static void Destory();
	bool LoadWave2D(const wchar_t*, unsigned int);
	bool LoadWave3D(const wchar_t*, unsigned int);
	bool PlayWave2D(unsigned int);
	void StopWave2D(unsigned int);
	bool PlayLoodWave2D(unsigned int);
	bool PlayWave3D(unsigned int, float, float, float);
	void SetListenerPosition(float x, float y, float z);
	void SetListenerPosition(unsigned int nArrayNum, float x, float y, float z);
	bool CheckSoundsSound2D(unsigned int nArrayNum);
	bool CheckSoundsSound3D(unsigned int nArrayNum);
	void SetVolume2D(unsigned int nArrayNum, int nVolumePercent);
	void SetVolume3D(unsigned int nArrayNum, int nVolumePercent);
	// �T�E���h�����폜
	void Delete();

private:
	void ShutdownDirectSound();
	void ShutdownWaveFile();

private:
	static CSound* m_pInstance;

	HWND m_hwnd;
	Sound2DData m_Sound2dArray[SOUND_ARRAY_NUM_MAX];// 2D�T�E���h��������z��
	Sound3DData m_Sound3dArray[SOUND_ARRAY_NUM_MAX];// 3D�T�E���h��������z��
};

// endif