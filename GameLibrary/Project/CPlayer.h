#pragma once

#include "CTaskSystem.h"
#include "CModel.h"
#include "Collision.h"
#include "CSword.h"

// 主人公
class CPlayer : public CObj3D
{
private:
	// アニメーションの種類
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
	// 当たり判定処理
	void Collsion();

	// 移動処理
	void Move();

	// 剣を振る
	void SwordSlash();

private:
	CModel* m_Model;
	bool   m_SlashSwordFlag;	// 剣を振るフラグ
	bool   m_DamageAniMoveFlag;	// ダメージを受けるアニメーションを動かすフラグ
	int    m_HpMax;				// ヒットポイント最大値
	int    m_Hp;				// ヒットポイント
	int    m_nInvincibleTime;   // 無敵時間

	D3DXVECTOR3 m_Velocity;	// 移動ベクトル
	float m_MoveSpeed;		// 移動速度

	Collision::OBB3D	 m_ColObb;
	Collision::Segment3D m_ColSegment;

	// アニメーションキーフレーム管理変数
	float m_fIdelAniFrame;
	float m_fRunAniFrame;
	float m_fSwordAniFrame;
	float m_fWalkAniFrame;
	
	CSword* m_Sword;			// 主人公が持つ剣オブジェクト
};
