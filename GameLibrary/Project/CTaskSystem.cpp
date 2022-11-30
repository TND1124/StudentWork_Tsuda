#include "CTaskSystem.h"

#include <Windows.h>

using namespace std;

CTaskSystem* CTaskSystem::m_pInstance = 0;

CTaskSystem::CTaskSystem()
{
	m_TaskList = new list<shared_ptr<CObj>>();
}

CTaskSystem::~CTaskSystem()
{
	if (m_TaskList)
	{
		m_TaskList->clear();
		delete m_TaskList;
		m_TaskList = NULL;
	}
}

void CTaskSystem::Create()
{
	// ����������h��
	if (m_pInstance != nullptr)
	{
		MessageBox(NULL, L"CTaskSystem��������������܂����B",
			L"�G���[", MB_OK);
		return;
	}

	m_pInstance = new CTaskSystem;
}

CTaskSystem* CTaskSystem::GetInstance()
{
	if (m_pInstance == NULL)
	{
		MessageBox(NULL, L"CTaskSystem�N���X����������Ă��܂���", L"�G���[", MB_OK);
		return NULL;
	}
	return m_pInstance;
}

void CTaskSystem::Destory()
{
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

void CTaskSystem::AddObj(CObj* object)
{
	//�V�F�A�|�C���^�ɃI�u�W�F�N�g�̃A�h���X���i�[
	shared_ptr< CObj > sp;
	sp.reset(object);

	m_TaskList->push_back(sp);
}

// �S�ẴI�u�W�F�N�g���X�V
void CTaskSystem::Update()
{
	for (auto ip = m_TaskList->begin(); ip != m_TaskList->end();)
	{
		// �폜�t���O�������Ă�����A�I�u�W�F�N�g���폜
		if (ip->get()->GetDeleteFlag() == true)
		{
			ip = m_TaskList->erase(ip);
		}
		else
		{
			ip->get()->Update();
			ip++;
		}
	}
}

// �S�ẴI�u�W�F�N�g��`��
void CTaskSystem::Draw()
{
	for (auto ip = m_TaskList->begin(); ip != m_TaskList->end(); ip++)
	{
		ip->get()->Draw();
	}
}

// �I�u�W�F�N�g���擾
// ����1 const type_info& : typeid(�N���X��)�Ǝw�肷��
// ����2 vector<shared_ptr<Co>>
void CTaskSystem::ObjsInfomation(const type_info& Type, vector<shared_ptr<CObj>>* pObj)
{
	// ���X�g�̐�[���疖�[�܂Ō���
	for (auto ip = m_TaskList->begin(); ip != m_TaskList->end(); ip++)
	{
		// �^�`�F�b�N
		if (Type == typeid(*ip->get()))
		{
			// �I�u�W�F�N�g���폜����Ă��Ȃ���
			if (ip->get()->GetDeleteFlag() == true) continue;

			// �����^�ł���΁Aobj�ɃA�h���X������
			pObj->push_back(*ip);
		}
	}
}

// �I�u�W�F�N�g���폜����Ă���܂��́A���݂��Ă��Ȃ���
bool CTaskSystem::ObjDeleteCheck(CObj* object)
{
	if (object == NULL) return true;

	// ���X�g�̐�[���疖�[�܂Ō���
	for (auto ip = m_TaskList->begin(); ip != m_TaskList->end(); ip++)
	{
		if (ip->get() == object)
		{
			return false;
		}
	}

	return true;
}

// �S�ẴI�u�W�F�N�g���폜����֐�
void CTaskSystem::ObjAllDelete()
{
	m_TaskList->clear();
}

// �I�u�W�F�N�g�̐���Ԃ��֐�
// ����1 const type_info& : ���ׂ����N���X typeid(�N���X��)�Ǝw�肷��
// �߂�l int			  : �I�u�W�F�N�g�̐� 
int CTaskSystem::ObjectNum(const type_info& Type)
{
	// �I�u�W�F�N�g���L�^����ϐ�
	int nCount = 0;

	// ���X�g�̐�[���疖�[�܂Ō���
	for (auto ip = m_TaskList->begin(); ip != m_TaskList->end(); ip++)
	{
		// �^�`�F�b�N
		if (Type == typeid(*ip->get()))
		{
			nCount++;
		}
	}

	return nCount;
}
