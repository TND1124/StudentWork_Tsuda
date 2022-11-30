#pragma once

#include "CTaskSystem.h"
#include "CModel.h"
#include "Collision.h"

// キャラクターに持たせる剣
class CSword : public CObj3D
{
public:
	CSword();
	~CSword() {}
	void Update() {}
	void Draw();

	void Move(const D3DXMATRIX& matWorld);
	Collision::OBB3D* GetObb() { return &m_ColObb; }

	// 当たり判定処理
	void Collision();
	
private:
	CModel* m_Model;

	Collision::OBB3D m_ColObb;
	bool m_isHitBoxDraw; // デバッグ用の当たり判定を表示させるかどうか
};
