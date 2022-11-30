#pragma once

#pragma warning(push)
#pragma warning(disable : 4005)
#include <d3dx10.h>
#include <fstream>
#include <wchar.h>
#pragma warning(pop)

#include "Collision.h"

// �L�����N�^�[�����Ă�ő�̃A�j���[�V������
#define ANIMATION_DATA_MAX (10) 

// ���_�f�[�^
struct VertexType
{
	VertexType()
	{
		// �E�F�C�g�l������
		memset(m_weight, 0x00, sizeof(m_weight));

		// �{�[���s��id�l������
		memset(m_matoffsetid, 0x00, sizeof(m_matoffsetid));
	}

	D3DXVECTOR3 m_vPos;				// �I�u�W�F�N�g���W
	D3DXVECTOR2 m_vTex;				// �e�N�X�`�����W
	D3DXVECTOR3 m_vNor;				// �@�����
	float		m_weight[4];		// �E�F�C�g�l
	int		    m_matoffsetid[4];	// �{�[���s��id
};

// �A�j���[�V�����L�[
struct AnimationKey
{
	AnimationKey()
	{
		m_pmatRotation = NULL;
		m_pvScale = NULL;
		m_pvTranslation = NULL;
	}

	~AnimationKey()
	{
		if (m_pmatRotation != NULL)
		{
			delete[] m_pmatRotation;
		}
		if (m_pvScale != NULL)
		{
			delete[] m_pvScale;
		}
		if (m_pvTranslation != NULL)
		{
			delete[] m_pvTranslation;
		}
	}

	D3DXMATRIX*	    m_pmatRotation;	 // ��]�s��
	D3DXVECTOR3*	m_pvScale;		 // �g��k��
	D3DXVECTOR3*	m_pvTranslation; // ���s�ړ�
};

// �}�e���A���f�[�^
struct MaterialType
{
public:
	MaterialType()
	{
		m_pTexture		 = NULL;
		m_vAmbientColor  = D3DXVECTOR4(0.1f, 0.1f, 0.1f, 0.f);
		m_vDiffuseColor  = D3DXVECTOR4(0.5f, 0.5f, 0.5f, 0.f);
		m_vSpecularColor = D3DXVECTOR4(0.2f, 0.2f, 0.2f, 1.f);
		m_fSpecularPower = 0.f;
		m_vEmission		 = D3DXVECTOR4(0.f, 0.f, 0.f, 0.f);
	}

public:
	ID3D10ShaderResourceView* m_pTexture;		 // �e�N�X�`�����
	D3DXVECTOR4				  m_vAmbientColor;	 // ����
	D3DXVECTOR4				  m_vDiffuseColor;	 // �g�U��
	D3DXVECTOR4				  m_vSpecularColor;	 // ���ʔ���
	float					  m_fSpecularPower;	 // ���ʔ��ˌ��̋���
	D3DXVECTOR4				  m_vEmission;		 // ����
};

// �{�[���f�[�^
class Bone
{
public:
	Bone()
	{
		D3DXMatrixIdentity(&m_matOffset);
		m_ppBoneChild	  = NULL;
		m_nBoneChildCount = 0;
		m_nBoneDepthCount = 0;
		D3DXMatrixIdentity(&m_matAnimation);
		m_pmatOyaAnimation = NULL;
		m_pmatOyaInvOffset = NULL;
	}

	~Bone()
	{
		if (m_ppBoneChild != NULL)
		{
			delete[] m_ppBoneChild;
			m_ppBoneChild = NULL;
		}

		if (m_pmatOyaAnimation != NULL)
		{
			delete[] m_pmatOyaAnimation;
			m_pmatOyaAnimation = NULL;
		}

		if (m_pmatOyaInvOffset != NULL)
		{
			delete[] m_pmatOyaInvOffset;
			m_pmatOyaInvOffset = NULL;
		}
	}

