#pragma once

#include "CTaskSystem.h"
#include "CModel.h"

// ��(����)�I�u�W�F�N�g(�L�����N�^�[����ʊO�ɏo�Ȃ��悤�ɂ������������)
class CSky : public CObj3D
{
public:
	CSky();
	~CSky() {};
	void Update() {}
	void Draw();

	CModel* GetModel() { return &m_Model; }

private:
	CModel m_Model;
};
