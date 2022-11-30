#include "CModel.h"

#include "CDevice3D.h"
#include "CTextureManager.h"

#include <wchar.h>
#include <vector>

void WeightInsertionSort(float* p_fSortWeight, int* p_nSortOffsetId, int nElementCount);

MeshType::MeshType()
{
	m_pVertexBuffer = NULL;
	m_ppIndexBuffer = NULL;
	m_nMaterialCount = 0;
}

MeshType::~MeshType()
{
	// �o�[�e�b�N�X�o�b�t�@���
	if (m_pVertexBuffer != NULL)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
	}

	// �C���f�b�N�X�o�b�t�@�����
	if (m_ppIndexBuffer != NULL)
	{
		for (int i = 0; i < m_nMaterialCount; i++)
		{
			if (m_pMaterialIndexCount[i] != 0)
			{
				m_ppIndexBuffer[i]->Release();
				m_ppIndexBuffer[i] = NULL;
			}
		}
		delete[] m_ppIndexBuffer;
	}

	// �}�e���A�������
	if (m_pMaterialType != NULL)
	{
		delete[] m_pMaterialType;
		m_pMaterialType = NULL;
	}

	// �}�e���A���C���f�b�N�X�����
	if (m_pMaterialIndexCount != NULL)
	{
		delete[] m_pMaterialIndexCount;
		m_pMaterialIndexCount = NULL;
	}
}

CModel::CModel()
{
	m_nMeshCount = 0;

	m_pMeshType = NULL;
	m_pBone = NULL;
	m_pObb = NULL;
}

CModel::~CModel()
{
	// ���b�V���f�[�^���
	if (m_pMeshType != NULL)
	{
		delete[] m_pMeshType;
		m_pMeshType = NULL;
	}

	// �{�[���f�[�^���
	if (m_pBone != NULL)
	{
		delete[] m_pBone;
		m_pBone = NULL;
	}

	// OBB�f�[�^���
	if (m_pObb != NULL)
	{
		delete[] m_pObb;
		m_pObb = NULL;
	}
}

