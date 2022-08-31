#include "Player.h"
#include "TextureManager.h"
#include <cassert>
#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"
#include <random>
Vector3 Player::Velocity(Vector3 velocity, WorldTransform worldTransform_) {
	Vector3 v;
    v.x = velocity.x * worldTransform_.matWorld_.m[0][0] + velocity.y * worldTransform_.matWorld_.m[1][0] + velocity.z * worldTransform_.matWorld_.m[2][0];
	v.y = velocity.x * worldTransform_.matWorld_.m[0][1] + velocity.y * worldTransform_.matWorld_.m[1][1] + velocity.z * worldTransform_.matWorld_.m[2][1];
	v.z = velocity.x * worldTransform_.matWorld_.m[0][2] + velocity.y * worldTransform_.matWorld_.m[1][2] + velocity.z * worldTransform_.matWorld_.m[2][2];
	return v;
}
Vector3 Player::GetWorldPosition() {
	//���[���h���W������ϐ�
	Vector3 worldPos;
	//���[���h�s��̕��s�ړ��������擾�i���[���h���W�j
	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;
	return worldPos;
}
void Player::Initialize(Model* model, uint32_t textureHandle) {
	//NULL�|�C���^�`�F�b�N
	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	//�V���O���g���C���X�^���X���擾����
	input_ = Input::GetInstance();
	debugText_ = DebugText::GetInstance();

	worldTransform_.Initialize();
	worldTransform_.translation_.z = -20;

}
void Player::Update() {
	//�L�����N�^�[�̈ړ��x�N�g��
	Vector3 move = { 0,0,0 };
	//���_�̈ړ�����
	const float kEyeSpeed = 0.2f;
	//������̉�]����[���W�A��/frame]
	const float kUpSpeed = 0.05f;

	//�f�X�t���O�̗������e���폜
	bullets_.remove_if([](std::unique_ptr<PlayerBullet>& bullet) {
		return bullet->IsDead();
		});

	//�L�[�{�[�h���͂ɂ��ړ�����
	//�����������ňړ��x�N�g����ύX
	if (input_->PushKey(DIK_W)) {
		move = { 0,kEyeSpeed,0 };
	}else if (input_->PushKey(DIK_S)) {
		move = { 0,-kEyeSpeed,0 };
	}
	//�����������ňړ��x�N�g����ύX
	if (input_->PushKey(DIK_A)) {
		move = { -kEyeSpeed, 0,0 };
	}else if (input_->PushKey(DIK_D)) {
		move = { kEyeSpeed, 0,0, };
	}

	//�㔼�g��]����
	{
		//�����������ňړ��x�N�g����ύX
		if (input_->PushKey(DIK_U)) {
			worldTransform_.rotation_.y -= kUpSpeed;
		}
		else if (input_->PushKey(DIK_I)) {
			worldTransform_.rotation_.y += kUpSpeed;
		}
	}

	//���W�ړ�(�x�N�g���̉��Z)
	worldTransform_.translation_.x += move.x;
	worldTransform_.translation_.y += move.y;

	//�ړ����E���W
	const float kMoveLimitX = 34.0f;
	const float kMoveLimitY = 18.0f;

	//�͈͂𒴂��Ȃ�����
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);

	//�L�����N�^�[�U������
	Attack();

	//�e�X�V
	/*if (bullet_) {
		bullet_->Update();
	}*/
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Update();
	}

	//�s��X�V
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

	//worldTransform_.matWorld_ *= worldTransform_.parent_->matWorld_;

	//�s��]��
	worldTransform_.TransferMatrix();

	//�f�o�b�O�e�L�X�g
	debugText_->SetPos(50, 150);
	//debugText_->Printf("translation:(%f,%f,%f)", worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z);
}
void Player::Draw(ViewProjection viewProjection) {
	model_->Draw(worldTransform_,viewProjection,textureHandle_);
	/*if (bullet_) {
		bullet_->Draw(viewProjection);
	}*/
	//�e�`��
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
}
void Player::Attack() {
	if (input_->TriggerKey(DIK_SPACE)) {
		//�e�̑��x
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);
		//���x�x�N�g�������@�̌����ɍ��킹�ĉ�]������
		velocity = Velocity(velocity, worldTransform_);
		//�e�𐶐����A������
		std::unique_ptr<PlayerBullet>newBullet = std::make_unique<PlayerBullet>();
		//PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, worldTransform_.translation_, velocity);
		//�e��o�^����
		//bullet_ = newBullet;
		//bullet_.reset(newBullet);
		bullets_.push_back(std::move(newBullet));
	}
}
void Player::OnCollition() {}
