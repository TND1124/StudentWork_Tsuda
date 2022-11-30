#include <stdio.h>
#include <conio.h>
#include <wchar.h>
#include <math.h>

#include "Source.h"
#include "math.h"

#define DELETE_ARRAY(p) { if(p != NULL) delete[] p; }

bool LoadXFile(wchar_t* sFileName);
bool CreateNXFile(wchar_t* sFileName);
bool CreateNXAnimationFile(wchar_t* sFileName);
void MemoryRelease();

// XN�t�@�C�������̂ɕK�v�ȕ�----------------------------------------------
float* g_pVertex		 = NULL; // ���_�ʒu�f�[�^
float* g_pTex			 = NULL; // uv�f�[�^
float* g_pNormal		 = NULL; // �@���f�[�^
float* g_pVertexColor	 = NULL; // ���_�J���[�f�[�^
int* g_pIndexVertex		 = NULL; // ���_�C���f�b�N�X�f�[�^
int* g_pIndexTex		 = NULL; // uv�C���f�b�N�X�f�[�^
int* g_pIndexNormal		 = NULL; // �@���C���f�b�N�X�f�[�^
int* g_pIndexVertexColor = NULL; // ���_�J���[�C���f�b�N�X�f�[�^
int** g_ppIndexMaterial  = NULL; // �}�e���A�����̃C���f�b�N�X���
// --------------------------------------------------------------------------

bool g_isRoot = false;			 // Root���擾�t���O(Root�������ςȂƂ���ɂ������ꍇ�̑΍�)

Matrix4x4 g_RootMatrix4x4;		 // Root�s���������ꕨ
Matrix4x4 g_matArmature4x4;		 // Armature�s���������ꕨ

int g_nVertexCount = 0;			 // ���_��
int g_nTexCount = 0;			 // uv��
int g_nNormalCount = 0;			 // �@����
int g_nVertexColorCount = 0;	 // ���_�J���[��
int g_nIndexCount = 0;			 // �C���f�b�N�X��

// �{�[�����--------------------------------
wchar_t** g_ppBoneName;					// �{�[����(x�t�@�C���ł̓{�[�����̏��Ԃ�����������������̂ł��̑΍��p)
int* g_pBoneChild = NULL;				// �{�[���̎q���̐�
int g_nBoneCount = 0;					// �{�[����
int* g_pSkinWeightsCount = NULL;		// �E�F�C�g��
float** g_ppSkinWeights	 = NULL;		// �E�F�C�g�l
int** g_ppSkinWeightsIndex = NULL;		// �E�F�C�g�C���f�b�N�X���
Matrix4x4* g_pBoneMatrix4x4 = NULL;		// �{�[���I�t�Z�b�g�l
D3DXMATRIX* g_pBoneInvMatrix4x4 = NULL;	// �t�{�[���I�t�Z�b�g�l(�t�s��)
int* g_pBoneDepth = NULL;				// �e��0�A�q�̐[���̕�����������
// -----------------------------------------

// �A�j���[�V�������------------------------
AnimationKey* g_pAnimationKey = NULL;
// ------------------------------------------

// �}�e���A�������p�ϐ�------------------------------
Material* g_pMaterial = NULL;			// �}�e���A�����
int g_nMaterialCount	= 0;			// �}�e���A���̑���
int* g_pMaterialNumber = NULL;			// �ʂ̐������}�e���A���ԍ���������ꕨ	 
int* g_pMaterialSurfaceNum;				// ���ꂼ��̃}�e���A���𔽉f������ʂ̐�
int* g_pTriangleSurfaceNumber = NULL;	// �ʏ��̎l�p�`���O�p�`�ɂ���
int g_nTriangleSurfaceCount = 0;		// �O�p�`�̖ʂ̐�(�l�p�`���O�p�`�ɂ���Ƃ��ɕK�v)
int g_nSurfaceCount = 0;				// �ʂ̐�(�l�p�`���O�p�`�ɂ���Ƃ��ɕK�v)
int g_nInitIndex = 0;					// �C���f�b�N�X�������l(�l�p�`���O�p�`�ɂ���Ƃ��ɕK�v)
// ----------------------------------------------------

// OBB�����������ޗp�ϐ�--------------------------------------------------------
Vector3D object_scale[2] = { Vector3D(0.f, 0.f, 0.f), Vector3D(0.f, 0.f, 0.f) }; // �I�u�W�F�N�g�̑傫�� [0]�ő�l [1]�ŏ��l
Vector3D obb_scale = { 0.f, 0.f, 0.f};	   // OBB�傫��
Vector3D obb_position = { 0.f, 0.f, 0.f};  // OBB�̈ʒu���
// -------------------------------------------------------------------------------

int main()
{
	wchar_t sFileName[256];

	printf_s("�ϊ�����X�t�@�C������͂��Ă�������\n");
	wscanf_s(L"%s", sFileName, 256);

	// X�t�@�C����ǂݍ���
	if (LoadXFile(sFileName) == false)
	{
		MemoryRelease();
		return -1;
	}

	// x�t�@�C������x�̕�����nx�ɕς���
	for (int i = 0; i < 256; i++)
	{
		if (sFileName[i] == L'\0')
		{
			sFileName[i - 1] = L'n';
			sFileName[i] = L'x';
			sFileName[i + 1] = L'\0';
			break;
		}
	}

	// nx�t�@�C�����쐬
	if (CreateNXFile(sFileName) == false)
	{
		MemoryRelease();
		return -1;
	}

	// ���������
	MemoryRelease();

	printf_s("�ϊ����I�����܂����B\n");

	_getch();

	return 0;
}

