#pragma once

#include "CTaskSystem.h"
#include "CModel.h"
#include "Collision.h"
#include "CSword.h"

// 敵オブジェクト
class CEnemy : public CObj3D
{
private:
	// アニメーションの種類
	enum class AnimationKey
	{
		Walk,	// 歩く
		Run,	// 走る
		Idle,	// 動いていない
		Impact, // ダメージを受ける
		Slash	// 剣を振る
	};

public:
	CEnemy(const D3DXVECTOR3& vPosition); // 初期位置を設定
	~CEnemy() {}
	void Update();
	void Draw();
	Collision::OBB3D* GetObb() { return &m_ColObb; }
	// 死亡フラグ管理関数
	void SetDefeatFlag(bool flag) { m_isDefeat = flag; }

private:
	// 当たり判定処理
	void Collsion();

	// 移動処理
	void Move();

private:
	CModel* m_pModel;

	// 移動ベクトル
	D3DXVECTOR3 m_Velocity;

	// 移動速度
	float m_MoveSpeed;

	// OBB当たり判定管理変数
	Collision::OBB3D	 m_ColObb;
	// 線分当たり判定管理変数
	Collision::Segment3D m_ColSegment;

	// ジャンプをするタイミングを管理する変数
	int m_jump_time;

	// 重力の強さ
	D3DXVECTOR3 m_vGravity;

	// プレイヤーの位置情報(追いかける移動の際に必要)
	D3DXVECTOR3* m_pvPlayerPosition;

	// 主人公に倒されたかどうか
	bool m_isDefeat;

	// 倒された際の回転
	float m_DefeatRadian;
};
