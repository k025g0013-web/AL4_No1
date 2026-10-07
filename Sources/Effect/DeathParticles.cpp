#define NOMINMAX
#include "DeathParticles.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Math/UpdateWorldTransform.h"
#include <algorithm>

using namespace KamataEngine;

// コンストラクタ
DeathParticles::DeathParticles() {}
DeathParticles::~DeathParticles() { 
	delete model_; 
};

// 初期化処理
void DeathParticles::Initialize(Model* model, Camera* camera, const Vector3& position) {
	// nullポインタチェック
#ifdef _DEBUG
	assert(model);
#endif 

	// 引数の内容をメンバ変数に記録する
	model_ = model;

	// 引数の内容をメンバ変数に記録
	camera_ = camera;

	// ワールド変換の初期化
	for (auto& worldTransform : worldTransform_) {
		worldTransform.Initialize();
		worldTransform.translation_ = position;
	}

	// フェードアウト
	objectColor_.Initialize();
	color_ = {1, 1, 1, 1};
};

// 更新処理
void DeathParticles::Update() {
	// 終了なら何もしない
	if (finished_) {
		return;
	}

	// 移動
	for (uint32_t i = 0; i < kNumParticles; ++i) {
		// 基本となる速度ベクトル
		Vector3 velocity = {kSpeed, 0, 0};
		// 回転角を計算する
		float angle = kAngleUint * i;
		// Z軸まわり回転行列
		Matrix4x4 matrixRotation = MakeRotateZMatrix(angle);
		// 基本ベクトルを回転させて速度ベクトルを得る
		velocity = TransformVector3(velocity, matrixRotation);
		// 移動処理
		worldTransform_[i].translation_.x += velocity.x;
		worldTransform_[i].translation_.y += velocity.y;
		worldTransform_[i].translation_.z += velocity.z;
	}

	// カウンターを1フレーム分の秒数進める
	counter_ += 1.0f / 60.0f;

	// 経過時間の条件に達したら
	if (counter_ >= kDuration) {
		counter_ = kDuration;
		// 終了扱いにする
		finished_ = true;
	}

	color_.w = std::clamp(1.0f - counter_ / kDuration, 0.0f, 1.0f);
	// 色変更オブジェクトに色の数値を設定する
	objectColor_.SetColor(color_);

	// ワールド変換の更新
	for (auto& worldTransform : worldTransform_) {
		UpdateWorldTransform(worldTransform);
	}
};

// 描画処理
void DeathParticles::Draw() {
	// 終了なら何もしない
	if (finished_) {
		return;
	}

	// 3Dモデルを描画
	for (auto& worldTransform : worldTransform_) {
		model_->Draw(worldTransform, *camera_, &objectColor_);
	}
};