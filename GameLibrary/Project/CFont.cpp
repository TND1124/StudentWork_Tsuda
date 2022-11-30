// �t�H���g�摜��\��������@��
// �܂�؂�����[�l�̃T�C�g���玝���Ă��܂����B
// http://marupeke296.com/DX10_No5_FontTexture.html

#include "CFont.h"
#include "CDevice3D.h"

#pragma warning(push)
#pragma warning(disable : 4005)
#include <d3dx10.h>

#pragma comment(lib,"d3d10.lib")
#pragma comment(lib,"d3dx10.lib")
#pragma warning(pop)

using namespace std;

CFont* CFont::m_pInstance = 0;

void CFont::Create()
{
	if (m_pInstance != NULL)
	{
		MessageBox(NULL, L"�t�H���g�N���X�쐬���s", L"�G���[", MB_OK);
		return;
	}

	m_pInstance = new CFont;
}

CFont* CFont::GetInstance()
{ 
	if (m_pInstance == NULL)
	{
		MessageBox(NULL, L"�t�H���g�N���X����������Ă��܂���B", L"�G���[", MB_OK);
		return NULL;
	}

	return m_pInstance;
}

void CFont::Destory()
{
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

CFont::CFont()
{
	// �t�H���g�̐���
	LOGFONT lf = { 64, 0, 0, 0, 0, 0, 0, 0, SHIFTJIS_CHARSET, OUT_TT_ONLY_PRECIS,
	CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, L"�l�r ����" };
	if (!(m_hFont = CreateFontIndirect(&lf)))
	{
		MessageBox(NULL, L"�t�H���g�������s", L"�G���[", MB_OK);
		return;
	}

	// �f�o�C�X�R���e�L�X�g�擾
	// �f�o�C�X�Ƀt�H���g���������Ȃ���GetGlyphOutline�֐��̓G���[�ƂȂ�
	m_hdc = GetDC(NULL);
	m_oldFont = (HFONT)SelectObject(m_hdc, m_hFont);

	// �t�H���g�r�b�g�}�b�v�擾
	GetTextMetrics(m_hdc, &m_TM);
}

CFont::~CFont()
{
	//�����GDI�I�u�W�F�N�g��j��
	DeleteObject(m_oldFont);
	DeleteObject(m_hFont);
	//�f�B�X�v���C�f�o�C�X�R���e�L�X�g�n���h�����
	ReleaseDC(NULL, m_hdc);
}

// �t�H���g�摜���쐬
ID3D10ShaderResourceView* CFont::FontGraphicCreate(wchar_t Char)
{
	unordered_map< wchar_t, ID3D10ShaderResourceView* > ::iterator itr;

	itr = m_TextureMap.find(Char);
	
	if (itr != m_TextureMap.end())
	{
		return itr->second;
	}

	// �����R�[�h�擾
	UINT code = 0;
	code = (UINT)Char;
	
	GLYPHMETRICS GM;
	CONST MAT2 Mat = { {0,1}, {0,0}, {0,0}, {0,1} };
	DWORD size = GetGlyphOutline(m_hdc, code, GGO_GRAY4_BITMAP, &GM, 0, NULL, &Mat);
	BYTE* ptr = new BYTE[size];
	GetGlyphOutline(m_hdc, code, GGO_GRAY4_BITMAP, &GM, size, ptr, &Mat);

	// �e�N�X�`���쐬
	D3D10_TEXTURE2D_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Width = GM.gmCellIncX;
	desc.Height = m_TM.tmHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// RGBA(255,255,255,255)�^�C�v
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D10_USAGE_DYNAMIC;			// ���I�i�������݂��邽�߂̕K�{�����j
	desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;	// �V�F�[�_���\�[�X�Ƃ��Ďg��
	desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;	// CPU����A�N�Z�X���ď�������OK

	ID3D10Texture2D* pTexture2D = NULL;
	CDevice3D* pDevice3D = CDevice3D::GetInstance();
	pDevice3D->GetDevice()->CreateTexture2D(&desc, 0, &pTexture2D);

	D3DXVECTOR2 wh;
	wh.x = (float)desc.Width;
	wh.y = (float)desc.Height;

	// �e�N�X�`���ɏ�������
	// �e�N�X�`�����}�b�v�i�����b�N�j����ƁA
	// �������ɃA�N�Z�X���邽�߂̏��D3D10_MAPPED_TEXTURE2D�Ɋi�[����܂��B
	D3D10_MAPPED_TEXTURE2D mapped;
	pTexture2D->Map(D3D10CalcSubresource(0, 0, 1),
		D3D10_MAP_WRITE_DISCARD,
		0,
		&mapped);

	BYTE* pBits = (BYTE*)mapped.pData;

	// �t�H���g���̏�������
	// iOfs_x, iOfs_y : �����o���ʒu(����)
	// iBmp_w, iBmp_h : �t�H���g�r�b�g�}�b�v�̕���
	// Level : ���l�̒i�K (GGO_GRAY4_BITMAP�Ȃ̂�17�i�K)
	int iOfs_x = GM.gmptGlyphOrigin.x;
	int iOfs_y = m_TM.tmAscent - GM.gmptGlyphOrigin.y;
	int iBmp_w = GM.gmBlackBoxX + (4 - (GM.gmBlackBoxX % 4)) % 4;
	int iBmp_h = GM.gmBlackBoxY;
	int Level = 17;
	int x, y;
	DWORD Alpha, Color;
	memset(pBits, 0, mapped.RowPitch * m_TM.tmHeight);
	for (y = iOfs_y; y < iOfs_y + iBmp_h; y++)
	{
		for (x = iOfs_x; x < iOfs_x + iBmp_w; x++)
		{
			Alpha = (255 * ptr[x - iOfs_x + iBmp_w * (y - iOfs_y)]) / (Level - 1);
			Color = 0x00ffffff | (Alpha << 24);
			memcpy((BYTE*)pBits + mapped.RowPitch * y + 4 * x, &Color, sizeof(DWORD));
		}
	}

	pTexture2D->Unmap(D3D10CalcSubresource(0, 0, 1));

	ID3D10ShaderResourceView* pTexture;
	pDevice3D->GetDevice()->CreateShaderResourceView(pTexture2D, NULL, &pTexture);

	// �t�H���g�摜�f�[�^��ۑ�
	m_TextureMap.insert(make_pair(Char, pTexture));

	delete[] ptr;

	return pTexture;
}
