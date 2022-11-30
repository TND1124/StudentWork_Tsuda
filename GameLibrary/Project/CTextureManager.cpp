// �摜�f�[�^�������Ă���f�B���N�g���������Ŏw��
// (�����Ŏw�肷�邱�Ƃł킴�킴�S�Ẵf�B���N�g���������Ȃ��Ă��悭�Ȃ�)
#define GRAPHIC_DIRECTORY (L"Asset\\Graphic\\")

#include "CTextureManager.h"
#include "CDevice3D.h"

using namespace std;

CTextureManager* CTextureManager::m_pInstance = 0;

void CTextureManager::Create()
{
	if (m_pInstance != NULL)
	{
		MessageBox(NULL, L"�e�N�X�`���Ǘ��N���X�쐬���s", L"�G���[", MB_OK);
		return;
	}

	m_pInstance = new CTextureManager;
}

CTextureManager* CTextureManager::GetInstance()
{
	if (m_pInstance == NULL)
	{
		MessageBox(NULL, L"�e�N�X�`���Ǘ��N���X����������Ă��܂���B", L"�G���[", MB_OK);
		return NULL;
	}

	return m_pInstance;
}

void CTextureManager::Destory()
{
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

// �摜�쐬
// ����1 const wchar_t* : Asset\Graphic�ɓ����Ă���t�@�C����
void CTextureManager::CreateTexture(const wchar_t* FileName)
{
	CDevice3D* pDevice = CDevice3D::GetInstance();
	Texture pTexture;

	// �f�B���N�g���ƃt�@�C���������킹��
	wstring wsFileName(GRAPHIC_DIRECTORY);
	wsFileName += FileName;

	HRESULT hr;
	//�e�N�X�`���p�摜��ǂݍ��݃e�N�X�`�����쐬
	hr = D3DX10CreateShaderResourceViewFromFile(pDevice->GetDevice(), wsFileName.c_str(), NULL, NULL, &pTexture, NULL);
	if (FAILED(hr))
	{
		wchar_t str[256];
		wcscpy_s(str, wsFileName.c_str());
		wcscat_s(str, L"��������܂���ł����B");
		MessageBox(NULL, str, L"�G���[", MB_OK);
		return;
	}
	
	// �e�N�X�`�����ǉ�
	m_TextureMap.insert(make_pair(wsFileName, pTexture));
}

// �摜�f�[�^�擾
Texture CTextureManager::GetTexture(const wchar_t* FileName)
{
	unordered_map<wstring, Texture>::iterator itr;

	// �f�B���N�g���ƃt�@�C���������킹��
	wstring wsFileName(GRAPHIC_DIRECTORY);
	wsFileName += FileName;

	// �摜�f�[�^��T��
	itr = m_TextureMap.find(wsFileName);

	// �f�[�^�����������ꍇ
	if (itr != m_TextureMap.end())
	{
		// �摜�f�[�^��Ԃ�
		return itr->second;
	}

	// �f�[�^���Ȃ������ꍇ
	wchar_t str[256];
	wcscpy_s(str, wsFileName.c_str());
	wcscat_s(str, L"���쐬����Ă��܂���B");
	MessageBox(NULL, str, L"�G���[", MB_OK);
	return NULL;
}
