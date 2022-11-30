#pragma once

#include "CTaskSystem.h"
#include "CModel.h"
#include "Collision.h"
#include "CSword.h"

// ��l��
class CPlayer : public CObj3D
{
private:
	// �A�j���[�V�����̎��
	enum class AnimationKey
	{
		Walk,
		Run,
		Idle,
		Impact,
		Slash
	};

public:
	CPlayer();
	~CPlayer() {}
	void Update();
	void Draw();
	CSword* GetSwordData() { return m_Sword; }
	int* GetpHp() { return &m_Hp; }
	int GetHpMax() { return m_HpMax; }

private:
	// �����蔻�菈��
	void Collsion();

	// �ړ�����
	void Move();

	// ����U��
	void SwordSlash();

private:
	CModel* m_Model;
	bool   m_SlashSwordFlag;	// ����U��t���O
	bool   m_DamageAniMoveFlag;	// �_���[�W���󂯂�A�j���[�V�����𓮂����t���O
	int    m_HpMax;				// �q�b�g�|�C���g�ő�l
	int    m_Hp;				// �q�b�g�|�C���g
	int    m_nInvincibleTime;   // ���G����

	D3DXVECTOR3 m_Velocity;	// �ړ��x�N�g��
	float m_MoveSpeed;		// �ړ����x

	Collision::OBB3D	 m_ColObb;
	Collision::Segment3D m_ColSegment;

	// �A�j���[�V�����L�[�t���[���Ǘ��ϐ�
	float m_fIdelAniFrame;
	float m_fRunAniFrame;
	float m_fSwordAniFrame;
	float m_fWalkAniFrame;
	
	CSword* m_Sword;			// ��l���������I�u�W�F�N�g
};
