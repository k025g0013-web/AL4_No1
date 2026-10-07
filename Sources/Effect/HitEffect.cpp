#define _USE_MATH_DEFINES
#include "HitEffect.h"
#include "Math/UpdateWorldTransform.h"
#include <random>
#include <cmath>

using namespace KamataEngine;

// 静的メンバ変数
Model* HitEffect::model_ = nullptr;
Camera* HitEffect::camera_ = nullptr;

// ライフサイクル
HitEffect::HitEffect() {}
HitEffect::~HitEffect() {}

#pragma region 基本関数
void HitEffect::Initialize(const Vector3& position) {
	HitEffect::SetCamera(camera_);
	HitEffect::SetModel(model_);

	// 乱数生成
	static std::random_device seedGenerator;
	static std::mt19937_64 randomEngine(seedGenerator());

	std::uniform_real_distribution<float> rotationDistribution(-static_cast<float>(M_PI), static_cast<float>(M_PI));

	// 円エフェクト
	circleWorldTransform_.Initialize();
	circleWorldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	circleWorldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	circleWorldTransform_.translation_ = position;

	// 楕円エフェクト
	for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
		worldTransform.scale_ = {3.0f, 0.1f, 1.0f};
		worldTransform.rotation_ = {
			0.0f, 0.0f, rotationDistribution(randomEngine)
		};
		
		worldTransform.translation_ = position;

		worldTransform.Initialize();
	}

	// 状態遷移
	behavior_ = Behavior::kSpread;
	behaviorRequest_ = Behavior::kUnknown;

	BehaviorSpreadInitialize();
}

void HitEffect::Update() { 
	if (behaviorRequest_ != Behavior::kUnknown) {

		behavior_ = behaviorRequest_;

		switch (behavior_) {

		case Behavior::kSpread:
			BehaviorSpreadInitialize();
			break;

		case Behavior::kFade:
			BehaviorFadeInitialize();
			break;
		}

		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {

	case Behavior::kSpread:
		BehaviorSpreadUpdate();
		break;

	case Behavior::kFade:
		BehaviorFadeUpdate();
		break;
	}

	UpdateWorldTransform(circleWorldTransform_);

	for (auto& wt : ellipseWorldTransforms_) {
		UpdateWorldTransform(wt);
	}
}

void HitEffect::Draw() { 
	if (isDead_ || alpha_ <= 0.0f) return;

	model_->SetAlpha(alpha_);

	model_->Draw(circleWorldTransform_, *camera_);
	for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
		model_->Draw(worldTransform, *camera_);
	}
}
#pragma endregion

// インスタンス生成・初期化
HitEffect* HitEffect::Create(const Vector3& position) {
	// 生成
	HitEffect* instance = new HitEffect();
#ifdef _DEBUG
	assert(instance);	// 失敗を検出
#endif 

	// 初期化
	instance->Initialize(position);
	return instance;
}

#pragma region 状態遷移
void HitEffect::BehaviorSpreadInitialize() { counter_ = 0.0f; }
void HitEffect::BehaviorSpreadUpdate() {

	counter_ = (std::min)(counter_ + 1.0f / 60.0f, spreadDuration_);

	float t = std::clamp(counter_ / spreadDuration_, 0.0f, 1.0f);

	circleWorldTransform_.scale_ = {1.0f + t, 1.0f + t, 1.0f};

	for (auto& wt : ellipseWorldTransforms_) {
		wt.scale_.x = 3.0f + 2.0f * t;
		wt.scale_.y = 0.3f + 0.3f * t;
	}

	if (counter_ >= spreadDuration_) {
		behaviorRequest_ = Behavior::kFade;
	}
}

void HitEffect::BehaviorFadeInitialize() { counter_ = 0.0f; }
void HitEffect::BehaviorFadeUpdate() {

	counter_ = (std::min)(counter_ + 1.0f / 60.0f, fadeDuration_);

	alpha_ = std::clamp(1.0f - counter_ / fadeDuration_, 0.0f, 1.0f);

	if (counter_ >= fadeDuration_) {
		isDead_ = true;
	}
}
#pragma endregion

