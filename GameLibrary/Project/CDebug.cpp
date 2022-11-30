#include "CDebug.h"
#include "CShader.h"

CDebug* CDebug::m_pInstance = 0;

void CDebug::Create()
{
	if (m_pInstance != NULL)
	{
		MessageBox(NULL, L"�f�o�b�O�N���X�������s", L"�G���[", MB_OK);
		return;
	}

	m_pInstance = new CDebug;
}

CDebug* CDebug::GetInstance()
{
	if (m_pInstance == NULL)
	{
		MessageBox(NULL, L"�f�o�b�O�N���X����������Ă��܂���B", L"�G���[", MB_OK);
		return NULL;
	}

	return m_pInstance;
}

void CDebug::Destory()
{
	if (m_pInstance != NULL)
	{
		delete m_pInstance;
		m_pInstance = 0;
	}
}

CDebug::CDebug()
{
	m_StartTime = timeGetTime();
	m_nFpsCounter = 0;
	m_nFpsCount = 0;
}

void CDebug::Update()
{
#ifdef _DEBUG
	// FPS�𒲂ׂ�---------------------------------------------------

	// 1�b�o�߂������ǂ����𒲂ׂ�
	if (timeGetTime() - m_StartTime > 1000)
	{
		// 1�b�o��

		// ��񏉊���
		m_StartTime = timeGetTime();
		m_nFpsCount = m_nFpsCounter;
		m_nFpsCounter = 0;
	}
	else
	{
		// 1�b���o��
		m_nFpsCounter++;
	}

	// -------------------------------------------------------------
#endif
}

void CDebug::Draw()
{
#ifdef _DEBUG
	CShader* pShader = CShader::GetInstance();

	// FPS��\��
	wchar_t str[256];
	swprintf_s(str, L"FPS : %d", m_nFpsCount);
	pShader->StrDraw(0.f, 500.f, 32.f, str);
#endif
}