bool LoadXFile(wchar_t* sFileName)
{
	FILE* pFile;  // �ǂݍ��ރt�@�C��
	wchar_t FileData[256];

	g_RootMatrix4x4.Identity();

	// �ǂݍ��݃t�@�C�����J��
	if (_wfopen_s(&pFile, sFileName, L"r") != 0)
	{
		// �t�@�C����������Ȃ������ꍇ
		wchar_t ErrorString[256];
		swprintf_s(ErrorString, 256, L"%s��������܂���ł����B", sFileName);
		wprintf_s(ErrorString);
		return false;
	}

	while (fwscanf_s(pFile, L"%s", FileData, 256) != EOF)
	{
		// Blender�p�ϊ��s��擾
		if (wcscmp(FileData, L"Root") == 0)
		{
			if (g_isRoot == true) continue; // End of Root�����
			fwscanf_s(pFile, L"%s", FileData, 256);
			fwscanf_s(pFile, L"%s", FileData, 256);
			fwscanf_s(pFile, L"%s", FileData, 256);
			fgetws(FileData, 256, pFile);

			// �s����擾
			for (int i = 0; i < 4; i++)
			{
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"%f,%f,%f,%f",
					(&g_RootMatrix4x4._11) + i * 4, 
					(&g_RootMatrix4x4._12) + i * 4, 
					(&g_RootMatrix4x4._13) + i * 4,
					(&g_RootMatrix4x4._14) + i * 4);
			}

			g_isRoot = true;

			fgetws(FileData, 256, pFile);
			fgetws(FileData, 256, pFile);
			fgetws(FileData, 256, pFile);

			// �s����擾
			for (int i = 0; i < 4; i++)
			{
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"%f,%f,%f,%f",
					(&g_matArmature4x4._11) + i * 4,
					(&g_matArmature4x4._12) + i * 4,
					(&g_matArmature4x4._13) + i * 4,
					(&g_matArmature4x4._14) + i * 4);
			}
		}
		// ���_�ǂݍ���
		else if (wcscmp(FileData, L"Mesh") == 0)
		{
			fgetws(FileData, 256, pFile);
			// ���_�����擾
			fwscanf_s(pFile, L"%s", FileData, 256);

			if (FileData[0] == L'<') continue; //template�͏Ȃ�

			swscanf_s(FileData, L"%d;", &g_nVertexCount);
			// ���_�̐��������_�������锠�����
			g_pVertex = new float[g_nVertexCount * 3];

			fgetws(FileData, 256, pFile);
			// ���_���擾
			for (int i = 0; i < g_nVertexCount; i++)
			{
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"%f;%f;%f",
					&g_pVertex[i * 3 + 0],
					&g_pVertex[i * 3 + 1],
					&g_pVertex[i * 3 + 2]);

				// ���_�̈ʒu�𐳂����ʒu�ɕϊ�(blender����)
				Vector3D vertex_vec = Vector3D(g_pVertex[i * 3 + 0], g_pVertex[i * 3 + 1], g_pVertex[i * 3 + 2]);
				vertex_vec = g_RootMatrix4x4.TransformPointVector3D(vertex_vec);
				g_pVertex[i * 3 + 0] = vertex_vec.x;
				g_pVertex[i * 3 + 1] = vertex_vec.y;
				g_pVertex[i * 3 + 2] = vertex_vec.z;

				// ���_�̍ő�l�ƍŒ�l�𒲂ׂ�(OBB�ł̓����蔻����ɒ��ׂ�)
				if (object_scale[0].x < vertex_vec.x)
				{
					object_scale[0].x = vertex_vec.x;
				}
				if (object_scale[1].x > vertex_vec.x)
				{
					object_scale[1].x = vertex_vec.x;
				}
				if (object_scale[0].y < vertex_vec.y)
				{
					object_scale[0].y = vertex_vec.y;
				}
				if (object_scale[1].y > vertex_vec.y)
				{
					object_scale[1].y = vertex_vec.y;
				}
				if (object_scale[0].z < vertex_vec.z)
				{
					object_scale[0].z = vertex_vec.z;
				}
				if (object_scale[1].z > vertex_vec.z)
				{
					object_scale[1].z = vertex_vec.z;
				}
			}

			// ���_�C���f�b�N�X�����擾
			fwscanf_s(pFile, L"%s", FileData, 256);
			swscanf_s(FileData, L"%d;", &g_nIndexCount);

			g_nInitIndex = g_nIndexCount; // �C���f�b�N�X�����l��ۑ�

			// ���_�̐��������_�������锠�����
			g_pIndexVertex = new int[g_nIndexCount * 2 * 3];

			g_pTriangleSurfaceNumber = new int[g_nIndexCount * 2];

			// ���_�C���f�b�N�X���擾
			for (int i = 0, count = 0; i < g_nIndexCount; i++, count++)
			{
				fwscanf_s(pFile, L"%s", FileData, 256);
				if (swscanf_s(FileData, L"3;%d,%d,%d;,",
					&g_pIndexVertex[i * 3 + 0],
					&g_pIndexVertex[i * 3 + 1],
					&g_pIndexVertex[i * 3 + 2])
					== 0)
				{
					// �l�p�`���������̂ŎO�p�`�ɕϊ�
					int index1, index2, index3, index4;
					swscanf_s(FileData, L"4;%d,%d,%d,%d;,",
						&index1, &index2, &index3, &index4);
					
					g_pIndexVertex[i * 3]			= index1;
					g_pIndexVertex[i * 3 + 1]		= index2;
					g_pIndexVertex[i * 3 + 2]		= index3;

					g_pIndexVertex[(i + 1) * 3]		= index3;
					g_pIndexVertex[(i + 1) * 3 + 1] = index4;
					g_pIndexVertex[(i + 1) * 3 + 2] = index1;

					g_nIndexCount++;
					i++;

					//-----------------------------------------------------------
					// �}�e���A���K�p�ʔԍ������蓖�Ă�
					g_pTriangleSurfaceNumber[g_nTriangleSurfaceCount] = count;
					g_nTriangleSurfaceCount++;
					//-----------------------------------------------------------
				}

				//-----------------------------------------------------------
				// �}�e���A���K�p�ʔԍ������蓖�Ă�
				g_pTriangleSurfaceNumber[g_nTriangleSurfaceCount] = count;
				g_nTriangleSurfaceCount++;
				//-----------------------------------------------------------
			}

		}
		// ���_�J���[�ǂݍ���
		else if (wcscmp(FileData, L"MeshVertexColors") == 0)
		{
			fgetws(FileData, 256, pFile);
			// ���_�J���[�����擾
			fwscanf_s(pFile, L"%s", FileData, 256);

			if (FileData[0] == L'<') continue; //template�͏Ȃ�

			swscanf_s(FileData, L"%d;", &g_nVertexColorCount);
			// ���_�J���[�̐��������_�J���[�������锠�����
			g_pVertexColor = new float[g_nVertexColorCount * 4];

			fgetws(FileData, 256, pFile);
			// ���_�J���[���擾
			for (int i = 0; i < g_nVertexColorCount; i++)
			{
				// �_�~�[�p�ϐ�
				int none;

				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"%d; %f; %f; %f; %f;;,",
					&none,
					&g_pVertexColor[i * 4 + 0],
					&g_pVertexColor[i * 4 + 1],
					&g_pVertexColor[i * 4 + 2],
					&g_pVertexColor[i * 4 + 3]);
			}

			fwscanf_s(pFile, L"%s", FileData, 256);
			// ���_�J���[�̐��������_�J���[�����锠�����
			g_pIndexVertexColor = new int[g_nIndexCount * 3];

			fgetws(FileData, 256, pFile);
			// ���_�J���[�C���f�b�N�X�쐬
			// ���_�J���[�C���f�b�N�X�͒��_�C���f�b�N�X�Ɠ����ɂ���
			memcpy_s(g_pIndexVertexColor, sizeof(int) * g_nIndexCount * 3,
				g_pIndexVertex, sizeof(int) * g_nIndexCount * 3);
		}
		// �@���ǂݍ���
		else if (wcscmp(FileData, L"MeshNormals") == 0)
		{
			fgetws(FileData, 256, pFile);
			// �@�������擾
			fwscanf_s(pFile, L"%s", FileData, 256);

			if (FileData[0] == L'<') continue; //template�͏Ȃ�

			swscanf_s(FileData, L"%d;", &g_nNormalCount);
			// �@���̐������@���������锠�����
			g_pNormal = new float[g_nNormalCount * 3];

			fgetws(FileData, 256, pFile);
			// �@�����擾
			for (int i = 0; i < g_nNormalCount; i++)
			{
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"%f;%f;%f",
					&g_pNormal[i * 3 + 0], &g_pNormal[i * 3 + 1], &g_pNormal[i * 3 + 2]);

				// �@���̌����𐳂��������ɕϊ�(blender��p)
				Vector3D normal_vec = Vector3D(g_pNormal[i * 3 + 0], g_pNormal[i * 3 + 1], g_pNormal[i * 3 + 2]);
				normal_vec = g_RootMatrix4x4.TransformNormalVector3D(normal_vec);

				g_pNormal[i * 3 + 0] = normal_vec.x;
				g_pNormal[i * 3 + 1] = normal_vec.y;
				g_pNormal[i * 3 + 2] = normal_vec.z;
			}
			
			fwscanf_s(pFile, L"%s", FileData, 256);
			// �@���C���f�b�N�X�̐������C���f�b�N�X�������锠�����
			g_pIndexNormal = new int[g_nIndexCount * 3];

			fgetws(FileData, 256, pFile);
			// �@���C���f�b�N�X���擾
			for (int i = 0; i < g_nIndexCount; i++)
			{
				fwscanf_s(pFile, L"%s", FileData, 256);
				if (swscanf_s(FileData, L"3;%d,%d,%d;,",
					&g_pIndexNormal[i * 3],
					&g_pIndexNormal[i * 3 + 1],
					&g_pIndexNormal[i * 3 + 2])
					== 0)
				{
					// �l�p�`���������̂ŎO�p�`�ɕϊ�
					int index1, index2, index3, index4;
					swscanf_s(FileData, L"4;%d,%d,%d,%d;,",
						&index1, &index2, &index3, &index4);

					g_pIndexNormal[i * 3]		    = index1;
					g_pIndexNormal[i * 3 + 1]		= index2;
					g_pIndexNormal[i * 3 + 2]		= index3;

					g_pIndexNormal[(i + 1) * 3]		= index3;
					g_pIndexNormal[(i + 1) * 3 + 1] = index4;
					g_pIndexNormal[(i + 1) * 3 + 2] = index1;

					i++;
				}
			}
		}
		// uv�ǂݍ���
		else if (wcscmp(FileData, L"MeshTextureCoords") == 0)
		{
			fgetws(FileData, 256, pFile);
			fwscanf_s(pFile, L"%s", FileData, 256);

			if (FileData[0] == L'<') continue; //template�͏Ȃ�

			swscanf_s(FileData, L"%d;", &g_nTexCount);

			// uv�̐�����uv�������锠�����
			g_pTex = new float[g_nTexCount * 2];

			fgetws(FileData, 256, pFile);
			// uv���擾
			for (int i = 0; i < g_nTexCount; i++)
			{
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"%f; %f;,",
					&g_pTex[i * 2 + 0], &g_pTex[i * 2 + 1]);
			}

			fwscanf_s(pFile, L"%s", FileData, 256);
			// �C���f�b�N�X�̐�����uv�����锠�����
			g_pIndexTex = new int[g_nIndexCount * 3];

			fgetws(FileData, 256, pFile);
			// uv�C���f�b�N�X�쐬
			// uv�C���f�b�N�X�͒��_�C���f�b�N�X�Ɠ����ɂ���
			memcpy_s(g_pIndexTex, sizeof(int) * g_nIndexCount * 3,
				g_pIndexVertex, sizeof(int) * g_nIndexCount * 3);
		}
		// �}�e���A�����擾
		else if (wcscmp(FileData, L"MeshMaterialList") == 0)
		{
			fgetws(FileData, 256, pFile);
			fwscanf_s(pFile, L"%s", FileData, 256);

			if (FileData[0] == L'<') continue; //template�͏Ȃ�

			swscanf_s(FileData, L"%d;", &g_nMaterialCount);
			fwscanf_s(pFile, L"%s", FileData, 256);
			swscanf_s(FileData, L"%d;", &g_nSurfaceCount);

			g_pMaterialNumber = new int[g_nTriangleSurfaceCount];

			for (int i = 0; i < g_nSurfaceCount; i++)
			{
				fwscanf_s(pFile, L"%s", FileData, 256);
				swscanf_s(FileData, L"%d", &g_pMaterialNumber[i]);
			}
			fwscanf_s(pFile, L"%s", FileData, 256);

			g_pMaterial = new Material[g_nMaterialCount];

			for (int i = 0; i < g_nMaterialCount; i++)
			{
				fgetws(FileData, 256, pFile);
				// �g�U��
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"%f; %f; %f; %f;;",
					&(g_pMaterial[i].vDiffuseColor[0]),
					&(g_pMaterial[i].vDiffuseColor[1]),
					&(g_pMaterial[i].vDiffuseColor[2]),
					&(g_pMaterial[i].vDiffuseColor[3]));

				// ���ʔ��ˌ��̋���
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"%f;",
					&(g_pMaterial[i].fSpecularPower));

				// ���ʔ���
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"%f; %f; %f;;",
					&(g_pMaterial[i].vSpecularColor[0]),
					&(g_pMaterial[i].vSpecularColor[1]),
					&(g_pMaterial[i].vSpecularColor[2]));

				// ����
				fgetws(FileData, 256, pFile);
				swscanf_s(FileData, L"%f; %f; %f;;",
					&(g_pMaterial[i].vEmission[0]),
					&(g_pMaterial[i].vEmission[1]),
					&(g_pMaterial[i].vEmission[2]));

				// �e�N�X�`����
				wchar_t tempTextureName[256];
				fwscanf_s(pFile, L"%s", FileData, 256);
				fwscanf_s(pFile, L"%s", FileData, 256);
				if (swscanf_s(FileData, L"{%s;}", tempTextureName, 256) == 0)
				{
					// �e�N�X�`�����Ȃ�
					g_pMaterial[i].sTextureName[0] = L'\0';
				}
				else
				{
					// blender�ȊO�ō쐬����X�t�@�C���̏ꍇ
					if (wcscmp(FileData, L"{") == 0)
					{
						fwscanf_s(pFile, L"%s", FileData, 256);
						swscanf_s(FileData, L"%s", tempTextureName, 256);

						// �e�N�X�`���������o��-------------------------------
						// ������̒������擾
						int nLenght = 0;
						for (int j = 0; j < 256; j++)
						{
							if (tempTextureName[j] == L'\0') break;
							nLenght++;
						}

						// �u"�v���u\0�v�ɂ��� ��( {"tex.jpg";} �� {"tex.jpg\0 )
						for (int j = nLenght - 1; j >= 0; j--)
						{
							if (tempTextureName[j] == L'"')
							{
								tempTextureName[j] = L'\0';
								break;
							}
						}

						// �u"�v���獶�̕�������菜��   ��( {"tex.jpg\0        �� tex.jpg\0 )
						// �u\\�v���獶�̕�������菜��  ��( {"Asset\\tex.jpg\0 �� tex.jpg\0 )
						for (int j = nLenght - 1; j >= 0; j--)
						{
							if (tempTextureName[j] == L'"' || 
								tempTextureName[j] == L'\\')
							{
								memcpy_s(g_pMaterial[i].sTextureName, 256, &tempTextureName[j + 1], 256);
								break;
							}
						}

						// -----------------------------------------------------
						fgetws(FileData, 256, pFile);
						fgetws(FileData, 256, pFile);
						fgetws(FileData, 256, pFile);
						fgetws(FileData, 256, pFile);
					}
					// blender�ō쐬����X�t�@�C���̏ꍇ
					else
					{
						// �e�N�X�`��������
						memcpy_s(g_pMaterial[i].sTextureName, 256, &tempTextureName[1], 256);
						for (int j = 0; j < 256; j++)
						{
							if (g_pMaterial[i].sTextureName[j] == L'"')
							{
								g_pMaterial[i].sTextureName[j] = L'\0';
								break;
							}
						}
						fgetws(FileData, 256, pFile);
						fgetws(FileData, 256, pFile);
					}
				}
			}
		}
		// �{�[�����擾
		else if (wcscmp(FileData, L"XSkinMeshHeader") == 0)
		{
			fgetws(FileData, 256, pFile);
			fwscanf_s(pFile, L"%s", FileData, 256);

			if (FileData[0] == L'<') continue; //template�͏Ȃ�

			fwscanf_s(pFile, L"%s", FileData, 256);
			fwscanf_s(pFile, L"%s", FileData, 256);
			swscanf_s(FileData, L"%d;", &g_nBoneCount);

			// �f�[�^������ꏊ�����
			g_ppBoneName = new wchar_t*[g_nBoneCount];
			g_ppSkinWeightsIndex = new int*[g_nBoneCount];
			g_ppSkinWeights = new float*[g_nBoneCount];
			g_pSkinWeightsCount = new int[g_nBoneCount];
			g_pBoneMatrix4x4 = new Matrix4x4[g_nBoneCount];

			fwscanf_s(pFile, L"%s", FileData, 256);

			for (int i = 0; i < g_nBoneCount; i++)
			{
				fwscanf_s(pFile, L"%s", FileData, 256);
				fwscanf_s(pFile, L"%s", FileData, 256);
				
				// �{�[�����擾
				fwscanf_s(pFile, L"%s", FileData, 256);
				g_ppBoneName[i] = new wchar_t[256];
				swscanf_s(FileData, L"%s;", g_ppBoneName[i], 256);
				// "�{�[����;" ���� �{�[�����ɕ��������ς���
				g_ppBoneName[i] = &g_ppBoneName[i][1];
				int len = wcslen(g_ppBoneName[i]);
				g_ppBoneName[i][len - 2] = L'\0';

				// �E�F�C�g�̐��擾
				fwscanf_s(pFile, L"%s", FileData, 256);
				swscanf_s(FileData, L"%d;", &g_pSkinWeightsCount[i]);

				// �E�F�C�g�l���擾
				g_ppSkinWeightsIndex[i] = new int[g_pSkinWeightsCount[i]];
				g_ppSkinWeights[i] = new float[g_nVertexCount];
				memset(g_ppSkinWeights[i], 0x00, sizeof(float) * g_nVertexCount);

				// �{�[���C���f�b�N�X�擾
				for (int j = 0; j < g_pSkinWeightsCount[i]; j++)
				{
					fwscanf_s(pFile, L"%s", FileData, 256);
					swscanf_s(FileData, L"%d,", &g_ppSkinWeightsIndex[i][j]);
				}

				// �{�[���E�F�C�g�l�擾
				for (int j = 0; j < g_pSkinWeightsCount[i]; j++)
				{
					fwscanf_s(pFile, L"%s", FileData, 256);
					swscanf_s(FileData, L"%f,", &g_ppSkinWeights[i][g_ppSkinWeightsIndex[i][j]]);
				}

				fgetws(FileData, 256, pFile);

				// blender�ō쐬����X�t�@�C���̏ꍇ
				if (g_isRoot == true)
				{
					// �{�[���I�t�Z�b�g�s��擾
					for (int j = 0; j < 4; j++)
					{
						fgetws(FileData, 256, pFile);
						swscanf_s(FileData, L"%f,%f,%f,%f",
							(&g_pBoneMatrix4x4[i]._11) + j * 4,
							(&g_pBoneMatrix4x4[i]._12) + j * 4,
							(&g_pBoneMatrix4x4[i]._13) + j * 4,
							(&g_pBoneMatrix4x4[i]._14) + j * 4);
					}
				}

				fgetws(FileData, 256, pFile);
			}
		}
		// �A�j���[�V�����L�[�擾
		else if (wcscmp(FileData, L"AnimationSet") == 0)
		{
			fgetws(FileData, 256, pFile);
			if (fgetws(FileData, 256, pFile) == NULL) continue;
			fgetws(FileData, 256, pFile);

			g_pAnimationKey = new AnimationKey[g_nBoneCount];

			int nBoneCount = 0;

			while (1)
			{
				while (1)
				{
					if (wcscmp(FileData, L"Animation") == 0) break;
					fwscanf_s(pFile, L"%s", FileData, 256);
				}

				fgetws(FileData, 256, pFile);
				fgetws(FileData, 256, pFile);

				for (int i = 0; i < g_nBoneCount; i++)
				{
					// ����ւ�
					wchar_t* psframe;
					wchar_t* ctx;
					psframe = wcstok_s(FileData, L" {}", &ctx);

					if (wcscmp(g_ppBoneName[i], psframe) == 0)
					{
						fgetws(FileData, 256, pFile);
						fgetws(FileData, 256, pFile);
						fgetws(FileData, 256, pFile);

						swscanf_s(FileData, L"%d;", &g_pAnimationKey[i].nEndFrame);
						// �ϊ��s����A�j���[�V�����t���[���������������ꕨ�����
						g_pAnimationKey[i].pWorld = new World[g_pAnimationKey[i].nEndFrame];

						// ��]���擾
						for (int j = 0; j < g_pAnimationKey[i].nEndFrame; j++)
						{
							int none1, none2;
							fgetws(FileData, 256, pFile);
							swscanf_s(FileData, L"%d;%d;%f,%f,%f,%f;;",
								&none1, &none2,
								&g_pAnimationKey[i].pWorld[j].qRotation[3],
								&g_pAnimationKey[i].pWorld[j].qRotation[0],
								&g_pAnimationKey[i].pWorld[j].qRotation[1],
								&g_pAnimationKey[i].pWorld[j].qRotation[2]);

							// z�l�Ŕ��]������
							D3DXQUATERNION qRotation;
							qRotation.x = g_pAnimationKey[i].pWorld[j].qRotation[0];
							qRotation.y = g_pAnimationKey[i].pWorld[j].qRotation[1];
							qRotation.z = -g_pAnimationKey[i].pWorld[j].qRotation[2];
							qRotation.w = g_pAnimationKey[i].pWorld[j].qRotation[3];

							// �N�H�[�^�j�I�������]�s��ɕϊ�
							D3DXMatrixRotationQuaternion(&g_pAnimationKey[i].pWorld[j].matRotation, &qRotation);
						}

						fgetws(FileData, 256, pFile);
						fgetws(FileData, 256, pFile);
						fgetws(FileData, 256, pFile);
						fgetws(FileData, 256, pFile);

						// �g��k�����
						for (int j = 0; j < g_pAnimationKey[i].nEndFrame; j++)
						{
							int none1, none2;
							fgetws(FileData, 256, pFile);
							swscanf_s(FileData, L"%d;%d;%f,%f,%f;;",
								&none1, &none2,
								&g_pAnimationKey[i].pWorld[j].vScale[0],
								&g_pAnimationKey[i].pWorld[j].vScale[1],
								&g_pAnimationKey[i].pWorld[j].vScale[2]);
						}

						fgetws(FileData, 256, pFile);
						fgetws(FileData, 256, pFile);
						fgetws(FileData, 256, pFile);
						fgetws(FileData, 256, pFile);

						// ���s�ړ����
						for (int j = 0; j < g_pAnimationKey[i].nEndFrame; j++)
						{
							// �K�v�̂Ȃ��f�[�^��������ꕨ
							int none1, none2;
							float none3;

							fgetws(FileData, 256, pFile);
							swscanf_s(FileData, L"%d;%d;%f,%f,%f,%f;;",
								&none1, &none2,
								&g_pAnimationKey[i].pWorld[j].vPosition[0],
								&g_pAnimationKey[i].pWorld[j].vPosition[1],
								&g_pAnimationKey[i].pWorld[j].vPosition[2],
								&none3);
						}

						// �{�[���I�t�Z�b�g�s��𐳂����Ȃ�悤�ɕϊ�
						g_pBoneMatrix4x4[i].Multiply(g_matArmature4x4);
						float temp;
						temp = g_pBoneMatrix4x4[i]._42;
						g_pBoneMatrix4x4[i]._42 = g_pBoneMatrix4x4[i]._43;
						g_pBoneMatrix4x4[i]._43 = temp;

						// �A�j���[�V�����̕��s�ړ��Ƀ{�[���I�t�Z�b�g�s��̕��s�ړ��l�������Ă���̂ŁA��菜��----------------
						float tempPosition[3];
						tempPosition[0] = g_pAnimationKey[i].pWorld[0].vPosition[0];
						tempPosition[1] = g_pAnimationKey[i].pWorld[0].vPosition[1];
						tempPosition[2] = g_pAnimationKey[i].pWorld[0].vPosition[2];

						for (int j = 0; j < g_pAnimationKey[i].nEndFrame; j++)
						{
							g_pAnimationKey[i].pWorld[j].vPosition[0] -= tempPosition[0];
							g_pAnimationKey[i].pWorld[j].vPosition[1] -= tempPosition[1];
							g_pAnimationKey[i].pWorld[j].vPosition[2] -= tempPosition[2];
						// ---------------------------------------------------------------------------------------------------

							Vector3D vPosition = Vector3D(g_pAnimationKey[i].pWorld[j].vPosition);
							vPosition = g_matArmature4x4.TransformPointVector3D(vPosition);

							vPosition = g_RootMatrix4x4.TransformPointVector3D(vPosition);
							g_pAnimationKey[i].pWorld[j].vPosition[0] = vPosition.x;
							g_pAnimationKey[i].pWorld[j].vPosition[1] = vPosition.y;
							g_pAnimationKey[i].pWorld[j].vPosition[2] = vPosition.z;
						}
						

						nBoneCount++;
						break;
					}
				}
				
				if (g_nBoneCount == nBoneCount) break;
			}
		}
	}

	rewind(pFile);

	// �{�[���̐e�q�֌W�������鏈��
	// blender�ō쐬����X�t�@�C���Ȃ�
	if(g_nBoneCount > 0 && g_isRoot == true)
	{
		g_pBoneDepth = new int[g_nBoneCount];
		memset(g_pBoneDepth, 0x00, sizeof(int) * g_nBoneCount);
		g_pBoneChild = new int[g_nBoneCount];
		memset(g_pBoneChild, 0x00, sizeof(int) * g_nBoneCount);
		g_pBoneInvMatrix4x4 = new D3DXMATRIX[g_nBoneCount];

		bool FindArmature = false; // �t�@�C������Armature���݂������ǂ����̃t���O
		int nBoneCount = 0;
		int nBoneDepthCount = 0;

		// �����Ń{�[���̐e�q�֌W���擾����
		while(fgetws(FileData, 256, pFile) != NULL)
		{	
			wchar_t* psframe;
			wchar_t* ctx;
			psframe = wcstok_s(FileData, L" ", &ctx);

			if (wcscmp(psframe, L"Frame") == 0)
			{
				psframe = wcstok_s(NULL, L" ", &ctx);

				for (int iBC = 0; iBC < g_nBoneCount; iBC++)
				{
					if (wcscmp(psframe, g_ppBoneName[iBC]) == 0)
					{
						int nBoneDepthCount = 0; // �{�[���̐e�q�֌W�̐[��

						for (int i = 2; i < 256; i++)
						{
							if (FileData[i * 2] == L'F') break;

							nBoneDepthCount++;
						}
						g_pBoneDepth[nBoneCount] = nBoneDepthCount;

						// �{�[����������
						wchar_t sBoneName[256];
						wcscpy_s(sBoneName, 256, psframe);

						int nBoneNumber;
						for (nBoneNumber = 0; nBoneNumber < g_nBoneCount; nBoneNumber++)
						{
							if (wcscmp(g_ppBoneName[nBoneNumber], sBoneName) == 0) break;
						}

						if (nBoneCount != nBoneNumber)
						{
							// �{�[���������Ԃɓ���ւ�----------------------------------------------
							wchar_t NameTemp[256];
							int nWeightTemp;
							int *pWeightIndexTemp;
							float *pWeightTemp;

							Matrix4x4 matTemp;
							wcscpy_s(NameTemp, 256, g_ppBoneName[nBoneCount]);
							wcscpy_s(g_ppBoneName[nBoneCount], 256, g_ppBoneName[nBoneNumber]);
							wcscpy_s(g_ppBoneName[nBoneNumber], 256, NameTemp);

							nWeightTemp = g_pSkinWeightsCount[nBoneCount];
							g_pSkinWeightsCount[nBoneCount] = g_pSkinWeightsCount[nBoneNumber];
							g_pSkinWeightsCount[nBoneNumber] = nWeightTemp;

							pWeightIndexTemp = g_ppSkinWeightsIndex[nBoneCount];
							g_ppSkinWeightsIndex[nBoneCount] = g_ppSkinWeightsIndex[nBoneNumber];
							g_ppSkinWeightsIndex[nBoneNumber] = pWeightIndexTemp;

							pWeightTemp = g_ppSkinWeights[nBoneCount];
							g_ppSkinWeights[nBoneCount] = g_ppSkinWeights[nBoneNumber];
							g_ppSkinWeights[nBoneNumber] = pWeightTemp;

							matTemp = g_pBoneMatrix4x4[nBoneCount];
							g_pBoneMatrix4x4[nBoneCount] = g_pBoneMatrix4x4[nBoneNumber];
							g_pBoneMatrix4x4[nBoneNumber] = matTemp;

							AnimationKey tempAnimation;
							tempAnimation = g_pAnimationKey[nBoneCount];
							g_pAnimationKey[nBoneCount] = g_pAnimationKey[nBoneNumber];
							g_pAnimationKey[nBoneNumber] = tempAnimation;
							// -------------------------------------------------------------------------
						}

						fgetws(FileData, 256, pFile);

						D3DXMATRIX TempInvBoneMatrix;
						// �t�{�[���I�t�Z�b�g�s����擾(���s�ړ����𒼂��p)
						for (int i = 0; i < 4; i++)
						{
							fgetws(FileData, 256, pFile);
							swscanf_s(FileData, L"%f,%f,%f,%f",
								(&g_pBoneInvMatrix4x4[nBoneCount]._11) + i * 4,
								(&g_pBoneInvMatrix4x4[nBoneCount]._12) + i * 4,
								(&g_pBoneInvMatrix4x4[nBoneCount]._13) + i * 4,
								(&g_pBoneInvMatrix4x4[nBoneCount]._14) + i * 4);
						}

						D3DXMATRIX BoneMatrix = D3DXMATRIX((float*)&g_pBoneMatrix4x4[nBoneCount]);
						// �����ŋt�{�[���s��쐬
						if (g_pBoneDepth[nBoneCount] == 0)
						{
							// �e�{�[���Ȃ�{�[���s��̋t�s������߂�
							D3DXMatrixInverse(&g_pBoneInvMatrix4x4[nBoneCount], NULL, &BoneMatrix);
						}
						else
						{
							int iBoneCount = nBoneCount;
							while (1)
							{
								iBoneCount--;
								if (g_pBoneDepth[iBoneCount] == g_pBoneDepth[nBoneCount] - 1)
								{
									// �q�{�[���̐��𑝂₷ 
									g_pBoneChild[iBoneCount]++;
									break;
								}
							}
							D3DXMATRIX BoneInvMatrix = D3DXMATRIX((float*)&g_pBoneMatrix4x4[iBoneCount]);
							D3DXMatrixInverse(&BoneInvMatrix, NULL, &BoneInvMatrix);
							D3DXMatrixMultiply(&BoneInvMatrix,
								&BoneMatrix, &BoneInvMatrix);
							D3DXMatrixInverse(&g_pBoneInvMatrix4x4[nBoneCount], NULL, &BoneInvMatrix);
						}

						nBoneCount++;
						if (nBoneCount == g_nBoneCount) break;
					}
				}

				
			}
		}
	}

	// MeshMaterialList�̖ʂ̐�������Ȃ��ꍇ(���}���u�ł��A��ŏC�����܂�)
	if (g_nTriangleSurfaceCount == g_nSurfaceCount * 2)
	{
		int* pTemp = new int[g_nTriangleSurfaceCount];

		for (int i = 0; i < g_nTriangleSurfaceCount; i++)
		{
			pTemp[i] = g_pMaterialNumber[i / 2];
		}

		memcpy_s(g_pMaterialNumber, sizeof(int) * g_nTriangleSurfaceCount,
			pTemp, sizeof(int) * g_nTriangleSurfaceCount);

		delete[] pTemp;
	}
	// �l�p�`�ƎO�p�`�ł̃}�e���A���̂�����C��
	else if (g_nInitIndex == g_nSurfaceCount)
	{
		// �}�e���A���ԍ������锠���ꎞ�I�ɍ쐬(���Ƃ�g_pTriangleSurfaceNumber�ɓ���ւ���)
		int* pTemp = new int[g_nTriangleSurfaceCount];

		for (int i = 0; i < g_nTriangleSurfaceCount; i++)
		{
			pTemp[i] = g_pMaterialNumber[g_pTriangleSurfaceNumber[i]];
		}

		memcpy_s(g_pMaterialNumber, sizeof(int) * g_nTriangleSurfaceCount,
			pTemp, sizeof(int) * g_nTriangleSurfaceCount);

		delete[] pTemp;
	}

	// �}�e���A����񂪂���ꍇ�Axn�t�@�C���Ŏg����`�ɕϊ�
	// �}�e���A���ԍ��ʂɖʂ̐��Ɩʂ̏�������
	if (g_nMaterialCount > 0)
	{
		g_ppIndexMaterial = new int*[g_nMaterialCount];
		g_pMaterialSurfaceNum = new int[g_nMaterialCount];

		// �}�e���A���𔽉f����ʂ̐��𒲂ׂ�
		for (int matenumi = 0; matenumi < g_nMaterialCount; matenumi++)
		{
			// �ϐ�������
			g_pMaterialSurfaceNum[matenumi] = 0;

			for (int j = 0; j < g_nTriangleSurfaceCount; j++)
			{
				// �}�e���A���ԍ�����v������
				if (matenumi == g_pMaterialNumber[j])
				{
					// ���f������ʂ̐��𑝂₷
					g_pMaterialSurfaceNum[matenumi]++;
				}
			}
		}

		for (int matenumi = 0; matenumi < g_nMaterialCount; matenumi++)
		{
			// �}�e���A���𔽉f������ʂ̐������}�e���A���C���f�b�N�X�����锠�����
			g_ppIndexMaterial[matenumi] = new int[g_pMaterialSurfaceNum[matenumi]];

			// �}�e���A���C���f�b�N�X����ꂽ��
			int count = 0;

			for (int j = 0; j < g_nTriangleSurfaceCount; j++)
			{
				// �}�e���A���ԍ�����v������
				if (matenumi == g_pMaterialNumber[j])
				{
					// �}�e���A���C���f�b�N�X������
					g_ppIndexMaterial[matenumi][count] = j;

					count++;

					// �}�e���A���f�[�^���Ō�܂œ��ꂽ��I��
					if (count >= g_pMaterialSurfaceNum[matenumi]) break;
				}
			}
		}
	}

	// OBB��������
	obb_position = (object_scale[0] / 2.f) + (object_scale[1] / 2.f);
	obb_scale = object_scale[0] - obb_position;

	fclose(pFile);

	return true;
}

