#include "GameOverScene.h"

#include "KamataEngine.h"
#include "Math/UpdateWorldTransform.h"

using namespace KamataEngine;

GameOverScene::~GameOverScene() {
    delete cameraManager_;

    delete skydome_;

	delete modelEnemy_;
}

void GameOverScene::Initialize() {
    isFinished_ = false;

	// カメラマネージャーを生成
	cameraManager_ = new CameraManager();
	cameraManager_->Initialize(nullptr);
	cameraManager_->Reset();

	// テクスチャ読み込み
	graphHandleGameOverText_ = TextureManager::Load("gameOverText.png");

	// スプライト生成
	spriteGameOver_ = Sprite::Create(graphHandleGameOverText_, { 250,25 }, { 1.0f, 1.0f, 1.0f, 1.0f });

	// モデル読み込み
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	modelEnemy_ = Model::CreateFromOBJ("enemy", true);

	// モデル生成
	skydome_ = new Skydome;
	skydome_->Initialize(modelSkydome_, &cameraManager_->GetCamera());

	enemyWorldTransform_[0].Initialize();
	enemyWorldTransform_[0].translation_ = { -12.0f, 0.0f, -25.0f };
	enemyWorldTransform_[0].rotation_ = { 0.0f, 0.0f, 0.0f };
	enemyWorldTransform_[0].scale_ = { 3.0f, 3.0f, 3.0f };

	enemyWorldTransform_[1].Initialize();
	enemyWorldTransform_[1].translation_ = { 12.0f, 0.0f, -25.0f };
	enemyWorldTransform_[1].rotation_ = { 0.0f, 0.0f, 0.0f };
	enemyWorldTransform_[1].scale_ = { 3.0f, 3.0f, 3.0f };

	// メニュー初期化
    gameOverMenu_.Initialize();

	// 初回描画前にワールド行列を更新
	UpdateWorldTransform(enemyWorldTransform_[0]);
	UpdateWorldTransform(enemyWorldTransform_[1]);
}

void GameOverScene::Update() {
	// 天球
	skydome_->Update();

	enemyWorldTransform_[0].rotation_.y -= 0.02f;
	UpdateWorldTransform(enemyWorldTransform_[0]);

	enemyWorldTransform_[1].rotation_.y += 0.02f;
	UpdateWorldTransform(enemyWorldTransform_[1]);

    gameOverMenu_.Update();

    if (gameOverMenu_.IsDecided()) {
        isFinished_ = true;
    }
}

void GameOverScene::Draw() {
	// モデル描画
	//=========================
	Model::PreDraw(); // 開始

	// 天球
	skydome_->Draw();

	modelEnemy_->Draw(enemyWorldTransform_[0], cameraManager_->GetCamera());
	modelEnemy_->Draw(enemyWorldTransform_[1], cameraManager_->GetCamera());

	Model::PostDraw(); // 終了


	// スプライト描画
	//=========================
	Sprite::PreDraw(); // 開始

	spriteGameOver_->Draw();
	gameOverMenu_.Draw();

	Sprite::PostDraw(); // 終了
}

bool GameOverScene::IsFinished() const {
    return isFinished_;
}
