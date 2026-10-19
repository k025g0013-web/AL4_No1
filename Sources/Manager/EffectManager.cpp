#include "EffectManager.h"

#include "Effect/HitEffect.h"
#include "Effect/GuardEffect.h"

using namespace KamataEngine;

EffectManager::~EffectManager() {
	Clear();
}

void EffectManager::Initialize(
	Model *hitModel,
	Model *guardModel,
	Camera *camera
) {
	// 再初期化時に残っているエフェクトを破棄
	Clear();

	// 各エフェクトのモデルとカメラを設定
	HitEffect::SetModel(hitModel);
	HitEffect::SetCamera(camera);

	GuardEffect::SetModel(guardModel);
	GuardEffect::SetCamera(camera);
}

void EffectManager::Update() {
	for (Effect *effect : effects_) {
		effect->Update();
	}

	// 終了したエフェクトを削除
	effects_.remove_if([](Effect *effect) {
		if (effect->IsDead()) {
			delete effect;
			return true;
		}
		return false;
		});
}

void EffectManager::Draw() {
	for (Effect *effect : effects_) {
		effect->Draw();
	}
}

void EffectManager::Clear() {
	for (Effect *effect : effects_) {
		delete effect;
	}

	effects_.clear();
}