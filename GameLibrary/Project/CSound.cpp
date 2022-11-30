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
	// ����������h��
	if (m_pInstance != nullptr)
	{
		MessageBox(hwnd, L"CSound�𕡐���������܂����B",
			L"�G���[", MB_OK);
		return;
	}

	m_pInstance = new CSound(hwnd);
}

CSound* CSound::GetInstance()
{
	if (m_pInstance == NULL)
	{
		MessageBox(NULL, L"�T�E���h�N���X����������Ă��܂���B", L"�G���[", MB_OK);
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

// �v���C�}���o�b�t�@��p_DirectSound�C���^�[�t�F�C�X�����
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

// 2D�T�E���h��ǂݍ���
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
	
	// �_�C���N�g�T�E���h�C���^�t�F�C�X�|�C���^��������
	result = DirectSoundCreate8(NULL, &m_Sound2dArray[nArrayNum].pDirectSound, NULL);
	if (FAILED(result))
		return false;

	// �������x����ݒ�
	result = m_Sound2dArray[nArrayNum].pDirectSound->SetCooperativeLevel(m_hwnd, DSSCL_PRIORITY);
	if (FAILED(result))
		return false;

	// �v���C�}���o�b�t�@�̐ݒ�
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// �T�E���h�o�b�t�@���쐬
	result = m_Sound2dArray[nArrayNum].pDirectSound->CreateSoundBuffer(&bufferDesc, &m_Sound2dArray[nArrayNum].pPrimaryBuffer , NULL);
	if (FAILED(result))
		return false;

	// �o�b�t�@������
	ppSecondaryBuffer = &m_Sound2dArray[nArrayNum].pSecondaryBuffer;
	
	// �o�C�i���t�@�C�����J��
	if (_wfopen_s(&pFile, szFileName, L"rb") != 0)
		return false;

	// �w�b�_�[��ǂݍ���
	nCount = fread(&waveFileHeader, sizeof(waveFileHeader), 1, pFile);
	if (nCount != 1)
		return false;

	// RIFF�t�H�[�}�b�g�̃`�F�b�N
	if ((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') ||
		(waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
		return false;

	// WAVE�t�H�[�}�b�g�̃`�F�b�N
	if ((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
		(waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
		return false;

	// chunkID�t�H�[�}�b�g�̃`�F�b�N
	if ((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
		(waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
		return false;

	// �I�[�f�B�I�t�H�[�}�b�g��WAVE_FOMRAT_PCM���ǂ���
	if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
		return false;

	// �f�[�^�`�����N�w�b�_�[���`�F�b�N
	if ((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
		(waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
		return false;

	// �t�H�[�}�b�g�ݒ�
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


	// �o�b�t�@�ݒ�
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// �T�E���h�o�b�t�@���쐬
	result = m_Sound2dArray[nArrayNum].pDirectSound->CreateSoundBuffer(&bufferDesc, &pTempBuffer, NULL);
	if (FAILED(result))
		return false;

	result = pTempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)ppSecondaryBuffer);
	if (FAILED(result))
		return false;

	// �o�b�t�@���
	pTempBuffer->Release();
	pTempBuffer = 0;

	// �E�F�[�u�f�[�^�̐擪�Ɉړ�
	fseek(pFile, sizeof(WaveHeaderType), SEEK_SET);

	// �E�F�[�u�t�@�C���f�[�^��ۑ����邽�߂Ƀo�b�t�@���쐬
	pWaveData = new unsigned char[waveFileHeader.dataSize];
	if (!pWaveData)
		return false;

	// �E�F�[�u�t�@�C���f�[�^���o�b�t�@�ɓǂݍ���
	nCount = fread(pWaveData, 1, waveFileHeader.dataSize, pFile);
	if (nCount != waveFileHeader.dataSize)
		return false;

	// �t�@�C�������
	if (fclose(pFile) != 0)
		return false;

	// �E�F�[�u�f�[�^����������
	result = (*ppSecondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&pBufferPtr, (DWORD*)&lBufferSize, NULL, 0, 0);
	if (FAILED(result))
		return false;

	memcpy(pBufferPtr, pWaveData, waveFileHeader.dataSize);

	result = (*ppSecondaryBuffer)->Unlock((void*)pBufferPtr, lBufferSize, NULL, 0);
	if (FAILED(result))
		return false;

	// �o�b�t�@�̃{�����[����100%�ɐݒ�
	result = m_Sound2dArray[nArrayNum].pSecondaryBuffer->SetVolume(DSBVOLUME_MAX);
	if (FAILED(result))
		return false;

	// �E�F�[�u�f�[�^�����
	delete[] pWaveData;
	pWaveData = 0;

	return true;
}

// 3D�T�E���h��ǂݍ���
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

	// �_�C���N�g�T�E���h�C���^�t�F�C�X�|�C���^��������
	result = DirectSoundCreate8(NULL, &m_Sound3dArray[nArrayNum].Sound2dData.pDirectSound, NULL);
	if (FAILED(result))
		return false;

	// �������x����ݒ�
	result = m_Sound3dArray[nArrayNum].Sound2dData.pDirectSound->SetCooperativeLevel(m_hwnd, DSSCL_PRIORITY);
	if (FAILED(result))
		return false;

	// �v���C�}���o�b�t�@�̐ݒ�
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// �T�E���h�o�b�t�@���쐬
	result = m_Sound3dArray[nArrayNum].Sound2dData.pDirectSound->CreateSoundBuffer(&bufferDesc, &m_Sound3dArray->Sound2dData.pPrimaryBuffer, NULL);
	if (FAILED(result))
		return false;

	ppSecondaryBuffer = &m_Sound3dArray[nArrayNum].Sound2dData.pSecondaryBuffer;
	ppSecondary3DBuffer = &m_Sound3dArray[nArrayNum].pSecondary3dBuffer;

	// �o�C�i���t�@�C�����J��
	if (_wfopen_s(&pFile, szFileName, L"rb") != 0)
		return false;

	// �w�b�_�[��ǂݍ���
	nCount = fread(&waveFileHeader, sizeof(waveFileHeader), 1, pFile);
	if (nCount != 1)
		return false;

	// RIFF�t�H�[�}�b�g�̃`�F�b�N
	if ((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') ||
		(waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
		return false;

	// WAVE�t�H�[�}�b�g�̃`�F�b�N
	if ((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
		(waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
		return false;

	// chunkID�t�H�[�}�b�g�̃`�F�b�N
	if ((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
		(waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
		return false;

	// �I�[�f�B�I�t�H�[�}�b�g��WAVE_FOMRAT_PCM���ǂ���
	if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
		return false;

	// Wave�t�@�C�������m�����`���ŋL�^����Ă��邩�ǂ���
	if (waveFileHeader.numChannels != 1)
		return false;

	// �f�[�^�`�����N�w�b�_�[���`�F�b�N
	if ((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
		(waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
		return false;

	// �t�H�[�}�b�g�ݒ�
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = waveFileHeader.sampleRate;
	waveFormat.wBitsPerSample = waveFileHeader.bitsPerSample;
	waveFormat.nChannels = waveFileHeader.numChannels;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// �g�`�`���ɐݒ�
	result = m_Sound3dArray[nArrayNum].Sound2dData.pPrimaryBuffer->SetFormat(&waveFormat);
	if (FAILED(result))
		return false;
	
	// �C���^�[�t�F�[�X���擾
	result = m_Sound3dArray[nArrayNum].Sound2dData.pPrimaryBuffer->QueryInterface(IID_IDirectSound3DListener8, (LPVOID*)&m_Sound3dArray[nArrayNum].pListener);
	if (FAILED(result))
		return false;

	m_Sound3dArray[nArrayNum].pListener->SetPosition(0.f, 0.f, 0.f, DS3D_IMMEDIATE);

	// �o�b�t�@�ݒ�
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// �T�E���h�o�b�t�@���쐬
	result = m_Sound3dArray[nArrayNum].Sound2dData.pDirectSound->CreateSoundBuffer(&bufferDesc, &pTempBuffer, NULL);
	if (FAILED(result))
		return false;
	
	result = pTempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)ppSecondaryBuffer);
	if (FAILED(result))
		return false;

	// �o�b�t�@���
	pTempBuffer->Release();
	pTempBuffer = 0;

	// �E�F�[�u�f�[�^�̐擪�Ɉړ�
	fseek(pFile, sizeof(WaveHeaderType), SEEK_SET);

	// �E�F�[�u�t�@�C���f�[�^��ۑ����邽�߂Ƀo�b�t�@���쐬
	pWaveData = new unsigned char[waveFileHeader.dataSize];
	if (!pWaveData)
		return false;

	// �E�F�[�u�t�@�C���f�[�^���o�b�t�@�ɓǂݍ���
	nCount = fread(pWaveData, 1, waveFileHeader.dataSize, pFile);
	if (nCount != waveFileHeader.dataSize)
		return false;

	// �t�@�C�������
	if (fclose(pFile) != 0)
		return false;

	// �E�F�[�u�f�[�^����������
	result = (*ppSecondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&pBufferPtr, (DWORD*)&lBufferSize, NULL, 0, 0);
	if (FAILED(result))
		return false;

	memcpy(pBufferPtr, pWaveData, waveFileHeader.dataSize);
	
	result = (*ppSecondaryBuffer)->Unlock((void*)pBufferPtr, lBufferSize, NULL, 0);
	if (FAILED(result))
		return false;

	// 3D�C���^�[�t�F�C�X���擾
	result = (*ppSecondaryBuffer)->QueryInterface(IID_IDirectSound3DBuffer8, (void**)ppSecondary3DBuffer);
	if (FAILED(result))
		return false;

	// �o�b�t�@�̃{�����[����100%�ɐݒ�
	result = m_Sound3dArray[nArrayNum].Sound2dData.pSecondaryBuffer->SetVolume(DSBVOLUME_MAX);
	if (FAILED(result))
		return false;

	// �E�F�[�u�f�[�^�����
	delete[] pWaveData;
	pWaveData = 0;

	return true;
}

// �o�b�t�@�����
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

// 2D�T�E���h��炷
bool CSound::PlayWave2D(unsigned int nArrayNum)
{
	HRESULT result;
	if (m_Sound2dArray[nArrayNum].pSecondaryBuffer == NULL)
	{
		return false;
	}

	// �T�E���h�o�b�t�@�̐��̈ʒu�ɐݒ�
	result = m_Sound2dArray[nArrayNum].pSecondaryBuffer->SetCurrentPosition(0);
	if (FAILED(result))
		return false;

	// �o�b�t�@�̒��g���Đ�
	m_Sound2dArray[nArrayNum].pSecondaryBuffer->Play(0, 0, 0);
	if (FAILED(result))
		return false;

	return true;
}

// 3D�T�E���h��炷
bool CSound::PlayWave3D(unsigned int nArrayNum, float x, float y, float z)
{
	HRESULT result;

	if (m_Sound3dArray[nArrayNum].Sound2dData.pSecondaryBuffer == NULL)
	{
		return false;
	}

	// �T�E���h�o�b�t�@�̐��̈ʒu�ɐݒ�
	result = m_Sound3dArray[nArrayNum].Sound2dData.pSecondaryBuffer->SetCurrentPosition(0);
	if (FAILED(result))
		return false;

	// ���𕷂��ʒu���Z�b�g
	m_Sound3dArray[nArrayNum].pSecondary3dBuffer->SetPosition(x, y, z, DS3D_IMMEDIATE);

	// �o�b�t�@�̒��g���Đ�
	m_Sound3dArray[nArrayNum].Sound2dData.pSecondaryBuffer->Play(0, 0, 0);
	if (FAILED(result))
		return false;

	return true;
}

// BGM���~�߂�
void CSound::StopWave2D(unsigned int nArrayNum)
{
	if (m_Sound2dArray[nArrayNum].pSecondaryBuffer == NULL)
	{
		return;
	}

	m_Sound2dArray[nArrayNum].pSecondaryBuffer->Stop();
}

// SE�����[�v������
bool CSound::PlayLoodWave2D(unsigned int nArrayNum)
{
	HRESULT result;

	if (m_Sound2dArray[nArrayNum].pSecondaryBuffer == NULL)
	{
		return false;
	}

	// �T�E���h�o�b�t�@�̐��̈ʒu�ɐݒ�
	result = m_Sound2dArray[nArrayNum].pSecondaryBuffer->SetCurrentPosition(0);
	if (FAILED(result))
		return false;

	// �o�b�t�@�̒��g���Đ�
	m_Sound2dArray[nArrayNum].pSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
	if (FAILED(result))
		return false;

	return true;
}

// SE�̍s���ʒu��ς���(���SE�̈ʒu�����ς����Ȃ�)
void CSound::SetListenerPosition(unsigned int nArrayNumber, float x, float y, float z)
{
	if (m_Sound3dArray[nArrayNumber].pListener == NULL)
	{
		return;
	}

	m_Sound3dArray[nArrayNumber].pListener->SetPosition(x, y, z, DS3D_IMMEDIATE);
}

// �S�Ă�SE�𕷂��ʒu��ς���
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

// 2D�T�E���h�̃{�����[���ݒ�
// ����1 nArrayNum		: �T�E���h��񂪓����Ă���z��̗v�f�ԍ�
// ����2 nVolumePercent : �T�E���h�̉��ʃp�[�Z���g(0�`100) �������l��100
void CSound::SetVolume2D(unsigned int nArrayNum, int nVolumePercent)
{
	if (m_Sound2dArray[nArrayNum].pSecondaryBuffer == NULL)
	{
		return;
	}

	// 0�`100�ɍ��킹�āA�{�����[����ς���
	nVolumePercent = (nVolumePercent - 100) * 100;

	// �T�E���h�{�����[����ݒ�
	m_Sound2dArray[nArrayNum].pSecondaryBuffer->SetVolume(nVolumePercent);
}

// 3D�T�E���h�̃{�����[���ݒ�
// ����1 nArrayNum		: �T�E���h��񂪓����Ă���z��̗v�f�ԍ�
// ����2 nVolumePercent : �T�E���h�̉��ʃp�[�Z���g(0�`100) �������l��100
void CSound::SetVolume3D(unsigned int nArrayNum, int nVolumePercent)
{
	if (m_Sound3dArray[nArrayNum].Sound2dData.pSecondaryBuffer == NULL)
	{
		return;
	}

	// 0�`100�ɍ��킹�āA�{�����[����ς���
	nVolumePercent = (nVolumePercent - 100) * 100;

	// �T�E���h�{�����[����ݒ�
	m_Sound3dArray[nArrayNum].Sound2dData.pSecondaryBuffer->SetVolume(nVolumePercent);
}

// �w�肵���ԍ���SE�����Ă��邩�ǂ���(2D)
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

// �w�肵���ԍ���SE�����Ă��邩�ǂ���(3D)
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
	// �v���C�}���o�b�t�@�ƃC���^�[�t�F�C�X�����
	ShutdownDirectSound();

	// �Z�J���_���o�b�t�@�����
	ShutdownWaveFile();
}
