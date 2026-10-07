#pragma once

#include "KamataEngine.h"

// マップとの当たり判定情報
struct CollisionMapInfo {
	bool ceiling = false;
	bool landing = false;
	bool wall = false;
	KamataEngine::Vector3 move = {};
};

// 角
enum Corner {
	kRightBottom, // 右下
	kLeftBottom,  // 左下
	kRightTop,    // 右上
	kLeftTop,     // 左上

	kNumCorner // 要素数
};