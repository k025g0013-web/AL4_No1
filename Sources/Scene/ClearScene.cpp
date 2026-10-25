#include "ClearScene.h"
#include "KamataEngine.h"

using namespace KamataEngine;

ClearScene::ClearScene() {}

ClearScene::~ClearScene() {}

void ClearScene::Initialize() {
	isFinished_ = false;

	// カメラマネージャーを生成
	cameraManager_ = new CameraManager();
	cameraManager_->Initialize(nullptr);
	cameraManager_->Reset();

	// テクスチャ読み込み
	graphHandleGameClearText_ = TextureManager::Load("gameClearText.png");
	graphHandleBackTitleText_ = TextureManager::Load("backTitleText.png");

	// スプライト生成
	spriteGameClearText_ = Sprite::Create(graphHandleGameClearText_, { 280,25 });
	spriteBackTitleText_ = Sprite::Create(graphHandleBackTitleText_, { 400.0f, 450.0f });

	// モデル読み込み
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	// モデル生成
	skydome_ = new Skydome;
	skydome_->Initialize(modelSkydome_, &cameraManager_->GetCamera());
}

void ClearScene::Update() {
	// 天球
	skydome_->Update();

	// タイトルにもどる
	sineTimer_ += 1.0f / 60.0f;

	float offsetBackTitle =
		std::sin(sineTimer_ * sineSpeed_ + 1.0f) * sineAmplitude_;

	spriteBackTitleText_->SetPosition({400.0f, 450.0f + offsetBackTitle});

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
        isFinished_ = true;
    }
}

void ClearScene::Draw() {
	// モデル描画
	//=========================
	Model::PreDraw(); // 開始

	// 天球
	skydome_->Draw();

	Model::PostDraw(); // 終了


	// スプライト描画
	//=========================
	Sprite::PreDraw(); // 開始

	spriteGameClearText_->Draw();
	spriteBackTitleText_->Draw();

	Sprite::PostDraw(); // 終了
}

bool ClearScene::IsFinished() const {
	return isFinished_;
}