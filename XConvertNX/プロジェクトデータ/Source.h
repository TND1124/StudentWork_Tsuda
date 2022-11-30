#pragma once

#include <d3dx10.h>

#pragma comment(lib,"d3dx10.lib")

// �}�e���A�����
struct Material
{
	wchar_t sMaterialName[256]; // �}�e���A����
	wchar_t sTextureName[256];  // �e�N�X�`����
	float vAmbientColor[4];	    // ����
	float vDiffuseColor[4];	    // �g�U��
	float vSpecularColor[3];    // ���ʔ���
	float fSpecularPower;	    // ���ʔ��ˌ��̋���
	float vEmission[3];		    // ����
};

// ���[���h���
struct World
{
	float qRotation[4];			// �N�H�[�^�j�I��
	D3DXMATRIX matRotation;		// ��]�s��
	float vScale[3];			// �g��k��
	float vPosition[3];			// �ʒu���
};

// �A�j���[�V�����L�[
struct AnimationKey
{
	int nEndFrame;				// �ő�t���[����
	World* pWorld;				// �e�t���[���ł̃A�j���[�V�������
};