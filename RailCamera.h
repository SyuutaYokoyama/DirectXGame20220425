#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "DebugText.h"

class RailCamera {
public:
	void Initialize(const Vector3& position,const Vector3& rotation);
	void Update();
	Vector3 Vector3TransformNormal(Vector3 velocity,Matrix4 matWorld);
	//ビュープロジェクションを取得
	const ViewProjection& GetViewProjection() { return viewProjection_; };
	//ワールド行列を取得
	const WorldTransform& GetWorldMatrix() { return worldTransform_; }
private:
	//ワールド変換データ
	WorldTransform worldTransform_;
	//ビュープロジェクション
	ViewProjection viewProjection_;
	// デバッグテキスト
	DebugText* debugText_ = nullptr;
};
