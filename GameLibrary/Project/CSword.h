#pragma once

#include "CTaskSystem.h"
#include "CModel.h"
#include "Collision.h"

// �L�����N�^�[�Ɏ������錕
class CSword : public CObj3D
{
public:
	CSword();
	~CSword() {}
	void Update() {}
	void Draw();

	void Move(const D3DXMATRIX& matWorld);
	Collision::OBB3D* GetObb() { return &m_ColObb; }

	// �����蔻�菈��
	void Collision();
	
private:
	CModel* m_Model;

	Collision::OBB3D m_ColObb;
	bool m_isHitBoxDraw; // �f�o�b�O�p�̓����蔻���\�������邩�ǂ���
};
