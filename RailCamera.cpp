#include "RailCamera.h"
#include "TextureManager.h"
#include <cassert>
#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"
Vector3 RailCamera::Vector3TransformNormal(Vector3 velocity,Matrix4 matWorld_) {
	Vector3 v;
	v.x = velocity.x * matWorld_.m[0][0] + velocity.y * matWorld_.m[1][0] + velocity.z * matWorld_.m[2][0];
	v.y = velocity.x * matWorld_.m[0][1] + velocity.y * matWorld_.m[1][1] + velocity.z * matWorld_.m[2][1];
	v.z = velocity.x * matWorld_.m[0][2] + velocity.y * matWorld_.m[1][2] + velocity.z * matWorld_.m[2][2];
	return v;
}
void RailCamera::Initialize(const Vector3& position,const Vector3& rotation) {
	worldTransform_.translation_ = position;
	worldTransform_.rotation_ = rotation;
	viewProjection_.farZ = 100.0f;
	viewProjection_.Initialize();
}
void RailCamera::Update() {
	//�ړ��i�x�N�g�������Z�j
	worldTransform_.translation_ += Vector3(0, 0, 0.00f);
	//���[���h�g�����X�t�H�[���̍X�V
	Matrix4 matIdentity;
	matIdentity.m[0][0] = 1;
	matIdentity.m[1][1] = 1;
	matIdentity.m[2][2] = 1;
	matIdentity.m[3][3] = 1;

	Matrix4 matScale = matIdentity;
	matScale.m[0][0] = worldTransform_.scale_.x;
	matScale.m[1][1] = worldTransform_.scale_.y;
	matScale.m[2][2] = worldTransform_.scale_.z;
	matScale.m[3][3] = 1;

	Matrix4 matRotZ = matIdentity;
	matRotZ.m[0][0] = cos(worldTransform_.rotation_.z);
	matRotZ.m[0][1] = sin(worldTransform_.rotation_.z);
	matRotZ.m[1][0] = -sin(worldTransform_.rotation_.z);
	matRotZ.m[1][1] = cos(worldTransform_.rotation_.z);

	Matrix4 matRotX = matIdentity;
	matRotX.m[1][1] = cos(worldTransform_.rotation_.x);
	matRotX.m[1][2] = sin(worldTransform_.rotation_.x);
	matRotX.m[2][1] = -sin(worldTransform_.rotation_.x);
	matRotX.m[2][2] = cos(worldTransform_.rotation_.x);

	Matrix4 matRotY = matIdentity;
	matRotY.m[0][0] = cos(worldTransform_.rotation_.y);
	matRotY.m[0][2] = sin(worldTransform_.rotation_.y);
	matRotY.m[2][0] = -sin(worldTransform_.rotation_.y);
	matRotY.m[2][2] = cos(worldTransform_.rotation_.y);

	Matrix4 matTrans = matIdentity;
	matTrans.m[3][0] = worldTransform_.translation_.x;
	matTrans.m[3][1] = worldTransform_.translation_.y;
	matTrans.m[3][2] = worldTransform_.translation_.z;
	matTrans.m[3][3] = 1;

	worldTransform_.matWorld_ = matIdentity;
	worldTransform_.matWorld_ *= matScale;

	worldTransform_.matWorld_ *= matRotZ;
	worldTransform_.matWorld_ *= matRotX;
	worldTransform_.matWorld_ *= matRotY;

	worldTransform_.matWorld_ *= matTrans;
	//���[���h���W�̕��s�ړ��������擾
	viewProjection_.eye.x = worldTransform_.matWorld_.m[3][0];
	viewProjection_.eye.y = worldTransform_.matWorld_.m[3][1];
	viewProjection_.eye.z = worldTransform_.matWorld_.m[3][2];

	//���[���h�O���x�N�g��
	Vector3 forward(0, 0, 1);
	//���[���J�����̉�]�𔽉f�i���[���J�����̉�]�x�N�g���j
	forward = Vector3TransformNormal(forward, worldTransform_.matWorld_);
	//���_����O���ɓK���ȋ����i�񂾈ʒu�������_
	viewProjection_.target = viewProjection_.eye + forward;

	//���[���h����x�N�g��
	Vector3 Up(0, 1, 0);
	//���[���J�����̉�]�𔽉f
	viewProjection_.up = Vector3TransformNormal(Up, worldTransform_.matWorld_);
	
	//�r���[�v���W�F�N�V�������X�V
	viewProjection_.UpdateMatrix();

	DebugText* debugText_ = DebugText::GetInstance();
	//�f�o�b�O�p�\��
	debugText_->SetPos(20, 100);
	//debugText_->Printf("RailCamera:Pos(%f,%f,%f)", viewProjection_.eye.x,
		//viewProjection_.eye.y, viewProjection_.eye.z);
}