// �Ǝ��t�H�[�}�b�g��nx�t�@�C����ǂݍ���
bool CModel::LoadNXModel(const wchar_t* pFileName)
{
	FILE* pFile;
	wchar_t FileData[256];
	
	VertexType* pVertexType = NULL;
	int nVertexCount = 0;

	// �}�e���A���ʂɃC���f�b�N�X����������ꕨ
	int** ppMaterialIndex = NULL;
	
	// ���b�V���̐���nx�t�@�C���ł͈�����Ή����Ă��܂���
	m_pMeshType = new MeshType[1];
	m_nMeshCount = 1;

	// �t�@�C���I�[�v��
	if (_wfopen_s(&pFile, pFileName, L"r") != 0)
	{
		wchar_t ErrorString[256];
		swprintf_s(ErrorString, 256, L"%s��������܂���ł����B",
			pFileName);
		MessageBox(NULL, ErrorString, L"�G���[", MB_OK);
		return false;
	}
	
	while (fwscanf_s(pFile, L"%s", FileData, 256) != EOF)
	{
		// ���_���ǂݍ��݉ӏ�
		if (wcscmp(FileData, L"vnum") == 0)
		{
			NXLoadVertex(&pFile, &pVertexType, &nVertexCount);
		}
		// �@�����ǂݍ���
		else if (wcscmp(FileData, L"nnum") == 0)
		{
			NXLoadNormal(&pFile, &pVertexType, nVertexCount);
		}
		// uv���ǂݍ���
		else if (wcscmp(FileData, L"uvnum") == 0)
		{
			NXLoadUv(&pFile, &pVertexType, nVertexCount);
		}
		// �}�e���A�����ǂݍ���
		else if (wcscmp(FileData, L"matenum") == 0)
		{
			NXLoadMaterial(&pFile, &pVertexType, nVertexCount, &ppMaterialIndex);
		}
		// �{�[�����擾
		else if (wcscmp(FileData, L"bonenum") == 0)
		{
			// �{�[�����擾
			fwscanf_s(pFile, L"%s", FileData, 256);
			swscanf_s(FileData, L"%d", &m_nBoneCount);

			// �{�[��������������ꕨ���쐬
			m_pBone = new Bone[m_nBoneCount];

			// �{�[���̑�ڐ����擾(1��ڂȂ�0������)
			for (int i = 0; i < m_nBoneCount; i++)
			{
				fwscanf_s(pFile, L"%s", FileData, 256);
				fwscanf_s(pFile, L"%s", FileData, 256);
				swscanf_s(FileData, L"%d", &m_pBone[i].m_nBoneDepthCount);
			}

			// �q�{�[����������
			for (int i = 0; i < m_nBoneCount; i++)
			{
				fwscanf_s(pFile, L"%s", FileData, 256);
				fwscanf_s(pFile, L"%s", FileData, 256);
				swscanf_s(FileData, L"%d", &m_pBone[i].m_nBoneChildCount);

				// �q�̃{�[���̐������A�q�̃{�[����������ꕨ�����
				m_pBone[i].m_ppBoneChild = new Bone*[m_pBone[i].m_nBoneChildCount];

				int count = 0;
				// �q�̃{�[���𒲂ׂ�
				for (int j = i + 1; count != m_pBone[i].m_nBoneChildCount; j++)
				{
					if (m_pBone[i].m_nBoneDepthCount + 1 == m_pBone[j].m_nBoneDepthCount)
					{
						m_pBone[i].m_ppBoneChild[count] = &m_pBone[j];

						count++;
					}
				}
			}

			for (int i = 0; i < m_nBoneCount; i++)
			{
				fwscanf_s(pFile, L"%s", FileData, 256);
				fwscanf_s(pFile, L"%s", FileData, 256);

				fgetws(FileData, 256, pFile);

				// �E�F�C�g�l�擾
				// �{�[���I�t�Z�b�g�s��擾
				for (int j = 0; j < nVertexCount; j++)
				{
					int nWeightIndex = 0;
					float nWeight = 0;
					fgetws(FileData, 256, pFile);
					// �擾�����E�F�C�g�l��ۑ�
					swscanf_s(FileData, L"weight %d : %f",
						&nWeightIndex, &nWeight);

					if (nWeight == 0.f) continue;

					for (int wi = 0; wi < 4; wi++)
					{
						// �󂢂Ă���ꏊ����������
						if (pVertexType[nWeightIndex].m_weight[wi] == 0.f)
						{
							// �E�F�C�g�l������
							pVertexType[nWeightIndex].m_weight[wi] = nWeight;

							// �{�[���I�t�Z�b�g���E�F�C�g�l�Ɠ����v�f���œ����
							// �{�[���I�t�Z�b�g�l�ƃE�F�C�g�l���֘A�t����
							pVertexType[nWeightIndex].m_matoffsetid[wi] = i;
							break;
						}

						// ���f���̒��_��5�ȏ�̃{�[���̉e�����󂯂Ă�����̂�
						if (wi == 3)
						{
							float SortWeight[5];
							int SortOffsetId[5];

							// �\�[�g�p�̔z��ɓ���Ȃ���
							for (int si = 0; si < 5; si++)
							{
								SortWeight[si] = pVertexType[nWeightIndex].m_weight[si];
								SortOffsetId[si] = pVertexType[nWeightIndex].m_matoffsetid[si];
							}
							
							SortWeight[4] = nWeight;
							SortOffsetId[4] = i;

							// �E�F�C�g���傫�����Ƀ\�[�g
							WeightInsertionSort(SortWeight, SortOffsetId, 5);

							// �\�[�g�p�̔z�񂩂猳�̔z��ɓ���Ȃ���
							for (int si = 0; si < 4; si++)
							{
								pVertexType[nWeightIndex].m_weight[si] = SortWeight[si];
								pVertexType[nWeightIndex].m_matoffsetid[si] = SortOffsetId[si];
							}
							//MessageBox(NULL, L"���̃��f���̒��_��5�ȏ�̃{�[���̉e�����󂯂Ă�����̂�����܂�", L"�x��", MB_OK);
						}
					}
				}

				// �s��I�t�Z�b�g�擾
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"matoffset %f,%f,%f,%f",
					&m_pBone[i].m_matOffset._11, &m_pBone[i].m_matOffset._12,
					&m_pBone[i].m_matOffset._13, &m_pBone[i].m_matOffset._14);

				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"matoffset %f,%f,%f,%f",
					&m_pBone[i].m_matOffset._21, &m_pBone[i].m_matOffset._22,
					&m_pBone[i].m_matOffset._23, &m_pBone[i].m_matOffset._24);

				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"matoffset %f,%f,%f,%f",
					&m_pBone[i].m_matOffset._31, &m_pBone[i].m_matOffset._32,
					&m_pBone[i].m_matOffset._33, &m_pBone[i].m_matOffset._34);

				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"matoffset %f,%f,%f,%f",
					&m_pBone[i].m_matOffset._41, &m_pBone[i].m_matOffset._42,
					&m_pBone[i].m_matOffset._43, &m_pBone[i].m_matOffset._44);

				// �t�s��I�t�Z�b�g�擾
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"invmatoffset %f,%f,%f,%f",
					&m_pBone[i].m_matInvOffset._11, &m_pBone[i].m_matInvOffset._12,
					&m_pBone[i].m_matInvOffset._13, &m_pBone[i].m_matInvOffset._14);

				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"invmatoffset %f,%f,%f,%f",
					&m_pBone[i].m_matInvOffset._21, &m_pBone[i].m_matInvOffset._22,
					&m_pBone[i].m_matInvOffset._23, &m_pBone[i].m_matInvOffset._24);
				
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"invmatoffset %f,%f,%f,%f",
					&m_pBone[i].m_matInvOffset._31, &m_pBone[i].m_matInvOffset._32,
					&m_pBone[i].m_matInvOffset._33, &m_pBone[i].m_matInvOffset._34);

				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"invmatoffset %f,%f,%f,%f",
					&m_pBone[i].m_matInvOffset._41, &m_pBone[i].m_matInvOffset._42,
					&m_pBone[i].m_matInvOffset._43, &m_pBone[i].m_matInvOffset._44);
			}
		}
		else if (wcscmp(FileData, L"obb") == 0)
		{
			// �f�[�^�擾�p�ϐ�
			D3DXVECTOR3 vScale, vPosition;

			fgetws(FileData, 256, pFile);
			fgetws(FileData, 256, pFile);
			swscanf_s(FileData, L"obb_scale %f,%f,%f",
				&vScale.x, &vScale.y, &vScale.z);
				
			fgetws(FileData, 256, pFile);
			swscanf_s(FileData, L"obb_pos %f,%f,%f",
				&vPosition.x, &vPosition.y, &vPosition.z);

			// OBB��񏉊��l������
			m_pObb = new Collision::OBB3D;
			m_pObb->Init(vScale, vPosition);
		}
	}

	//�o�[�e�b�N�X�o�b�t�@�[�쐬
	// CPU����A�N�Z�X�\�ɂ��܂����B(�x���Ȃ邩��)
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VertexType) * nVertexCount;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = pVertexType;

	CDevice3D* pDevice3D = CDevice3D::GetInstance();
	ID3D10Device* pDevice = pDevice3D->GetDevice();

	if (FAILED(pDevice->CreateBuffer(&bd, &InitData, &(m_pMeshType[0].m_pVertexBuffer))))
	{
		MessageBox(NULL, L"���b�V���o�[�e�b�N�X�o�b�t�@�[�쐬���s", L"�G���[", MB_OK);
		return false;
	}
	
	// �C���f�b�N�X�o�b�t�@�[������
	m_pMeshType[0].m_ppIndexBuffer = new ID3D10Buffer*[m_pMeshType[0].m_nMaterialCount];
	for (int i = 0; i < m_pMeshType[0].m_nMaterialCount; i++)
	{
		// �g�p����Ă��Ȃ��}�e���A���̓C���f�b�N�X�o�b�t�@�[���쐬���Ȃ�
		if (m_pMeshType[0].m_pMaterialIndexCount[i] == 0) continue;

		//�C���f�b�N�X�o�b�t�@�[���쐬(�}�e���A���̐�����)
		// CPU����A�N�Z�X�\�ɂ��܂����B(�x���Ȃ邩��)
		bd.Usage = D3D10_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(int) * m_pMeshType[0].m_pMaterialIndexCount[i];
		bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		InitData.pSysMem = ppMaterialIndex[i];
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;
		if (FAILED(pDevice->CreateBuffer(&bd, &InitData, &(m_pMeshType[0].m_ppIndexBuffer[i]))))
		{
			MessageBox(NULL, L"���b�V���C���f�b�N�X�o�b�t�@�[�쐬���s", L"�G���[", MB_OK);

			return false;
		}
	}

	delete[] pVertexType;

	if (ppMaterialIndex != NULL)
	{
		for(int i = 0; i < m_pMeshType[0].m_nMaterialCount; i++)
		{
			delete[] ppMaterialIndex[i];
		}
		delete[] ppMaterialIndex;
	}

	return true;
}

