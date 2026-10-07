#define _USE_MATH_DEFINES
#include "GuardEffect.h"
#include "Math/UpdateWorldTransform.h"
#include <random>
#include <cmath>

using namespace KamataEngine;

// 静的メンバ変数
Model* GuardEffect::model_ = nullptr;
Camera* GuardEffect::camera_ = nullptr;

// ライフサイクル
GuardEffect::GuardEffect() {}
GuardEffect::~GuardEffect() {}

#pragma region 基本関数
void GuardEffect::Initialize(const Vector3& position) {
	GuardEffect::SetCamera(camera_);
	GuardEffect::SetModel(model_);

	// 乱数生成
	static std::random_device seedGenerator;
	static std::mt19937_64 randomEngine(seedGenerator());

	std::uniform_real_distribution<float> rotationDistribution(-static_cast<float>(M_PI), static_cast<float>(M_PI));

	// 円エフェクト
	circleWorldTransform_.Initialize();
	circleWorldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	circleWorldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	circleWorldTransform_.translation_ = position;

	// 状態遷移
	behavior_ = Behavior::kSpread;
	behaviorRequest_ = Behavior::kUnknown;

	BehaviorSpreadInitialize();
}

void GuardEffect::Update() { 
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
}

void GuardEffect::Draw() { 
	if (isDead_ || alpha_ <= 0.0f) return;

	model_->SetAlpha(alpha_);

	// リングのみ描画
	model_->Draw(circleWorldTransform_, *camera_);
}
#pragma endregion

// インスタンス生成・初期化
GuardEffect* GuardEffect::Create(const Vector3& position) {
	// 生成
	GuardEffect* instance = new GuardEffect();
#ifdef _DEBUG
	assert(instance);	// 失敗を検出
#endif 

	// 初期化
	instance->Initialize(position);
	return instance;
}

#pragma region 状態遷移
void GuardEffect::BehaviorSpreadInitialize() { counter_ = 0.0f; }
void GuardEffect::BehaviorSpreadUpdate() {

	counter_ = (std::min)(counter_ + 1.0f / 60.0f, spreadDuration_);

	float t = std::clamp(counter_ / spreadDuration_, 0.0f, 1.0f);

	// リングが一気に広がる
	float scale = 0.8f + 5.0f * t;

	circleWorldTransform_.scale_ = {
		scale,
		scale,
		scale
	};

	if (counter_ >= spreadDuration_) {
		behaviorRequest_ = Behavior::kFade;
	}
}

void GuardEffect::BehaviorFadeInitialize() { counter_ = 0.0f; }
void GuardEffect::BehaviorFadeUpdate() {

	counter_ = (std::min)(counter_ + 1.0f / 60.0f, fadeDuration_);

	alpha_ = std::clamp(1.0f - counter_ / fadeDuration_, 0.0f, 1.0f);

	if (counter_ >= fadeDuration_) {
		isDead_ = true;
	}
}
#pragma endregion

