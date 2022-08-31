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
	//移動（ベクトルを加算）
	worldTransform_.translation_ += Vector3(0, 0, 0.00f);
	//ワールドトランスフォームの更新
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
	//ワールド座標の平行移動成分を取得
	viewProjection_.eye.x = worldTransform_.matWorld_.m[3][0];
	viewProjection_.eye.y = worldTransform_.matWorld_.m[3][1];
	viewProjection_.eye.z = worldTransform_.matWorld_.m[3][2];

	//ワールド前方ベクトル
	Vector3 forward(0, 0, 1);
	//レールカメラの回転を反映（レールカメラの回転ベクトル）
	forward = Vector3TransformNormal(forward, worldTransform_.matWorld_);
	//視点から前方に適当な距離進んだ位置が注視点
	viewProjection_.target = viewProjection_.eye + forward;

	//ワールド上方ベクトル
	Vector3 Up(0, 1, 0);
	//レールカメラの回転を反映
	viewProjection_.up = Vector3TransformNormal(Up, worldTransform_.matWorld_);
	
	//ビュープロジェクションを更新
	viewProjection_.UpdateMatrix();

	DebugText* debugText_ = DebugText::GetInstance();
	//デバッグ用表示
	debugText_->SetPos(20, 100);
	//debugText_->Printf("RailCamera:Pos(%f,%f,%f)", viewProjection_.eye.x,
		//viewProjection_.eye.y, viewProjection_.eye.z);
}