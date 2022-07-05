#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"
#include <random>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_;
	delete modelSkydome_;
	delete debugCamera_;
	delete player_;
	delete enemy_;
}

void GameScene::Initialize() {
	
	//ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("mario.jpg");
	//3Dモデルの生成
	model_ = Model::Create();
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	//ワールドトランスフォームの初期化
	player_ = new Player();
	player_->Initialize(model_, textureHandle_);
	enemy_ = new Enemy();
	enemy_->Initialize(model_, textureHandle_);
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_);

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	enemy_->SetPlayer(player_);

	viewProjection_.Initialize();
	//デバッグカメラの作成
	debugCamera_ = new DebugCamera(1280, 720);
	
	
}

void GameScene::Update() //視点移動処理
{
	skydome_->Update();
	player_->Update();
	enemy_->Update();
	//skydome_->Update();
	CheckAllCollistions();

	//デバック用表示
	debugText_->SetPos(50, 50);
	debugText_->Printf("eye:(%f,%f,%f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);
}

void GameScene::Draw() {
	
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
	
#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);
	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	//3Dモデル描画
	player_->Draw(viewProjection_);
	enemy_->Draw(viewProjection_);
	skydome_->Draw(viewProjection_);
	
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
void GameScene::CheckAllCollistions() {
	//判定対象AとBの座標
	Vector3 posA, posB;

	//自弾リストの取得
	const std::list<std::unique_ptr<PlayerBullet>>& playerBullets = player_->GetBullets();
	//敵弾リストの取得
	const std::list<std::unique_ptr<EnemyBullet>>& enemyBullets = enemy_->GetBullets();

#pragma region 自キャラと敵弾の当たり判定
	//自キャラの座標
	posA = { player_->GetWorldPosition() };
	//自キャラと敵弾すべての当たり判定
	for (const std::unique_ptr<EnemyBullet>& bullet : enemyBullets) {
		//敵弾の座標
		posB = { bullet->GetWorldPosition() };

		float r1 = 1.5f;
		float r2 = 1.5f;
		float x = (posA.x - posB.x) * (posA.x - posB.x);
		float y = (posA.y - posB.y) * (posA.y - posB.y);
		float z = (posA.z - posB.z) * (posA.z - posB.z);
		//球と玉の交差判定
		if ((x + y + z) <= (r1 + r2) * (r1 + r2)) {
			//自キャラの衝突時コールバック関数を呼び出す
			player_->OnCollition();
			//敵弾の衝突時コールバック関数を呼び出す
			bullet->OnCollition();
		}
	}
#pragma endregion

#pragma region 自弾と敵キャラの当たり判定
	//敵キャラの座標
	posA = { enemy_->GetWorldPosition() };
	//自弾と敵弾キャラの当たり判定
	for (const std::unique_ptr<PlayerBullet>& PlayerBullet : playerBullets) {
		//自弾の座標
		posB = { PlayerBullet->GetWorldPosition() };

		float r1 = 1.5f;
		float r2 = 1.5f;
		float x = (posA.x - posB.x) * (posA.x - posB.x);
		float y = (posA.y - posB.y) * (posA.y - posB.y);
		float z = (posA.z - posB.z) * (posA.z - posB.z);
		//球と玉の交差判定
		if ((x + y + z) <= (r1 + r2) * (r1 + r2)) {
			//敵キャラの衝突時コールバック関数を呼び出す
			enemy_->OnCollition();
			//自弾の衝突時コールバック関数を呼び出す
			PlayerBullet->OnCollition();
		}
	}
#pragma endregion

#pragma region 自弾と敵弾の当たり判定
	//自弾と敵弾キャラの当たり判定
	for (const std::unique_ptr<PlayerBullet>& PlayerBullet : playerBullets) {
		for (const std::unique_ptr<EnemyBullet>& bullet : enemyBullets) {
			//自弾の座標
			posA = { bullet->GetWorldPosition() };
			posB = { PlayerBullet->GetWorldPosition() };
			float r1 = 1.5f;
			float r2 = 1.5f;
			float x = (posA.x - posB.x) * (posA.x - posB.x);
			float y = (posA.y - posB.y) * (posA.y - posB.y);
			float z = (posA.z - posB.z) * (posA.z - posB.z);
			//球と玉の交差判定
			if ((x + y + z) <= (r1 + r2) * (r1 + r2)) {
				//自弾の衝突時コールバック関数を呼び出す
				PlayerBullet->OnCollition();
				//敵弾の衝突時コールバック関数を呼び出す
				bullet->OnCollition();
			}
		}
	}
#pragma endregion
}
