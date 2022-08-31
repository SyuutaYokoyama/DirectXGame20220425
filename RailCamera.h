#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "DebugText.h"

class RailCamera {
public:
	void Initialize(const Vector3& position,const Vector3& rotation);
	void Update();
	Vector3 Vector3TransformNormal(Vector3 velocity,Matrix4 matWorld);
	//�r���[�v���W�F�N�V�������擾
	const ViewProjection& GetViewProjection() { return viewProjection_; };
	//���[���h�s����擾
	const WorldTransform& GetWorldMatrix() { return worldTransform_; }
private:
	//���[���h�ϊ��f�[�^
	WorldTransform worldTransform_;
	//�r���[�v���W�F�N�V����
	ViewProjection viewProjection_;
	// �f�o�b�O�e�L�X�g
	DebugText* debugText_ = nullptr;
};
