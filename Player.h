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
#include "PlayerBullet.h"
#include <memory>
#include <list>

/// <summary>
/// 自キャラ
/// </summary>
class Player {

public:
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
	/// 攻撃
	/// </summary>
	void Attack();
	Vector3 Velocity(Vector3 velocity,WorldTransform worldTransform_);
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
	//弾
	//PlayerBullet* bullet_ = nullptr;
	//std::unique_ptr<PlayerBullet> bullet_;
	std::list<std::unique_ptr<PlayerBullet>>bullets_;
};