// nx�t�@�C���̒��_�����擾����֐�
void CModel::NXLoadVertex(FILE** pFile, VertexType** pVertexType, int* pVertexCount)
{
	// �t�@�C���ɓ����Ă��镶������擾����ϐ�
	wchar_t FileData[256];

	// ���_���擾
	fwscanf_s(*pFile, L"%s", FileData, 256);
	swscanf_s(FileData, L"%d", pVertexCount);

	// ���_�f�[�^��������ꕨ�쐬
	*pVertexType = new VertexType[*pVertexCount];

	fgetws(FileData, 256, *pFile);
	for (int i = 0; i < *pVertexCount; i++)
	{
		// ���_�ʒu�擾
		fgetws(FileData, 256, *pFile);
		swscanf_s(FileData, L"v %f,%f,%f",
			&(*pVertexType)[i].m_vPos.x,
			&(*pVertexType)[i].m_vPos.y,
			&(*pVertexType)[i].m_vPos.z);
	}
}

// nx�t�@�C���̖@�������擾����֐�
void CModel::NXLoadNormal(FILE** pFile, VertexType** pVertexType, int nVertexCount)
{
	// �t�@�C���ɓ����Ă��镶������擾����ϐ�
	wchar_t FileData[256];

	fwscanf_s(*pFile, L"%s", FileData, 256);

	fgetws(FileData, 256, *pFile);
	for (int i = 0; i < nVertexCount; i++)
	{
		// �@�����擾
		fgetws(FileData, 256, *pFile);
		swscanf_s(FileData, L"n %f,%f,%f",
			&(*pVertexType)[i].m_vNor.x,
			&(*pVertexType)[i].m_vNor.y,
			&(*pVertexType)[i].m_vNor.z);
	}
}