// ���f��nx�t�@�C�����쐬����
bool CreateNXFile(wchar_t* sFileName)
{
	FILE* pFile; // �������ރt�@�C��

	// �������݃t�@�C�����J��
	if (_wfopen_s(&pFile, sFileName, L"w") != 0)
	{
		// �t�@�C�����쐬�ł��Ȃ������ꍇ
		wchar_t ErrorString[256];
		swprintf_s(ErrorString, 256, L"%s���쐬�ł��܂���ł����B", sFileName);
		wprintf_s(ErrorString);
		return false;
	}

	// ���_��񏑂�����-----------------------------
	// ���_�������o��
	fwprintf_s(pFile, L"vnum %d\n", g_nIndexCount * 3);

	// ���_��񏑂��o��
	for (int i = 0; i < g_nIndexCount * 3; i++)
	{
		fwprintf_s(pFile, L"v %f,%f,%f\n",
			g_pVertex[g_pIndexVertex[i] * 3 + 0],
			g_pVertex[g_pIndexVertex[i] * 3 + 1],
			g_pVertex[g_pIndexVertex[i] * 3 + 2]);
	}
	fwprintf_s(pFile, L"\n");
	// ---------------------------------------------
	
	// ���_�J���[��񏑂�����-----------------------
	// ���_�J���[�������o��
	if (g_nVertexColorCount == 0)
	{
		//���_�J���[�Ȃ�
	}
	else
	{
		fwprintf_s(pFile, L"cnum %d\n", g_nIndexCount * 3);

		// ���_�J���[��񏑂��o��
		for (int i = 0; i < g_nIndexCount * 3; i++)
		{
			fwprintf_s(pFile, L"c %f,%f,%f,%f\n",
				g_pVertexColor[g_pIndexVertexColor[i] * 3 + 0],
				g_pVertexColor[g_pIndexVertexColor[i] * 3 + 1],
				g_pVertexColor[g_pIndexVertexColor[i] * 3 + 2],
				g_pVertexColor[g_pIndexVertexColor[i] * 3 + 3]);
		}
	}
	fwprintf_s(pFile, L"\n");
	// ---------------------------------------------
	
	// �@����񏑂�����-----------------------------
	// �@���������o��
	fwprintf_s(pFile, L"nnum %d\n", g_nIndexCount * 3);

	// �@����񏑂��o��
	for (int i = 0; i < g_nIndexCount * 3; i++)
	{
		fwprintf_s(pFile, L"n %f,%f,%f\n",
			g_pNormal[g_pIndexNormal[i] * 3 + 0],
			g_pNormal[g_pIndexNormal[i] * 3 + 1],
			g_pNormal[g_pIndexNormal[i] * 3 + 2]);
	}
	fwprintf_s(pFile, L"\n");
	// ---------------------------------------------

	// uv��񏑂�����-------------------------------
	// uv�������o��
	if (g_nTexCount == 0)
	{
		// uv�Ȃ�
	}
	else
	{
		fwprintf_s(pFile, L"uvnum %d\n", g_nIndexCount * 3);

		// uv��񏑂��o��
		for (int i = 0; i < g_nIndexCount * 3; i++)
		{
			fwprintf_s(pFile, L"uv %f,%f\n",
				g_pTex[g_pIndexTex[i] * 2 + 0],
				g_pTex[g_pIndexTex[i] * 2 + 1]);
		}
	}
	fwprintf_s(pFile, L"\n");
	// ---------------------------------------------
	
	fwprintf_s(pFile, L"matenum %d\n", g_nMaterialCount);

	for (int i = 0; i < g_nMaterialCount; i++)
	{
		// �e�N�X�`�����
		fwprintf_s(pFile, L"tex %s\n", g_pMaterial[i].sTextureName);

		// �}�e���A�����
		// �g�U��
		fwprintf_s(pFile, L"dif %f,%f,%f,%f\n", 
			g_pMaterial[i].vDiffuseColor[0],
			g_pMaterial[i].vDiffuseColor[1],
			g_pMaterial[i].vDiffuseColor[2],
			g_pMaterial[i].vDiffuseColor[3]);

		// ���ʔ��ˌ��̋���
		fwprintf_s(pFile, L"spep %f\n",
			g_pMaterial[i].fSpecularPower);

		// ���ʔ���
		fwprintf_s(pFile, L"spe %f,%f,%f\n",
			g_pMaterial[i].vSpecularColor[0],
			g_pMaterial[i].vSpecularColor[1],
			g_pMaterial[i].vSpecularColor[2]);
		
		// ����
		fwprintf_s(pFile, L"emi %f,%f,%f\n",
			g_pMaterial[i].vEmission[0],
			g_pMaterial[i].vEmission[1],
			g_pMaterial[i].vEmission[2]);
	}

	fwprintf_s(pFile, L"\n");

	// �O�p�`�̖ʂ̐������}�e���A���C���f�b�N�X�������o��
	for (int i = 0; i < g_nMaterialCount; i++)
	{
		fwprintf_s(pFile, L"matesurfacenum %d\n", g_pMaterialSurfaceNum[i]);
	
		for (int j = 0; j < g_pMaterialSurfaceNum[i]; j++)
		{
			fwprintf_s(pFile, L"mate %d\n",
				g_ppIndexMaterial[i][j]);
		}
	}

	fwprintf_s(pFile, L"\n");

	if (g_nBoneCount > 0)
	{
		fwprintf_s(pFile, L"bonenum %d\n", g_nBoneCount);
	}

	// �{�[�����������o��--------------------------------------------------
	for (int i = 0; i < g_nBoneCount; i++)
	{
		// �e�q�֌W
		fwprintf_s(pFile, L"depth %d\n",
			g_pBoneDepth[i]);
	}

	fwprintf_s(pFile, L"\n");

	for (int i = 0; i < g_nBoneCount; i++)
	{
		// �q�{�[���̐�
		fwprintf_s(pFile, L"child %d\n",
			g_pBoneChild[i]);
	}

	fwprintf_s(pFile, L"\n");

	// �{�[���E�F�C�g�l�������o��
	for (int i = 0; i < g_nBoneCount; i++)
	{
		// �E�F�C�g�l�̐�
		fwprintf_s(pFile, L"weightsnum %d\n",
			g_nIndexCount * 3);
		
		// �E�F�C�g�l���o��
		for(int j = 0; j < g_nIndexCount * 3; j++)
		{
			fwprintf_s(pFile, L"weight %d : %f\n", j, g_ppSkinWeights[i][g_pIndexVertex[j]]);
		}

		fwprintf_s(pFile, L"matoffset %f,%f,%f,%f\n",
			g_pBoneMatrix4x4[i]._11, g_pBoneMatrix4x4[i]._12,
			g_pBoneMatrix4x4[i]._13, g_pBoneMatrix4x4[i]._14);

		fwprintf_s(pFile, L"matoffset %f,%f,%f,%f\n",
			g_pBoneMatrix4x4[i]._21, g_pBoneMatrix4x4[i]._22,
			g_pBoneMatrix4x4[i]._23, g_pBoneMatrix4x4[i]._24);

		fwprintf_s(pFile, L"matoffset %f,%f,%f,%f\n",
			g_pBoneMatrix4x4[i]._31, g_pBoneMatrix4x4[i]._32,
			g_pBoneMatrix4x4[i]._33, g_pBoneMatrix4x4[i]._34);

		fwprintf_s(pFile, L"matoffset %f,%f,%f,%f\n",
			g_pBoneMatrix4x4[i]._41, g_pBoneMatrix4x4[i]._42,
			g_pBoneMatrix4x4[i]._43, g_pBoneMatrix4x4[i]._44);

		fwprintf_s(pFile, L"invmatoffset %f,%f,%f,%f\n",
			g_pBoneInvMatrix4x4[i]._11, g_pBoneInvMatrix4x4[i]._12,
			g_pBoneInvMatrix4x4[i]._13, g_pBoneInvMatrix4x4[i]._14);

		fwprintf_s(pFile, L"invmatoffset %f,%f,%f,%f\n",
			g_pBoneInvMatrix4x4[i]._21, g_pBoneInvMatrix4x4[i]._22,
			g_pBoneInvMatrix4x4[i]._23, g_pBoneInvMatrix4x4[i]._24);

		fwprintf_s(pFile, L"invmatoffset %f,%f,%f,%f\n",
			g_pBoneInvMatrix4x4[i]._31, g_pBoneInvMatrix4x4[i]._32,
			g_pBoneInvMatrix4x4[i]._33, g_pBoneInvMatrix4x4[i]._34);

		fwprintf_s(pFile, L"invmatoffset %f,%f,%f,%f\n",
			g_pBoneInvMatrix4x4[i]._41, g_pBoneInvMatrix4x4[i]._42,
			g_pBoneInvMatrix4x4[i]._43, g_pBoneInvMatrix4x4[i]._44);

		fwprintf_s(pFile, L"\n");
	}
	// ----------------------------------------------------------------------

	// �A�j���[�V������񏑂�����(�ʂ̃t�@�C�����쐬���āA�����ɏ�������)
	if (g_nBoneCount > 0)
	{
		wchar_t* sAniFileName;

		int nFileNameCharNum = wcslen(sFileName);
		sAniFileName = new wchar_t[nFileNameCharNum + 4];
		sAniFileName[0] = L'\0';
		
		wcsncat_s(sAniFileName, nFileNameCharNum + 4, sFileName, nFileNameCharNum - 3);
		wcscat_s(sAniFileName, nFileNameCharNum + 4, L"Ani.nx");

		CreateNXAnimationFile(sAniFileName);

		delete[] sAniFileName;
	}

	// OBB���������o��
	fwprintf_s(pFile, L"obb\n");

	fwprintf_s(pFile, L"obb_scale %f,%f,%f\n",
		obb_scale.x, obb_scale.y, obb_scale.z);
	
	fwprintf_s(pFile, L"obb_pos %f,%f,%f\n",
		obb_position.x, obb_position.y, obb_position.z);
	
	fclose(pFile);

	return true;
}

