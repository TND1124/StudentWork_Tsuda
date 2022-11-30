#pragma once

#pragma warning(push)
#pragma warning(disable : 4005)
#include <d3dx10.h>
#include <unordered_map>
#pragma warning(pop)

// �킩��ɂ����̂ŕʖ��Ő錾
using Texture = ID3D10ShaderResourceView*;

// �g�p����摜���Ǘ�����N���X(�V���O���g��)
class CTextureManager
{
private:
	CTextureManager() {}
	CTextureManager(const CTextureManager&) {}
	~CTextureManager() {}

public:
	static void Create();
	static CTextureManager* GetInstance();
	static void Destory();
	void CreateTexture(const wchar_t* FileName); // �摜�f�[�^�쐬
	Texture GetTexture(const wchar_t* FileName); // �摜�f�[�^�擾
	
private:
	static CTextureManager* m_pInstance;

	// �摜�f�[�^��ۑ�����ϐ�
	std::unordered_map<std::wstring, Texture> m_TextureMap; 
};
