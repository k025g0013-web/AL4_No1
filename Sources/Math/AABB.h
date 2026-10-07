#pragma once
#include "KamataEngine.h"

// AABB
struct AABB {
	KamataEngine::Vector3 min = {};
	KamataEngine::Vector3 max = {};
};

inline AABB GetAABB(const KamataEngine::WorldTransform &worldTransform) {
	KamataEngine::Vector3 worldPos = worldTransform.translation_;

	AABB aabb;

	aabb.min = { worldPos.x - 1.9f / 2.0f, worldPos.y - 1.9f / 2.0f, worldPos.z - 1.9f / 2.0f };
	aabb.max = { worldPos.x + 1.9f / 2.0f, worldPos.y + 1.9f / 2.0f, worldPos.z + 1.9f / 2.0f };

	return aabb;
};