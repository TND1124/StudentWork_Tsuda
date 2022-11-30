#pragma once

#include "CTaskSystem.h"
#include "CModel.h"

// è∞
class Cyuka : public CObj3D
{
public:
	Cyuka();
	~Cyuka() {};
	void Update();
	void Draw();

	CModel* GetModel() { return &m_Model; }

private:
	CModel m_Model;
};