	D3DXMATRIX   m_matOffset;		  // �{�[���I�t�Z�b�g�s��
	D3DXMATRIX   m_matInvOffset;	  // �{�[���t�I�t�Z�b�g�s��
	D3DXMATRIX   m_matAnimation;	  // �A�j���[�V�����s��
	Bone**	     m_ppBoneChild;		  // �q�{�[�����
	int		     m_nBoneChildCount;	  // �q�{�[���̐�
	int		     m_nBoneDepthCount;	  // ������ڂ̃{�[����
	D3DXMATRIX*  m_pmatOyaAnimation;  // �e�{�[���̃A�j���V�����s��
	D3DXMATRIX*  m_pmatOyaInvOffset;  // �e�{�[���̋t�s��
	AnimationKey m_pAnimationKey[10]; // �A�j���[�V�������(10��ނ̓�������������Ȃ�)
};

// ���b�V���f�[�^
struct MeshType
{
public:
	MeshType();
	~MeshType();

public:
	ID3D10Buffer*  m_pVertexBuffer;		  // ���_�o�b�t�@
	ID3D10Buffer** m_ppIndexBuffer;		  // �C���f�b�N�X�o�b�t�@
	int*		   m_pMaterialIndexCount; // �}�e���A���ʂ̃C���f�b�N�X�̐�
	MaterialType*  m_pMaterialType;		  // �}�e���A�����
	int			   m_nMaterialCount;	  // �}�e���A���̐�
};

// ���f���N���X
class CModel
{
public:
	CModel();
	~CModel();
	// �Ǝ��t�H�[�}�b�g��nx�t�@�C����ǂݍ���
	bool LoadNXModel(const wchar_t* pFileName);
	// �Ǝ��t�H�[�}�b�g��nx�A�j���[�V�����t�@�C����ǂݍ���
	bool LoadNXAnimation(const wchar_t* pFileName, int id);

	// �_��񂩂烂�f�����쐬����
	void PointToCreateModel(const D3DXVECTOR3* pPoints, int PointNum);
	// �l�p�`���쐬����(3D�`��p)
	void CreateQuadrangle3D();
	// �l�p�`���쐬����(2D�`��p)
	void CreateQuadrangle2D();

	MeshType* GetMesh() const { return m_pMeshType; }
	int GetMeshCount() const { return m_nMeshCount; }
	Bone* GetBone(int nBone) const { return &m_pBone[nBone]; }
	int GetBoneCount() const { return m_nBoneCount; }
	int GetAnimationFrame(int id) { return m_nEndFrame[id]; }
	Collision::OBB3D* GetObb() { return m_pObb; }
	// �X�L���A�j���[�V����
	// ����1 fFrameSecond : �A�j���[�V�����t���[��
	// ����2 id			  : �A�j���[�V�����ԍ�
	void SkinAnimation(float fFrameSecond, int id);

private:
	// �{�[���A�j���[�V�������쐬����(�ċA�֐�)
	void RecursionAnimation(Bone* pBone, const D3DXMATRIX* pmatOyaAni, const 
		D3DXMATRIX* pmatOyaInv, int nAnimationNumber, float fFrameSecond);

	// nx���_�����擾����֐�
	// ����2 pVertexData : ���_����������ꕨ
	// ����3 pVertexNum  : ���_��
	void NXLoadVertex(FILE** pFile, VertexType** pVertexType, int* pVertexCount);
	// nx�@�������擾����֐�
	void NXLoadNormal(FILE** pFile, VertexType** pVertexType, int pVertexCount);
	// nxUV�����擾����֐�
	void NXLoadUv(FILE** pFile, VertexType** pVertexType, int pVertexCount);
	// nx�}�e���A�������擾����֐�
	void NXLoadMaterial(FILE** pFile, VertexType** pVertexType, int pVertexCount, int*** pppMaterialIndex);

private:
	MeshType* m_pMeshType;  // ���b�V�����
	int m_nMeshCount;		// ���b�V���̐�

	Bone* m_pBone;			// �{�[�����
	int m_nBoneCount;		// �{�[���̐�

	Collision::OBB3D* m_pObb; // Obb���

	int	m_nEndFrame[ANIMATION_DATA_MAX]; // �A�j���[�V�������I������t���[����
};