// nx�t�@�C����UV�����擾����֐�
void CModel::NXLoadUv(FILE** pFile, VertexType** pVertexType, int nVertexCount)
{
	// �t�@�C���ɓ����Ă��镶������擾����ϐ�
	wchar_t FileData[256];

	fwscanf_s(*pFile, L"%s", FileData, 256);

	fgetws(FileData, 256, *pFile);
	for (int i = 0; i < nVertexCount; i++)
	{
		// uv���擾
		fgetws(FileData, 256, *pFile);
		swscanf_s(FileData, L"uv %f,%f",
			&(*pVertexType)[i].m_vTex.x,
			&(*pVertexType)[i].m_vTex.y);
	}
}

void CModel::NXLoadMaterial(FILE** pFile, VertexType** pVertexType, int nVertexCount, int*** pppMaterialIndex)
{
	// �t�@�C���ɓ����Ă��镶������擾����ϐ�
	wchar_t FileData[256];

	// �}�e���A���̐��擾
	fwscanf_s(*pFile, L"%s", FileData, 256);
	swscanf_s(FileData, L"%d", &m_pMeshType[0].m_nMaterialCount);

	m_pMeshType[0].m_pMaterialType = new MaterialType[m_pMeshType[0].m_nMaterialCount];

	fgetws(FileData, 256, *pFile);
	for (int i = 0; i < m_pMeshType[0].m_nMaterialCount; i++)
	{
		// �e�N�X�`�����擾
		fgetws(FileData, 256, *pFile);
		wchar_t sTextureName[256];
		if (swscanf_s(FileData, L"tex %s", sTextureName, 256) != EOF)
		{
			// �e�N�X�`�����쐬
			CTextureManager* pTextureManager = CTextureManager::GetInstance();
			pTextureManager->CreateTexture(sTextureName);
			m_pMeshType[0].m_pMaterialType[i].m_pTexture = pTextureManager->GetTexture(sTextureName);
		}

		// �f�t���[�Y
		fgetws(FileData, 256, *pFile);
		swscanf_s(FileData, L"dif %f,%f,%f,%f",
			&m_pMeshType[0].m_pMaterialType[i].m_vDiffuseColor.x,
			&m_pMeshType[0].m_pMaterialType[i].m_vDiffuseColor.y,
			&m_pMeshType[0].m_pMaterialType[i].m_vDiffuseColor.z,
			&m_pMeshType[0].m_pMaterialType[i].m_vDiffuseColor.w);

		// �X�y�L�����[�p���[
		fgetws(FileData, 256, *pFile);
		swscanf_s(FileData, L"spep %f",
			&m_pMeshType[0].m_pMaterialType[i].m_fSpecularPower);

		// �X�y�L�����[�J���[
		fgetws(FileData, 256, *pFile);
		swscanf_s(FileData, L"spe %f,%f,%f",
			&m_pMeshType[0].m_pMaterialType[i].m_vSpecularColor.x,
			&m_pMeshType[0].m_pMaterialType[i].m_vSpecularColor.y,
			&m_pMeshType[0].m_pMaterialType[i].m_vSpecularColor.z);

		m_pMeshType[0].m_pMaterialType[i].m_vSpecularColor.w = 0.f;

		// �G�~�b�V�u
		fgetws(FileData, 256, *pFile);
		swscanf_s(FileData, L"emi %f,%f,%f",
			&m_pMeshType[0].m_pMaterialType[i].m_vEmission.x,
			&m_pMeshType[0].m_pMaterialType[i].m_vEmission.y,
			&m_pMeshType[0].m_pMaterialType[i].m_vEmission.z);

		m_pMeshType[0].m_pMaterialType[i].m_vEmission.z = 0.f;
	}

	// �}�e���A���C���f�b�N�X�����擾
	m_pMeshType[0].m_pMaterialIndexCount = new int[m_pMeshType[0].m_nMaterialCount];
	*pppMaterialIndex = new int*[m_pMeshType[0].m_nMaterialCount];

	fgetws(FileData, 256, *pFile);

	// �}�e���A���C���f�b�N�X�̒��g��������
	for (int i = 0; i < m_pMeshType[0].m_nMaterialCount; i++)
	{
		// �}�e���A����K�p����ʂ̐����擾
		int nMaterialSurfaceCount = 0;
		fgetws(FileData, 256, *pFile);
		swscanf_s(FileData, L"matesurfacenum %d",
			&nMaterialSurfaceCount);

		// �C���f�b�N�X��������
		m_pMeshType[0].m_pMaterialIndexCount[i] = nMaterialSurfaceCount * 3;

		(*pppMaterialIndex)[i] = new int[m_pMeshType[0].m_pMaterialIndexCount[i]];

		for (int j = 0; j < nMaterialSurfaceCount; j++)
		{
			int nMaterialIndex;
			fgetws(FileData, 256, *pFile);
			swscanf_s(FileData, L"mate %d",
				&nMaterialIndex);

			(*pppMaterialIndex)[i][j * 3 + 0] = nMaterialIndex * 3 + 0;
			(*pppMaterialIndex)[i][j * 3 + 1] = nMaterialIndex * 3 + 1;
			(*pppMaterialIndex)[i][j * 3 + 2] = nMaterialIndex * 3 + 2;
		}
	}
}

