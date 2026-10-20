#include "TitleLogo.h"
#include <cassert>

#include <math.h>
#include "Math/UpdateWorldTransform.h"

using namespace KamataEngine;

// コンストラクタ
TitleLogo::TitleLogo() {}

// デストラクタ
TitleLogo::~TitleLogo() { delete model_; }

void TitleLogo::Initialize(Model* model, Camera* camera) {
	// nullポインタチェック
	assert(model);

	// 引数の内容をメンバ変数に記録する
	model_ = model;

	// ワールド変換の初期化
	worldTransform_.Initialize();

	// 引数の内容をメンバ変数に記録
	camera_ = camera;

	// 初期位置設定
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};
}

void TitleLogo::Update() {

	// 上下移動
	moveTimer_ += 0.05f;	// 時間を進める
	float amplitude = 2.0f;	// 振れ幅
	worldTransform_.translation_.y = sinf(moveTimer_) * amplitude;

	UpdateWorldTransform(worldTransform_);
}

void TitleLogo::Draw() {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, *camera_);
}