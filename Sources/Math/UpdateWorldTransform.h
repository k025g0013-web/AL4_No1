#pragma once
#include "Matrix.h"

inline void UpdateWorldTransform(KamataEngine::WorldTransform &worldTransform) {
	// スケール、回転、並行移動を合成して行列を計算する
	worldTransform.matWorld_ = MakeAffineMatrix(
		worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_
	);

	// 定数バッファに転送する
	worldTransform.TransferMatrix();
};