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

//自機クラスの前方宣言
class Player;
/// <summary>
/// 自キャラ
/// </summary>
class Enemy {
public:
	//発射間隔
	static const int kFireInterval = 15;
	enum class Phase {
		Approach, //接近する
		Leave,    //離脱する
	};
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, uint32_t textureHandle);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ViewProjection viewProjection);
	/// <summary>
	void Approach(WorldTransform& worldTransform_, Vector3& EnemyApproachSpeed);
	void Leave(WorldTransform& worldTransform_, Vector3& EnemyLraveSpeed);

	void Fire();
	//接近フェーズ初期化
	void ApproachInitialize();
	Vector3 Velocity(Vector3 velocity, WorldTransform worldTransform_);
	void SetPlayer(Player* player) { player_ = player; }
	//ワールド座標を取得
	Vector3 GetWorldPosition();
private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// 入力処理するため 
	Input* input_ = nullptr;
	// デバッグテキスト
	DebugText* debugText_ = nullptr;
	//フェーズ
	Phase phase_ = Phase::Approach;
	std::list<std::unique_ptr<EnemyBullet>>EnemyBullets_;
	//発射タイマー
	int32_t FireCount = 0;
	//自キャラ
	Player* player_ = nullptr;
};