// nx�̃A�j���[�V�����t�@�C��
bool CreateNXAnimationFile(wchar_t* sFileName)
{
	FILE* pFile; // �������ރt�@�C��

	// �������݃t�@�C�����J��
	if (_wfopen_s(&pFile, sFileName, L"w") != 0)
	{
		// �t�@�C�����쐬�ł��Ȃ������ꍇ
		wchar_t ErrorString[256];
		swprintf_s(ErrorString, 256, L"%s���쐬�ł��܂���ł����B", sFileName);
		wprintf_s(ErrorString);
		return false;
	}

	// �A�j���[�V�����t���[���ő吔�������o��
	fwprintf_s(pFile, L"framenum %d\n",
		g_pAnimationKey[0].nEndFrame);

	// �A�j���[�V�������������o��
	for (int i = 0; i < g_nBoneCount; i++)
	{
		for (int j = 0; j < g_pAnimationKey[i].nEndFrame; j++)
		{
			fwprintf_s(pFile, L"frame %d\n", j);

			// ��]�s��o��
			fwprintf_s(pFile, L"rota %f,%f,%f,%f\n",
				g_pAnimationKey[i].pWorld[j].matRotation._11,
				g_pAnimationKey[i].pWorld[j].matRotation._12,
				g_pAnimationKey[i].pWorld[j].matRotation._13,
				g_pAnimationKey[i].pWorld[j].matRotation._14);

			fwprintf_s(pFile, L"rota %f,%f,%f,%f\n",
				g_pAnimationKey[i].pWorld[j].matRotation._21,
				g_pAnimationKey[i].pWorld[j].matRotation._22,
				g_pAnimationKey[i].pWorld[j].matRotation._23,
				g_pAnimationKey[i].pWorld[j].matRotation._24);

			fwprintf_s(pFile, L"rota %f,%f,%f,%f\n",
				g_pAnimationKey[i].pWorld[j].matRotation._31,
				g_pAnimationKey[i].pWorld[j].matRotation._32,
				g_pAnimationKey[i].pWorld[j].matRotation._33,
				g_pAnimationKey[i].pWorld[j].matRotation._34);

			fwprintf_s(pFile, L"rota %f,%f,%f,%f\n",
				g_pAnimationKey[i].pWorld[j].matRotation._41,
				g_pAnimationKey[i].pWorld[j].matRotation._42,
				g_pAnimationKey[i].pWorld[j].matRotation._43,
				g_pAnimationKey[i].pWorld[j].matRotation._44);

			// �g��k�����
			fwprintf_s(pFile, L"scale %f,%f,%f\n",
				g_pAnimationKey[i].pWorld[j].vScale[0],
				g_pAnimationKey[i].pWorld[j].vScale[1],
				g_pAnimationKey[i].pWorld[j].vScale[2]);

			// ���s�ړ����
			fwprintf_s(pFile, L"trans %f,%f,%f\n",
				g_pAnimationKey[i].pWorld[j].vPosition[0],
				g_pAnimationKey[i].pWorld[j].vPosition[1],
				g_pAnimationKey[i].pWorld[j].vPosition[2]);
		}

		fwprintf_s(pFile, L"\n");
	}

	fclose(pFile);

	return true;
}