// �X�L���A�j���[�V�����v�Z
void CModel::SkinAnimation(float fFrameSecond, int nAnimationId)
{
	if (m_nBoneCount == 0) return;

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity(&matIdentity);
	RecursionAnimation(&m_pBone[0], NULL, NULL, nAnimationId, fFrameSecond);
}

// �e����q�փA�j���[�V�����v�Z������(�ċA�֐�)
void CModel::RecursionAnimation(Bone* pBone, const D3DXMATRIX* pmatOyaAni, const D3DXMATRIX* pmatOyaInv, int nAnimationId, float fFrameSecond)
{
	D3DXMATRIX matScaleOld, matRotationOld, matTranslateOld;
	D3DXMATRIX matAniamtion, matScale, matRotation, matTranslate;

	// �����l���擾
	int nFrameSecond = (int)fFrameSecond;

	// �g��k��
	D3DXMatrixScaling(&matScaleOld,
		pBone->m_pAnimationKey[nAnimationId].m_pvScale[nFrameSecond].x,
		pBone->m_pAnimationKey[nAnimationId].m_pvScale[nFrameSecond].y, 
		pBone->m_pAnimationKey[nAnimationId].m_pvScale[nFrameSecond].z);

	// ��]�s��
	matRotationOld = pBone->m_pAnimationKey[nAnimationId].m_pmatRotation[nFrameSecond];

	// ���s�ړ�
	D3DXMatrixTranslation(&matTranslateOld,
		pBone->m_pAnimationKey[nAnimationId].m_pvTranslation[nFrameSecond].x,
		pBone->m_pAnimationKey[nAnimationId].m_pvTranslation[nFrameSecond].y,
		pBone->m_pAnimationKey[nAnimationId].m_pvTranslation[nFrameSecond].z);

	if (nFrameSecond + 1 >= m_nEndFrame[nAnimationId])
	{
		D3DXMatrixScaling(&matScale,
			pBone->m_pAnimationKey[nAnimationId].m_pvScale[0].x,
			pBone->m_pAnimationKey[nAnimationId].m_pvScale[0].y,
			pBone->m_pAnimationKey[nAnimationId].m_pvScale[0].z);

		matRotation = pBone->m_pAnimationKey[nAnimationId].m_pmatRotation[0];

		D3DXMatrixTranslation(&matTranslate,
			pBone->m_pAnimationKey[nAnimationId].m_pvTranslation[0].x,
			pBone->m_pAnimationKey[nAnimationId].m_pvTranslation[0].y,
			pBone->m_pAnimationKey[nAnimationId].m_pvTranslation[0].z);
	}
	else
	{
		D3DXMatrixScaling(&matScale,
			pBone->m_pAnimationKey[nAnimationId].m_pvScale[nFrameSecond + 1].x,
			pBone->m_pAnimationKey[nAnimationId].m_pvScale[nFrameSecond + 1].y,
			pBone->m_pAnimationKey[nAnimationId].m_pvScale[nFrameSecond + 1].z);

		matRotation = pBone->m_pAnimationKey[nAnimationId].m_pmatRotation[nFrameSecond + 1];

		D3DXMatrixTranslation(&matTranslate,
			pBone->m_pAnimationKey[nAnimationId].m_pvTranslation[nFrameSecond + 1].x,
			pBone->m_pAnimationKey[nAnimationId].m_pvTranslation[nFrameSecond + 1].y,
			pBone->m_pAnimationKey[nAnimationId].m_pvTranslation[nFrameSecond + 1].z);
	}
	
	// ��Ԏ�---------------------------------
	matScale     = (1.f - (fFrameSecond - (float)nFrameSecond)) * matScaleOld	  + (fFrameSecond - (float)nFrameSecond) * matScale;
	matRotation  = (1.f - (fFrameSecond - (float)nFrameSecond)) * matRotationOld  + (fFrameSecond - (float)nFrameSecond) * matRotation;
	matTranslate = (1.f - (fFrameSecond - (float)nFrameSecond)) * matTranslateOld + (fFrameSecond - (float)nFrameSecond) * matTranslate;
	// ---------------------------------------

	// ���W�ϊ�---------------------------------------
	D3DXMatrixMultiply(&matAniamtion, &matRotation, &matScale);
	D3DXMatrixMultiply(&matAniamtion, &matAniamtion, &matTranslate);
	// -----------------------------------------------s

	// �����̃A�j���[�V�����s��v�Z
	pBone->m_matAnimation = pBone->m_matOffset * matAniamtion  * pBone->m_matInvOffset;

	// �e�{�[�����̃A�j���[�V�����𔽉f������
	for (int i = pBone->m_nBoneDepthCount - 1; i >= 0; i--)
	{
		pBone->m_matAnimation = pBone->m_matAnimation * pmatOyaAni[i] * pmatOyaInv[i];
	}

	// �q�{�[���ɓn���s��f�[�^�쐬
	pBone->m_pmatOyaAnimation = new D3DXMATRIX[pBone->m_nBoneDepthCount + 1];
	pBone->m_pmatOyaInvOffset = new D3DXMATRIX[pBone->m_nBoneDepthCount + 1];

	// �e�{�[���̃A�j���[�V�����s��Ƌt�{�[���s������q�{�[���ɓn���s��f�[�^�ɓ����
	for (int i = 0; i < pBone->m_nBoneDepthCount; i++)
	{
		pBone->m_pmatOyaAnimation[i] = pmatOyaAni[i];
		pBone->m_pmatOyaInvOffset[i] = pmatOyaInv[i];
	}

	// �q�{�[���ɓn���s��f�[�^�Ɏ����̃A�j���[�V�����s��Ƌt�{�[���s���������
	pBone->m_pmatOyaAnimation[pBone->m_nBoneDepthCount] = matAniamtion;
	pBone->m_pmatOyaInvOffset[pBone->m_nBoneDepthCount] = pBone->m_matInvOffset;

	// �q�{�[���̐������A�A�j���[�V�������𑗂��ăA�j���[�V�����s����쐬����
	for (int i = 0; i < pBone->m_nBoneChildCount; i++)
	{
		RecursionAnimation(pBone->m_ppBoneChild[i], pBone->m_pmatOyaAnimation,
			pBone->m_pmatOyaInvOffset, nAnimationId, fFrameSecond);
	}

	// �A�j���[�V�������쐬���I�����̂ō폜
	delete[] pBone->m_pmatOyaAnimation;
	pBone->m_pmatOyaAnimation = NULL;
	delete[] pBone->m_pmatOyaInvOffset;
	pBone->m_pmatOyaInvOffset = NULL;
}

