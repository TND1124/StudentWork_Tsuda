#pragma once

#include <vector>
#include <memory>
#include <list>
#include <algorithm>
#include <typeinfo.h>

#pragma warning(push)
#pragma warning(disable : 4005)
#include <d3dx10.h>
#pragma warning(pop)

// �I�u�W�F�N�g�p���p
class CObj
{
public:
	CObj()
	{
		m_isDelete = false;
	}
	virtual ~CObj() {};
	virtual void Update() = 0;
	virtual void Draw() = 0;

	void ObjDelete() { m_isDelete = true; }
	bool GetDeleteFlag() { return m_isDelete; }

private:
	bool m_isDelete; // �폜�t���O
};

// 3D�I�u�W�F�N�g�p���p
class CObj3D : public CObj
{
public:
	CObj3D()
	{
		m_vScale	= D3DXVECTOR3(1.f, 1.f, 1.f);
		m_qRotate	= D3DXQUATERNION(0.f, 0.f, 0.f, 1.f);
		m_vPosition = D3DXVECTOR3(0.f, 0.f, 0.f);
		D3DXMatrixIdentity(&m_matWorld);
	}
	D3DXVECTOR3* GetScale()		{ return &m_vScale; }
	D3DXQUATERNION* GetRotate() { return &m_qRotate; }
	D3DXVECTOR3* GetTranslate() { return &m_vPosition; }
	D3DXMATRIX* GetMatWorld()	{ return &m_matWorld; }
	// m_vScale�Ȃǂ̃I�u�W�F�N�g�����o�����g���āA���[���h�s����쐬���āAm_matWorld�ɓ���܂��B
	void CreateMatWorld() 
	{ 
		D3DXMATRIX matScale, matRotate, matTranslate;
		D3DXMatrixScaling(&matScale, m_vScale.x, m_vScale.y, m_vScale.z);
		D3DXMatrixRotationQuaternion(&matRotate, &m_qRotate);
		D3DXMatrixTranslation(&matTranslate, m_vPosition.x, m_vPosition.y, m_vPosition.z);
		m_matWorld = matScale * matRotate * matTranslate;
	}

protected:
	D3DXVECTOR3	   m_vScale;	// �傫�����
	D3DXQUATERNION m_qRotate;	// ��]���
	D3DXVECTOR3	   m_vPosition;	// �ʒu���
	D3DXMATRIX	   m_matWorld;	// ���[���h�s��
};

// �^�N�X���Ǘ�����N���X
class CTaskSystem
{
private:
	CTaskSystem();
	CTaskSystem(const CTaskSystem&) {}
	~CTaskSystem();

public:
	static void Create();
	static CTaskSystem* GetInstance();
	static void Destory();

	// �I�u�W�F�N�g�ǉ�
	void AddObj(CObj * object);
	// �I�u�W�F�N�g�X�V
	void Update();
	// �I�u�W�F�N�g�`��
	void Draw();
	// �I�u�W�F�N�g���擾
	// ����1 const type_info& : typeid(�N���X��)�Ǝw�肷��
	void ObjsInfomation(const type_info& type, std::vector<std::shared_ptr<CObj>>* obj);
	// �N���X�I�u�W�F�N�g�̐�
	// ����1 const type_info& : typeid(�N���X��)�Ǝw�肷��
	int ObjectNum(const type_info& type);
	
	// �I�u�W�F�N�g���폜����Ă���܂��́A���݂��Ă��Ȃ���
	bool ObjDeleteCheck(CObj* object);
	// �S�ẴI�u�W�F�N�g���폜����֐�
	void ObjAllDelete();

public:
	static CTaskSystem* m_pInstance;

	std::list<std::shared_ptr<CObj>>* m_TaskList; // �Q�[���I�u�W�F�N�g���X�g
};