// ���������������
void MemoryRelease()
{
	DELETE_ARRAY(g_pVertex);
	DELETE_ARRAY(g_pTex);
	DELETE_ARRAY(g_pNormal);
	DELETE_ARRAY(g_pVertexColor);
	DELETE_ARRAY(g_pMaterial);
	DELETE_ARRAY(g_pIndexVertex);
	DELETE_ARRAY(g_pIndexTex);
	DELETE_ARRAY(g_pIndexNormal);
	DELETE_ARRAY(g_pIndexVertexColor);
	DELETE_ARRAY(g_pSkinWeightsCount);
	DELETE_ARRAY(g_ppSkinWeights);
	DELETE_ARRAY(g_ppSkinWeightsIndex);
	DELETE_ARRAY(g_pBoneMatrix4x4);
	DELETE_ARRAY(g_pBoneDepth);
	DELETE_ARRAY(g_pBoneChild);

	for (int i = 0; i < g_nMaterialCount; i++)
	{
		DELETE_ARRAY(g_ppIndexMaterial[i]);
	}
	DELETE_ARRAY(g_pMaterialSurfaceNum);
	DELETE_ARRAY(g_pTriangleSurfaceNumber);

	for (int i = 0; i < g_nBoneCount; i++)
	{
		DELETE_ARRAY(g_pAnimationKey[i].pWorld);
	}

	DELETE_ARRAY(g_pAnimationKey);
}