// �l�p�`���쐬����(3D�`��p)
void CModel::CreateQuadrangle3D()
{
	VertexType* pVertexType = NULL;
	int* pIndex = NULL;

	m_nMeshCount = 1;
	m_pMeshType = new MeshType[m_nMeshCount];

	m_pMeshType[0].m_nMaterialCount = 1;
	m_pMeshType[0].m_pMaterialType = new MaterialType[m_pMeshType[0].m_nMaterialCount];
	m_pMeshType[0].m_pMaterialIndexCount = new int[m_pMeshType[0].m_nMaterialCount];
	m_pMeshType[0].m_ppIndexBuffer = new ID3D10Buffer*[m_pMeshType[0].m_nMaterialCount];
	m_pMeshType[0].m_pMaterialIndexCount[0] = 6;

	// �o�[�e�b�N�X���ݒ�--------------------------------------------------
	pVertexType = new VertexType[4];

	// ���_�ݒ�
	pVertexType[0].m_vPos = D3DXVECTOR3(-0.5f, 0.5f, 0.f); // ����
	pVertexType[1].m_vPos = D3DXVECTOR3(0.5f, 0.5f, 0.f); // �E��
	pVertexType[2].m_vPos = D3DXVECTOR3(-0.5f, -0.5f, 0.f); // ����
	pVertexType[3].m_vPos = D3DXVECTOR3(0.5f, -0.5f, 0.f); // �E��

	// �@���ݒ�
	for (int i = 0; i < 4; i++)
		pVertexType[i].m_vNor = D3DXVECTOR3(0, 0, -1);

	// uv�ݒ�
	pVertexType[0].m_vTex = D3DXVECTOR2(0.f, 0.f);		// ����
	pVertexType[1].m_vTex = D3DXVECTOR2(1.f, 0.f);		// �E��
	pVertexType[2].m_vTex = D3DXVECTOR2(0.f, 1.f);		// ����
	pVertexType[3].m_vTex = D3DXVECTOR2(1.f, 1.f);		// �E��
	// ----------------------------------------------------------------------

	// �C���f�b�N�X���ݒ�
	pIndex = new int[6];

	pIndex[0] = 0;
	pIndex[1] = 1;
	pIndex[2] = 2;
	pIndex[3] = 1;
	pIndex[4] = 3;
	pIndex[5] = 2;

	//�o�[�e�b�N�X�o�b�t�@�[�쐬
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VertexType) * 4;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = pVertexType;

	CDevice3D* pDevice3D = CDevice3D::GetInstance();
	ID3D10Device* pDevice = pDevice3D->GetDevice();

	// �o�[�e�b�N�X�o�b�t�@�[�쐬
	if (FAILED(pDevice->CreateBuffer(&bd, &InitData, &(m_pMeshType[0].m_pVertexBuffer))))
	{
		MessageBox(NULL, L"���b�V���o�[�e�b�N�X�o�b�t�@�[�쐬���s", L"�G���[", MB_OK);

		return;
	}

	//�C���f�b�N�X�o�b�t�@�[�쐬
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(int) * m_pMeshType[0].m_pMaterialIndexCount[0];
	bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	InitData.pSysMem = pIndex;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	if (FAILED(pDevice->CreateBuffer(&bd, &InitData, &(m_pMeshType[0].m_ppIndexBuffer[0]))))
	{
		MessageBox(NULL, L"���b�V���o�[�e�b�N�X�o�b�t�@�[�쐬���s", L"�G���[", MB_OK);

		return;
	}

	delete[] pVertexType;
	delete[] pIndex;
}

