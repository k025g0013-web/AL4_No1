#define NOMINMAX
#include "IParticle.h"

#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Math/UpdateWorldTransform.h"

#include <algorithm>
#include <cassert>

using namespace KamataEngine;

void IParticle::Initialize(Model *model, Camera *camera) {
#ifdef _DEBUG
	assert(model);
	assert(camera);
#endif

	// モデルとカメラを保持する
	model_ = model;
	camera_ = camera;

	for (auto &worldTransform : worldTransform_) {
		worldTransform.Initialize();
	}

	objectColor_.Initialize();
	color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
}

void IParticle::Draw() {
    if (finished_ || !model_ || !camera_) {
        return;
    }

    for (auto &worldTransform : worldTransform_) {
        model_->Draw(worldTransform, *camera_, &objectColor_);
    }
}