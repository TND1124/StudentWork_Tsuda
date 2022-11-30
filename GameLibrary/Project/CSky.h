#pragma once

#include "CTaskSystem.h"
#include "CModel.h"

// 空(そら)オブジェクト(キャラクターが画面外に出ないようにする役割もある)
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