// �l�p�`���쐬����(2D�`��p)
void CModel::CreateQuadrangle2D()
{
	VertexType* pVertexType = NULL;
	int* pIndex = NULL;

	m_nMeshCount = 1;
	m_pMeshType = new MeshType[m_nMeshCount];
	
	m_pMeshType[0].m_nMaterialCount = 1;
	m_pMeshType[0].m_pMaterialType		 = new MaterialType[m_pMeshType[0].m_nMaterialCount];
	m_pMeshType[0].m_pMaterialIndexCount = new int[m_pMeshType[0].m_nMaterialCount];
	m_pMeshType[0].m_ppIndexBuffer		 = new ID3D10Buffer*[m_pMeshType[0].m_nMaterialCount];
	m_pMeshType[0].m_pMaterialIndexCount[0] = 6;

	// �o�[�e�b�N�X���ݒ�--------------------------------------------------
	pVertexType = new VertexType[4];
	
	// ���_�ݒ�
	pVertexType[0].m_vPos = D3DXVECTOR3(-0.5f, -0.5f, 0.f); // ����
	pVertexType[1].m_vPos = D3DXVECTOR3( 0.5f, -0.5f, 0.f); // �E��
	pVertexType[2].m_vPos = D3DXVECTOR3(-0.5f,  0.5f, 0.f); // ����
	pVertexType[3].m_vPos = D3DXVECTOR3( 0.5f,  0.5f, 0.f); // �E��

	// �@���ݒ�
	for (int i = 0; i < 4; i++)
		pVertexType[i].m_vNor = D3DXVECTOR3(0, 0, -1);

	// uv�ݒ�
	pVertexType[0].m_vTex = D3DXVECTOR2( 0.f, 0.f);		// ����
	pVertexType[1].m_vTex = D3DXVECTOR2( 1.f, 0.f);		// �E��
	pVertexType[2].m_vTex = D3DXVECTOR2( 0.f, 1.f);		// ����
	pVertexType[3].m_vTex = D3DXVECTOR2( 1.f, 1.f);		// �E��
	// ----------------------------------------------------------------------

	// �C���f�b�N�X���ݒ�
	pIndex = new int[6];

	pIndex[0] = 0;
	pIndex[1] = 1;
	pIndex[2] = 2;
	pIndex[3] = 1;
	pIndex[4] = 3;
	pIndex[5] = 2;
	
	//�o�[�e�b�N�X�o�b�t�@�[�쐬
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VertexType) * 4;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = pVertexType;

	CDevice3D* pDevice3D = CDevice3D::GetInstance();
	ID3D10Device* pDevice = pDevice3D->GetDevice();

	// �o�[�e�b�N�X�o�b�t�@�[�쐬
	if (FAILED(pDevice->CreateBuffer(&bd, &InitData, &(m_pMeshType[0].m_pVertexBuffer))))
	{
		MessageBox(NULL, L"���b�V���o�[�e�b�N�X�o�b�t�@�[�쐬���s", L"�G���[", MB_OK);

		return;
	}

	//�C���f�b�N�X�o�b�t�@�[�쐬
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(int) * m_pMeshType[0].m_pMaterialIndexCount[0];
	bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	InitData.pSysMem = pIndex;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	if (FAILED(pDevice->CreateBuffer(&bd, &InitData, &(m_pMeshType[0].m_ppIndexBuffer[0]))))
	{
		MessageBox(NULL, L"���b�V���o�[�e�b�N�X�o�b�t�@�[�쐬���s", L"�G���[", MB_OK);

		return;
	}

	delete[] pVertexType;
	delete[] pIndex;
}

// �_��񂩂烂�f�����쐬����
void CModel::PointToCreateModel(const D3DXVECTOR3* pPoints, int PointNum)
{
	VertexType* pVertexType = NULL;
	int* pIndex = NULL;

	m_nMeshCount = 1;
	m_pMeshType = new MeshType[m_nMeshCount];

	m_pMeshType[0].m_nMaterialCount = 1;
	m_pMeshType[0].m_pMaterialType = new MaterialType[m_pMeshType[0].m_nMaterialCount];
	m_pMeshType[0].m_pMaterialIndexCount = new int[m_pMeshType[0].m_nMaterialCount];
	m_pMeshType[0].m_ppIndexBuffer = new ID3D10Buffer*[m_pMeshType[0].m_nMaterialCount];
	m_pMeshType[0].m_pMaterialIndexCount[0] = PointNum;

	// �o�[�e�b�N�X���ݒ�--------------------------------------------------
	pVertexType = new VertexType[PointNum];

	for (int i = 0; i < PointNum; i++)
	{
		// ���_�ݒ�
		pVertexType[i].m_vPos = pPoints[i]; // ����
	}

	// �@���ݒ�
	for (int i = 0; i < PointNum; i++)
		pVertexType[i].m_vNor = D3DXVECTOR3(0, 0, -1);

	for (int i = 0; i < PointNum; i++)
	{
		pVertexType[i].m_vTex = D3DXVECTOR2(0.f, 0.f);
	}
	// ----------------------------------------------------------------------

	// �C���f�b�N�X���ݒ�
	pIndex = new int[PointNum];

	for (int i = 0; i < PointNum; i++)
	{
		pIndex[i] = i;
	}

	//�o�[�e�b�N�X�o�b�t�@�[�쐬
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VertexType) * PointNum;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = pVertexType;

	CDevice3D* pDevice3D = CDevice3D::GetInstance();
	ID3D10Device* pDevice = pDevice3D->GetDevice();

	// �o�[�e�b�N�X�o�b�t�@�[�쐬
	if (FAILED(pDevice->CreateBuffer(&bd, &InitData, &(m_pMeshType[0].m_pVertexBuffer))))
	{
		MessageBox(NULL, L"���b�V���o�[�e�b�N�X�o�b�t�@�[�쐬���s", L"�G���[", MB_OK);

		return;
	}

	//�C���f�b�N�X�o�b�t�@�[�쐬
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(int) * m_pMeshType[0].m_pMaterialIndexCount[0];
	bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	InitData.pSysMem = pIndex;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	if (FAILED(pDevice->CreateBuffer(&bd, &InitData, &(m_pMeshType[0].m_ppIndexBuffer[0]))))
	{
		MessageBox(NULL, L"���b�V���o�[�e�b�N�X�o�b�t�@�[�쐬���s", L"�G���[", MB_OK);

		return;
	}

	delete[] pVertexType;
	delete[] pIndex;
}

