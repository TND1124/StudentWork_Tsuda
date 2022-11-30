#include "CModelManager.h"

// ���f���f�[�^�������Ă���f�B���N�g���������Ŏw��
// (�����Ŏw�肷�邱�Ƃł킴�킴�S�Ẵf�B���N�g���������Ȃ��Ă��悭�Ȃ�)
#define MODEL_DIRECTORY (L"Asset\\Model\\")

using namespace std;

CModelManager* CModelManager::m_instance = 0;

void CModelManager::Create()
{
	if (m_instance != NULL)
	{
		MessageBox(NULL, L"���f���Ǘ��N���X�쐬���s", L"�G���[", MB_OK);
		return;
	}

	m_instance = new CModelManager;
}

CModelManager* CModelManager::GetInstance()
{
	if (m_instance == NULL)
	{
		MessageBox(NULL, L"���f���Ǘ��N���X����������Ă��܂���B", L"�G���[", MB_OK);
		return NULL;
	}

	return m_instance;
}

void CModelManager::Destory()
{
	if (m_instance != nullptr)
	{
		delete m_instance;
		m_instance = nullptr;
	}
}

CModelManager::~CModelManager()
{
	for(auto &itr : m_ModelMap)
	{
		delete itr.second;
	}

	m_ModelMap.clear();
}

// �摜�쐬
// ����1 const wchar_t* : Asset\Model�ɓ����Ă���t�@�C����
bool CModelManager::CreateModel(const wchar_t* wzFileName)
{
	CModel* pModel;	

	// �f�B���N�g���ƃt�@�C���������킹��
	wstring wsFileName(MODEL_DIRECTORY);
	wsFileName += wzFileName;

	// ���f���쐬
	pModel = new CModel;

	// ���f���f�[�^�����[�h���܂�
	if (pModel->LoadNXModel(wsFileName.c_str()) == true)
	{
		m_ModelMap.insert(make_pair(wsFileName, pModel));

		return true;
	}

	// �f�[�^���Ȃ��ꍇ
	wchar_t str[256];
	wcscpy_s(str, wsFileName.c_str());
	wcscat_s(str, L"��������܂���ł����B");
	MessageBox(NULL, str, L"�G���[", MB_OK);
	return false;
}

// �摜�f�[�^�擾
CModel* CModelManager::GetModel(const wchar_t* FileName)
{
	unordered_map<wstring, CModel*>::iterator itr;

	// �f�B���N�g���ƃt�@�C���������킹��
	wstring wsFileName(MODEL_DIRECTORY);
	wsFileName += FileName;

	// �摜�f�[�^��T��
	itr = m_ModelMap.find(wsFileName);

	// �f�[�^�����������ꍇ
	if (itr != m_ModelMap.end())
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
