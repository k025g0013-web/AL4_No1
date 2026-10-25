#include "TitleScene.h"
using namespace KamataEngine;

#include "Game/Menu/TitleMenu.h"
#include "Math/UpdateWorldTransform.h"

TitleScene::~TitleScene() {
	delete cameraManager_;

	delete skydome_;

	delete modelPlayer_;
}

void TitleScene::Initialize(SceneManager *sceneManager) {
	isFinished_ = false;	// 終了フラグ初期化

	// カメラマネージャーを生成
	cameraManager_ = new CameraManager();
	cameraManager_->Initialize(nullptr);
	cameraManager_->Reset();

	// シーンマネージャ取得
	sceneManager_ = sceneManager;

	// テクスチャ読み込み
	graphHandleTitleLogo_ = TextureManager::Load("titleLogo.png");

	// スプライト生成
	spriteTitleLogo_ = Sprite::Create(graphHandleTitleLogo_, { 100,25 }, { 1.0f, 1.0f, 1.0f, 1.0f });

	// モデル読み込み
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	modelPlayer_ = Model::CreateFromOBJ("player", true);

	// モデル生成
	skydome_ = new Skydome;
	skydome_->Initialize(modelSkydome_, &cameraManager_->GetCamera());

	playerWorldTransform_.Initialize();
	playerWorldTransform_.translation_ =	{ 8.0f, 0.0f, -25.0f };
	playerWorldTransform_.rotation_ =		{ 0.0f, 0.0f, 0.0f };
	playerWorldTransform_.scale_ =			{ 3.0f, 3.0f, 3.0f };

	// タイトルメニュー初期化
	titleMenu_.Initialize();
}

void TitleScene::Update() {
	// 天球
	skydome_->Update();

	playerWorldTransform_.rotation_.y += 0.02f;
	UpdateWorldTransform(playerWorldTransform_);

	// メニュー操作更新
	titleMenu_.Update();

	// メニューで選択されたものに合わせてゲームの進行先を変化
	if (titleMenu_.IsDecided()) {
		if (titleMenu_.GetSelectedItem() == 0) {
			// Startだった場合はゲーム開始
			isFinished_ = true;
		} else if (titleMenu_.GetSelectedItem() == 1) {
			// Exitだった場合はゲーム終了
			sceneManager_->RequestGameEnd();
		}
	}
}

void TitleScene::Draw() {
	// モデル描画
	//=========================
	Model::PreDraw(); // 開始

	// 天球
	skydome_->Draw();

	modelPlayer_->Draw(playerWorldTransform_, cameraManager_->GetCamera());

	Model::PostDraw(); // 終了


	// スプライト描画
	//=========================
	Sprite::PreDraw(); // 開始

	spriteTitleLogo_->Draw();
	titleMenu_.Draw();

	Sprite::PostDraw(); // 終了
}

bool TitleScene::IsFinished() const { return isFinished_; }