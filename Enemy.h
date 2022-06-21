#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "DebugText.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DebugCamera.h"
#include "EnemyBullet.h"
#include <memory>
#include <list>

//���@�N���X�̑O���錾
class Player;
/// <summary>
/// ���L����
/// </summary>
class Enemy {
public:
	//���ˊԊu
	static const int kFireInterval = 15;
	enum class Phase {
		Approach, //�ڋ߂���
		Leave,    //���E����
	};
	/// <summary>
	/// ������
	/// </summary>
	void Initialize(Model* model, uint32_t textureHandle);
	/// <summary>
	/// �X�V
	/// </summary>
	void Update();
	/// <summary>
	/// �`��
	/// </summary>
	void Draw(ViewProjection viewProjection);
	/// <summary>
	void Approach(WorldTransform& worldTransform_, Vector3& EnemyApproachSpeed);
	void Leave(WorldTransform& worldTransform_, Vector3& EnemyLraveSpeed);

	void Fire();
	//�ڋ߃t�F�[�Y������
	void ApproachInitialize();
	Vector3 Velocity(Vector3 velocity, WorldTransform worldTransform_);
	void SetPlayer(Player* player) { player_ = player; }
	//���[���h���W���擾
	Vector3 GetWorldPosition();
private:
	// ���[���h�ϊ��f�[�^
	WorldTransform worldTransform_;
	// ���f��
	Model* model_ = nullptr;
	// �e�N�X�`���n���h��
	uint32_t textureHandle_ = 0u;
	// ���͏������邽�� 
	Input* input_ = nullptr;
	// �f�o�b�O�e�L�X�g
	DebugText* debugText_ = nullptr;
	//�t�F�[�Y
	Phase phase_ = Phase::Approach;
	std::list<std::unique_ptr<EnemyBullet>>EnemyBullets_;
	//���˃^�C�}�[
	int32_t FireCount = 0;
	//���L����
	Player* player_ = nullptr;
};