// nx�t�@�C���̃A�j���[�V������ǂݍ���
bool CModel::LoadNXAnimation(const wchar_t* pFileName, int id)
{
	FILE* pFile;
	wchar_t FileData[256];

	// �t�@�C���I�[�v��
	if (_wfopen_s(&pFile, pFileName, L"r") != 0)
	{
		wchar_t ErrorString[256];
		swprintf_s(ErrorString, 256, L"%s��������܂���ł����B",
			pFileName);
		MessageBox(NULL, ErrorString, L"�G���[", MB_OK);
		return false;
	}

	fwscanf_s(pFile, L"%s", FileData, 256);

	// �A�j���[�V�����擾
	if (wcscmp(FileData, L"framenum") == 0)
	{
		fwscanf_s(pFile, L"%s", FileData, 256);
		// �A�j���[�V�����ő�t���[�����擾
		swscanf_s(FileData, L"%d", &m_nEndFrame[id]);

		for (int i = 0; i < m_nBoneCount; i++)
		{			
			m_pBone[i].m_pAnimationKey[id].m_pmatRotation  = new D3DXMATRIX[m_nEndFrame[id]];
			m_pBone[i].m_pAnimationKey[id].m_pvScale		 = new D3DXVECTOR3[m_nEndFrame[id]];
			m_pBone[i].m_pAnimationKey[id].m_pvTranslation = new D3DXVECTOR3[m_nEndFrame[id]];

			fgetws(FileData, 256, pFile);
			// �A�j���[�V�����t���[���ő吔�܂ŃA�j���[�V�������擾
			for (int j = 0; j < m_nEndFrame[id]; j++)
			{
				fgetws(FileData, 256, pFile);

				// ��]���擾
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"rota %f,%f,%f,%f",
					&m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._11, &m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._12,
					&m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._13, &m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._14);
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"rota %f,%f,%f,%f",
					&m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._21, &m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._22,
					&m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._23, &m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._24);
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"rota %f,%f,%f,%f",
					&m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._31, &m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._32,
					&m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._33, &m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._34);
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"rota %f,%f,%f,%f",
					&m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._41, &m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._42,
					&m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._43, &m_pBone[i].m_pAnimationKey[id].m_pmatRotation[j]._44);

				// �g��k�����擾
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"scale %f,%f,%f",
					&m_pBone[i].m_pAnimationKey[id].m_pvScale[j].x, &m_pBone[i].m_pAnimationKey[id].m_pvScale[j].y,
					&m_pBone[i].m_pAnimationKey[id].m_pvScale[j].z);

				// ���s�ړ����擾
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"trans %f,%f,%f",
					&m_pBone[i].m_pAnimationKey[id].m_pvTranslation[j].x, &m_pBone[i].m_pAnimationKey[id].m_pvTranslation[j].y,
					&m_pBone[i].m_pAnimationKey[id].m_pvTranslation[j].z);
			}
		}
	}

	return true;
}

// �E�F�C�g�l���\�[�g����
void WeightInsertionSort(float* p_fSortWeight, int* p_nSortOffsetId, int nElementCount)
{
	for (int i = 0; i < nElementCount; i++)
	{
		for (int j = 0; j < i; j++)
		{
			if (p_fSortWeight[i] > p_fSortWeight[j])
			{
				// ����ւ�
				float tempWeight;
				tempWeight = p_fSortWeight[i];
				p_fSortWeight[i] = p_fSortWeight[j];
				p_fSortWeight[j] = tempWeight;

				int tempOffsetId;
				tempOffsetId = p_nSortOffsetId[i];
				p_nSortOffsetId[i] = p_nSortOffsetId[j];
				p_nSortOffsetId[j] = tempOffsetId;
			}
		}
	}
}
