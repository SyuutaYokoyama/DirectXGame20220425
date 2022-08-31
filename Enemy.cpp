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
Vector3 Enemy::GetWorldPosition() {
	//���[���h���W������ϐ�
	Vector3 worldPos;
	//���[���h�s��̕��s�ړ��������擾�i���[���h���W�j

	worldPos.x = worldTransforms_[2].translation_.x;
	worldPos.y = worldTransforms_[2].translation_.y;
	worldPos.z = worldTransforms_[2].translation_.z;
	return worldPos;
}
Vector3 Enemy::GetWorldPosition2() {
	//���[���h���W������ϐ�
	Vector3 worldPos;
	//���[���h�s��̕��s�ړ��������擾�i���[���h���W�j

	worldPos.x = worldTransforms_[0].translation_.x;
	worldPos.y = worldTransforms_[0].translation_.y;
	worldPos.z = worldTransforms_[0].translation_.z;
	return worldPos;
}
Vector3 Enemy::GetWorldPosition3() {
	//���[���h���W������ϐ�
	Vector3 worldPos;
	//���[���h�s��̕��s�ړ��������擾�i���[���h���W�j

	worldPos.x = worldTransforms_[1].translation_.x;
	worldPos.y = worldTransforms_[1].translation_.y;
	worldPos.z = worldTransforms_[1].translation_.z;
	return worldPos;
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
	for (int i = 0; i < 3; i++) {
		worldTransforms_[i].Initialize();
	}
	//�������W
	worldTransforms_[0].translation_ = {18.0f,-4.0f,10.0f};
	worldTransforms_[1].translation_ = { 0.0f,-4.0f,10.0f };
	worldTransforms_[2].translation_ = { -18.0f,-4.0f,10.0f };
	//�e�𔭎�
	//Fire();
	//�ڋ߃t�F�[�Y������
	ApproachInitialize();
	//�e�N�X�`���ǂݍ���
	textureHandle_ = TextureManager::Load("redEnemy.png");
}
void Enemy::Update() {
	Vector3 EnemyApproachSpeed = { 0,0,0 };
	//Vector3 EnemyLeaveSpeed = { 0,0.1f,0 };
	/*switch (phase_) {
	case Phase::Approach:
	default:
		Approach(worldTransforms_[0], EnemyApproachSpeed);
		Approach(worldTransforms_[1], EnemyApproachSpeed);
		Approach(worldTransforms_[2], EnemyApproachSpeed);
		break;
	case Phase::Leave:
		Leave(worldTransforms_[0], EnemyLeaveSpeed);
		Leave(worldTransforms_[1], EnemyLeaveSpeed);
		Leave(worldTransforms_[2], EnemyLeaveSpeed);
		break;
	}*/
	const float EnemySpeed = 0.01f;
	//worldTransform_.translation_ .z -= EnemySpeed;
	//const float EnemySpeed = 0.2f;
	//worldTransform_.translation_ .z -= EnemySpeed;
	Approach(worldTransforms_[0], EnemyApproachSpeed);
	Approach(worldTransforms_[1], EnemyApproachSpeed);
	Approach(worldTransforms_[2], EnemyApproachSpeed);
	EnemyBullets_.remove_if([](std::unique_ptr<EnemyBullet>& bullet) {
		return bullet->IsDead();
		});
	//Attack();
	for (std::unique_ptr<EnemyBullet>& bullet : EnemyBullets_) {
		bullet->Update();
	}
	for (int i = 0; i < 3; i++) {
		//�s��X�V
		Matrix4 matIdentity;
		matIdentity.m[0][0] = 1;
		matIdentity.m[1][1] = 1;
		matIdentity.m[2][2] = 1;
		matIdentity.m[3][3] = 1;

		Matrix4 matScale = matIdentity;
		matScale.m[0][0] = worldTransforms_[i].scale_.x;
		matScale.m[1][1] = worldTransforms_[i].scale_.y;
		matScale.m[2][2] = worldTransforms_[i].scale_.z;
		matScale.m[3][3] = 1;

		Matrix4 matRotZ = matIdentity;
		matRotZ.m[0][0] = cos(worldTransforms_[i].rotation_.z);
		matRotZ.m[0][1] = sin(worldTransforms_[i].rotation_.z);
		matRotZ.m[1][0] = -sin(worldTransforms_[i].rotation_.z);
		matRotZ.m[1][1] = cos(worldTransforms_[i].rotation_.z);

		Matrix4 matRotX = matIdentity;
		matRotX.m[1][1] = cos(worldTransforms_[i].rotation_.x);
		matRotX.m[1][2] = sin(worldTransforms_[i].rotation_.x);
		matRotX.m[2][1] = -sin(worldTransforms_[i].rotation_.x);
		matRotX.m[2][2] = cos(worldTransforms_[i].rotation_.x);

		Matrix4 matRotY = matIdentity;
		matRotY.m[0][0] = cos(worldTransforms_[i].rotation_.y);
		matRotY.m[0][2] = sin(worldTransforms_[i].rotation_.y);
		matRotY.m[2][0] = -sin(worldTransforms_[i].rotation_.y);
		matRotY.m[2][2] = cos(worldTransforms_[i].rotation_.y);

		Matrix4 matTrans = matIdentity;
		matTrans.m[3][0] = worldTransforms_[i].translation_.x;
		matTrans.m[3][1] = worldTransforms_[i].translation_.y;
		matTrans.m[3][2] = worldTransforms_[i].translation_.z;
		matTrans.m[3][3] = 1;

		worldTransforms_[i].matWorld_ = matIdentity;
		worldTransforms_[i].matWorld_ *= matScale;

		worldTransforms_[i].matWorld_ *= matRotZ;
		worldTransforms_[i].matWorld_ *= matRotX;
		worldTransforms_[i].matWorld_ *= matRotY;

		worldTransforms_[i].matWorld_ *= matTrans;
		//�s��]��
		worldTransforms_[i].TransferMatrix();
	}
}
void Enemy::Draw(ViewProjection viewProjection) {
	model_->Draw(worldTransforms_[0], viewProjection, textureHandle_);
	model_->Draw(worldTransforms_[1], viewProjection, textureHandle_);
	model_->Draw(worldTransforms_[2], viewProjection, textureHandle_);
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
	//worldTransform_.translation_ += EnemyLeaveSpeed;
}
void Enemy::Fire() {
	assert(player_);
	//if (input_->TriggerKey(DIK_SPACE)) {
		//�e�̑��x
	const float kBulletSpeed = 0.3f;
	Vector3 velocity(0, 0, -kBulletSpeed);
	Vector3 velocity2(0.0f, 0, -kBulletSpeed);
	Vector3 velocity3(0.0f, 0, -kBulletSpeed);
	player_->GetWorldPosition();
	GetWorldPosition();
	GetWorldPosition2();
	GetWorldPosition3();
	velocity = { player_->GetWorldPosition().x - GetWorldPosition().x
		,player_->GetWorldPosition().y - GetWorldPosition().y
		,player_->GetWorldPosition().z - GetWorldPosition().z
	};
	velocity2 = { player_->GetWorldPosition().x - GetWorldPosition2().x
		,player_->GetWorldPosition().y - GetWorldPosition2().y
		,player_->GetWorldPosition().z - GetWorldPosition2().z
	};
	velocity3 = { player_->GetWorldPosition().x - GetWorldPosition3().x
		,player_->GetWorldPosition().y - GetWorldPosition3().y
		,player_->GetWorldPosition().z - GetWorldPosition3().z
	};
	float nagasa = sqrtf(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z);
	float nagasa2 = sqrtf(velocity2.x * velocity2.x + velocity2.y * velocity2.y + velocity2.z * velocity2.z);
	float nagasa3 = sqrtf(velocity3.x * velocity3.x + velocity3.y * velocity3.y + velocity3.z * velocity3.z);
	velocity /= nagasa;
	velocity *= kBulletSpeed;
	velocity2 /= nagasa2;
	velocity2 *= kBulletSpeed;
	velocity3 /= nagasa3;
	velocity3 *= kBulletSpeed;
		//�e�𐶐����A������
		std::unique_ptr<EnemyBullet>newEnemyBullet = std::make_unique<EnemyBullet>();
		std::unique_ptr<EnemyBullet>newEnemyBullet2 = std::make_unique<EnemyBullet>();
		std::unique_ptr<EnemyBullet>newEnemyBullet3 = std::make_unique<EnemyBullet>();
		//PlayerBullet* newBullet = new PlayerBullet();
		newEnemyBullet->Initialize(model_, worldTransforms_[0].translation_, velocity2);
		newEnemyBullet2->Initialize(model_, worldTransforms_[1].translation_, velocity3);
		newEnemyBullet3->Initialize(model_, worldTransforms_[2].translation_, velocity);
		//�e��o�^����
		//bullet_ = newBullet;
		//bullet_.reset(newBullet);
		EnemyBullets_.push_back(std::move(newEnemyBullet));
		EnemyBullets_.push_back(std::move(newEnemyBullet2));
		EnemyBullets_.push_back(std::move(newEnemyBullet3));
	//}
}
void Enemy::ApproachInitialize() {
	//���˃^�C�}�[��������
	FireCount = kFireInterval;
}
void Enemy::OnCollition() {
	isDead_ = true;
}

