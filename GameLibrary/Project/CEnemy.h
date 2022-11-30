#pragma once

#include "CTaskSystem.h"
#include "CModel.h"
#include "Collision.h"
#include "CSword.h"

// �G�I�u�W�F�N�g
class CEnemy : public CObj3D
{
private:
	// �A�j���[�V�����̎��
	enum class AnimationKey
	{
		Walk,	// ����
		Run,	// ����
		Idle,	// �����Ă��Ȃ�
		Impact, // �_���[�W���󂯂�
		Slash	// ����U��
	};

public:
	CEnemy(const D3DXVECTOR3& vPosition); // �����ʒu��ݒ�
	~CEnemy() {}
	void Update();
	void Draw();
	Collision::OBB3D* GetObb() { return &m_ColObb; }
	// ���S�t���O�Ǘ��֐�
	void SetDefeatFlag(bool flag) { m_isDefeat = flag; }

private:
	// �����蔻�菈��
	void Collsion();

	// �ړ�����
	void Move();

private:
	CModel* m_pModel;

	// �ړ��x�N�g��
	D3DXVECTOR3 m_Velocity;

	// �ړ����x
	float m_MoveSpeed;

	// OBB�����蔻��Ǘ��ϐ�
	Collision::OBB3D	 m_ColObb;
	// ���������蔻��Ǘ��ϐ�
	Collision::Segment3D m_ColSegment;

	// �W�����v������^�C�~���O���Ǘ�����ϐ�
	int m_jump_time;

	// �d�͂̋���
	D3DXVECTOR3 m_vGravity;

	// �v���C���[�̈ʒu���(�ǂ�������ړ��̍ۂɕK�v)
	D3DXVECTOR3* m_pvPlayerPosition;

	// ��l���ɓ|���ꂽ���ǂ���
	bool m_isDefeat;

	// �|���ꂽ�ۂ̉�]
	float m_DefeatRadian;
};
