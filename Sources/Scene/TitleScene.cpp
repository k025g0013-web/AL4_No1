#include "TitleScene.h"
using namespace KamataEngine;

#include "Menu/TitleMenu.h"

void TitleScene::Initialize(SceneManager *sceneManager) {
	isFinished_ = false;	// 終了フラグ初期化

	// シーンマネージャ取得
	sceneManager_ = sceneManager;

	// テクスチャ読み込み
	graphHandleTitleLogo_ = TextureManager::Load("titleLogo.png");

	// スプライト生成
	spriteTitleLogo_ = Sprite::Create(graphHandleTitleLogo_, {100,0}, { 0.5f, 0.5f, 0.5f, 1.0f });


	// タイトルメニュー初期化
	titleMenu_.Initialize();
}

void TitleScene::Update() {
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
	// スプライト描画
	//=========================
	Sprite::PreDraw(); // 開始

	spriteTitleLogo_->Draw();
	titleMenu_.Draw();

	Sprite::PostDraw(); // 終了


	// モデル描画
	//=========================
	Model::PreDraw(); // 開始
	Model::PostDraw(); // 終了
}

bool TitleScene::IsFinished() const { return isFinished_; }