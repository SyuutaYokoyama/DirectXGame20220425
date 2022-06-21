#include "Enemy.h"
#include "Player.h"
#include "TextureManager.h"
#include <cassert>
#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"
#include <random>
Vector3 Enemy::Velocity(Vector3 velocity, WorldTransform worldTransform_) {
	Vector3 v;
	v.x = velocity.x * worldTransform_.matWorld_.m[0][0] + velocity.y * worldTransform_.matWorld_.m[1][0] + velocity.z * worldTransform_.matWorld_.m[2][0];
	v.y = velocity.x * worldTransform_.matWorld_.m[0][1] + velocity.y * worldTransform_.matWorld_.m[1][1] + velocity.z * worldTransform_.matWorld_.m[2][1];
	v.z = velocity.x * worldTransform_.matWorld_.m[0][2] + velocity.y * worldTransform_.matWorld_.m[1][2] + velocity.z * worldTransform_.matWorld_.m[2][2];
	return v;
}
void Enemy::Initialize(Model* model, uint32_t textureHandle) {
	//NULL�|�C���^�`�F�b�N
	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	//�V���O���g���C���X�^���X���擾����
	input_ = Input::GetInstance();
	debugText_ = DebugText::GetInstance();
	//������
	worldTransform_.Initialize();
	//�������W
	worldTransform_.translation_ = { 6.0f,0.0f,10.0f };
	//�e�𔭎�
	//Fire();
	//�ڋ߃t�F�[�Y������
	ApproachInitialize();
	//�e�N�X�`���ǂݍ���
	textureHandle_ = TextureManager::Load("black.png");
}
void Enemy::Update() {
	Vector3 EnemyApproachSpeed = { 0,0,-0.2f };
	Vector3 EnemyLeaveSpeed = { -0.1f,0.1f,0 };
	/*switch (phase_) {
	case Phase::Approach:
	default:
		Approach(worldTransform_, EnemyApproachSpeed);
		break;
	case Phase::Leave:
		Leave(worldTransform_, EnemyLeaveSpeed);
		break;
	}*/
	const float EnemySpeed = 0.2f;
	//worldTransform_.translation_ .z -= EnemySpeed;
	//const float EnemySpeed = 0.2f;
	worldTransform_.translation_ .z -= EnemySpeed;
	Approach(worldTransform_, EnemyApproachSpeed);
	EnemyBullets_.remove_if([](std::unique_ptr<EnemyBullet>& bullet) {
		return bullet->IsDead();
		});
	//Attack();
	for (std::unique_ptr<EnemyBullet>& bullet : EnemyBullets_) {
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
	//�s��]��
	worldTransform_.TransferMatrix();
}
void Enemy::Draw(ViewProjection viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
	for (std::unique_ptr<EnemyBullet>& bullet : EnemyBullets_) {
		bullet->Draw(viewProjection);
	}
}
void Enemy::Approach(WorldTransform& worldTransform_, Vector3& EnemyApproachSpeed) {
	//�ړ��x�N�g�������Z
	worldTransform_.translation_ += EnemyApproachSpeed;
	//�K��̈ʒu�ɓ��B�����痣�E
	if (worldTransform_.translation_.z < 0.0f) {
		phase_ = Phase::Leave;
	}
	//���˃^�C�}�[�J�E���g�_�E��
	FireCount--;
	//�w�莞�ԂɒB����
	if (FireCount < 0) {
		//�e�𔭎�
		Fire();
		FireCount = kFireInterval;
	}
}
void Enemy::Leave(WorldTransform& worldTransform_, Vector3& EnemyLeaveSpeed) {
	//�ړ��i�x�N�g�������Z�j
	worldTransform_.translation_ += EnemyLeaveSpeed;
	worldTransform_.translation_ += EnemyLeaveSpeed;
}
void Enemy::Fire() {
	//if (input_->TriggerKey(DIK_SPACE)) {
		//�e�̑��x
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, -kBulletSpeed);
		
		//�e�𐶐����A������
		std::unique_ptr<EnemyBullet>newEnemyBullet = std::make_unique<EnemyBullet>();
		//PlayerBullet* newBullet = new PlayerBullet();
		newEnemyBullet->Initialize(model_, worldTransform_.translation_, velocity);
		//�e��o�^����
		//bullet_ = newBullet;
		//bullet_.reset(newBullet);
		EnemyBullets_.push_back(std::move(newEnemyBullet));
	//}
}
void Enemy::ApproachInitialize() {
	//���˃^�C�}�[��������
	FireCount = kFireInterval;
}
