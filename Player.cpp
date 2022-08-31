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
	//ワールド座標を入れる変数
	Vector3 worldPos;
	//ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;
	return worldPos;
}
void Player::Initialize(Model* model, uint32_t textureHandle) {
	//NULLポインタチェック
	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	//シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
	debugText_ = DebugText::GetInstance();

	worldTransform_.Initialize();
	worldTransform_.translation_.z = -20;

}
void Player::Update() {
	//キャラクターの移動ベクトル
	Vector3 move = { 0,0,0 };
	//視点の移動速さ
	const float kEyeSpeed = 0.2f;
	//上方向の回転速さ[ラジアン/frame]
	const float kUpSpeed = 0.05f;

	//デスフラグの立った弾を削除
	bullets_.remove_if([](std::unique_ptr<PlayerBullet>& bullet) {
		return bullet->IsDead();
		});

	//キーボード入力による移動処理
	//押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_W)) {
		move = { 0,kEyeSpeed,0 };
	}else if (input_->PushKey(DIK_S)) {
		move = { 0,-kEyeSpeed,0 };
	}
	//押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_A)) {
		move = { -kEyeSpeed, 0,0 };
	}else if (input_->PushKey(DIK_D)) {
		move = { kEyeSpeed, 0,0, };
	}

	//上半身回転処理
	{
		//押した方向で移動ベクトルを変更
		if (input_->PushKey(DIK_U)) {
			worldTransform_.rotation_.y -= kUpSpeed;
		}
		else if (input_->PushKey(DIK_I)) {
			worldTransform_.rotation_.y += kUpSpeed;
		}
	}

	//座標移動(ベクトルの加算)
	worldTransform_.translation_.x += move.x;
	worldTransform_.translation_.y += move.y;

	//移動限界座標
	const float kMoveLimitX = 34.0f;
	const float kMoveLimitY = 18.0f;

	//範囲を超えない処理
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);

	//キャラクター攻撃処理
	Attack();

	//弾更新
	/*if (bullet_) {
		bullet_->Update();
	}*/
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Update();
	}

	//行列更新
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

	//行列転送
	worldTransform_.TransferMatrix();

	//デバッグテキスト
	debugText_->SetPos(50, 150);
	//debugText_->Printf("translation:(%f,%f,%f)", worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z);
}
void Player::Draw(ViewProjection viewProjection) {
	model_->Draw(worldTransform_,viewProjection,textureHandle_);
	/*if (bullet_) {
		bullet_->Draw(viewProjection);
	}*/
	//弾描画
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
}
void Player::Attack() {
	if (input_->TriggerKey(DIK_SPACE)) {
		//弾の速度
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);
		//速度ベクトルを自機の向きに合わせて回転させる
		velocity = Velocity(velocity, worldTransform_);
		//弾を生成し、初期化
		std::unique_ptr<PlayerBullet>newBullet = std::make_unique<PlayerBullet>();
		//PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, worldTransform_.translation_, velocity);
		//弾を登録する
		//bullet_ = newBullet;
		//bullet_.reset(newBullet);
		bullets_.push_back(std::move(newBullet));
	}
}
void Player::OnCollition() {}
