#pragma once

#pragma warning(push)
#pragma warning(disable : 4005)
#include <d3dx10.h>
#pragma warning(pop)

// ���f���N���X�Ƃ̑��݃C���N���[�h���
class CModel;

namespace Collision
{
	// AABB3D
	struct AABB3D
	{
		D3DXVECTOR3 m_vPosition;	// ���S���W
		D3DXVECTOR3 m_vScale;		// �傫��
	};

	// AABB���m�̏Փ˔���
	bool HitAABB3D(const AABB3D& Aabb_1, const AABB3D& Aabb_2);

	// ����
	struct Sphere3D
	{
		D3DXVECTOR3 m_vPosition; // ���S���W
		float m_fRadius;		 // ���a
	};

	// Sphere���m�̏Փ˔���
	bool HitSphere3D(const Sphere3D& Sphere_1, const Sphere3D& Sphere_2);

	// ����
	struct Segment3D
	{
		D3DXVECTOR3 m_vStartPosition; // �n�_���W
		D3DXVECTOR3 m_vEndPosition;   // �I�_���W
	};

	// �����Ɠ_�̍ŒZ����(2�{)
	float SegmentAndPointDistanceSq(const Segment3D& Segment, const D3DXVECTOR3& Point);

	// �����ƎO�p�`�̓����蔻��
	// ����1 Segment    : �������
	// ����2 vPoint1    : �O�p�`�̓_�̈ʒu�x�N�g������1
	// ����3 vPoint2    : �O�p�`�̓_�̈ʒu�x�N�g������2
	// ����4 vPoint3    : �O�p�`�̓_�̈ʒu�x�N�g������3
	// ����5 voutInter* : �����̎n�_�����_�ւ̃x�N�g��(out)
	// �߂�l		    : �����������ǂ���
	bool HitSegmentAndTriangle(const Segment3D& Segment, const D3DXVECTOR3& vPoint1,
		const D3DXVECTOR3& vPoint2, const D3DXVECTOR3& vPoint3, D3DXVECTOR3* ovutInter);

	// �����ƃ��f���̓����蔻��
	// ����1 Segment    : �������
	// ����2 CModel     : ���f���f�[�^
	// ����3 D3DXMATRIX : ���f���̃��[���h���W
	// ����4 voutInter* : �����̎n�_�����_�ւ̃x�N�g��(out)
	// �߂�l		    : �����������ǂ���
	// �����ƃ��f���̓����蔻��
	bool HitSegmentAndModel(const Segment3D& Segment, const CModel& Model, const D3DXMATRIX& matWorld, D3DXVECTOR3* voutInter);

	// �J�v�Z��
	struct Capsule3D
	{
		Segment3D m_Segment; // ����
		float m_fRadius;	 // ���a
	};

	// �J�v�Z���Ƌ��̂̏Փ˔���
	bool HitSphereAndCapsule(const Sphere3D& Sphere, const Capsule3D& Capusule);

	// �J�v�Z���ƃJ�v�Z���̏Փ˔���(�e�X�g��)
	bool HitCapsule3D(const Capsule3D& Capusule1, const Capsule3D& Capusule2);

	// OBB
	struct OBB3D
	{
		OBB3D()
		{
			m_vInitLength = D3DXVECTOR3(1.f, 1.f, 1.f);
			m_vInitPosition = D3DXVECTOR3(0.f, 0.f, 0.f);
		}

		// ���݂�OBB�̒l
		D3DXVECTOR3 m_vPosition; // ���S���W
		D3DXVECTOR3 m_vSlope[3]; // XYZ�̊e���W���̌X����\�������x�N�g��
		D3DXVECTOR3 m_vLength;   // OBB�̊e���W���ɉ����������̔���

	private:
		// OBB�̏�����ԕۑ��p
		D3DXVECTOR3 m_vInitPosition; // ���S���W
		D3DXVECTOR3 m_vInitLength;   // OBB�̊e���W���ɉ����������̔���

	public:
		
		void Init(const D3DXVECTOR3& vLength, const D3DXVECTOR3& vPosition); // OBB��������

		void Update(const D3DXVECTOR3& vScale, const D3DXQUATERNION& qRotate, const D3DXVECTOR3& vPosition); // OBB�̏��X�V
	};
											  	  
	// OBB�Ɠ_�̍ŒZ����
	float ObbAndPointDistance(const OBB3D& Obb, const D3DXVECTOR3& Point);

	// OBB�Ƌ��̂̏Փ˔���
	bool HitObbAndSphere3D(const OBB3D& Obb, const Sphere3D& Sphere);

	// OBB��OBB�̏Փ˔���
	bool HitObb3D(const OBB3D& Obb1, const OBB3D& Obb2);

	// �f�o�b�O�p�����蔻��---------------------------------------------
	// OBB�����蔻��\��
	void DrawDebugOBB(const OBB3D& Obb);

	// Sphere�����蔻��\��
	void DrawDebugSphere(const Sphere3D& Obb);
	// -----------------------------------------------------------------
}
