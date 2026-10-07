#pragma once

#define _USE_MATH_DEFINES
#include <random>
#include <algorithm>
#include <cassert>

#include "KamataEngine.h"

#ifndef MATH_EPSILON_DEFINED
#define MATH_EPSILON_DEFINED
// 浮動小数点の比較に使用する許容誤差
inline constexpr float kEpsilon = 1e-4f;
#endif

//==============================
// ベクトル型ごとの次元情報
//==============================

// ベクトルの次元数を取得するためのトレイト
template <typename T> struct VectorTraits;

// Vector2は2次元ベクトル
template <> struct VectorTraits <KamataEngine::Vector2> {
	static constexpr size_t Dimensions = 2;
};

// Vector3は3次元ベクトル
template <> struct VectorTraits<KamataEngine::Vector3> {
	static constexpr size_t Dimensions = 3;
};

// Vector4は4次元ベクトル
template <> struct VectorTraits<KamataEngine::Vector4> {
	static constexpr size_t Dimensions = 4;
};

//==============================
// 計算関数群
//==============================

// Vector2
inline KamataEngine::Vector2 AddVector(
	const KamataEngine::Vector2 &v1,
	const KamataEngine::Vector2 &v2
) {
	return {
		v1.x + v2.x,
		v1.y + v2.y
	};
}

inline KamataEngine::Vector2 SubtractVector(
	const KamataEngine::Vector2 &v1,
	const KamataEngine::Vector2 &v2
) {
	return {
		v1.x - v2.x,
		v1.y - v2.y
	};
}

inline KamataEngine::Vector2 Multiply(
	float scalar,
	const KamataEngine::Vector2 &v
) {
	return {
		v.x * scalar,
		v.y * scalar
	};
}


// Vector3
inline KamataEngine::Vector3 AddVector(
	const KamataEngine::Vector3 &v1,
	const KamataEngine::Vector3 &v2
) {
	return {
		v1.x + v2.x,
		v1.y + v2.y,
		v1.z + v2.z
	};
}

inline KamataEngine::Vector3 SubtractVector(
	const KamataEngine::Vector3 &v1,
	const KamataEngine::Vector3 &v2
) {
	return {
		v1.x - v2.x,
		v1.y - v2.y,
		v1.z - v2.z
	};
}
inline KamataEngine::Vector3 Multiply(
	float scalar,
	const KamataEngine::Vector3 &v
) {
	return {
		v.x * scalar,
		v.y * scalar,
		v.z * scalar
	};
}


// Vector4
inline KamataEngine::Vector4 AddVector(
	const KamataEngine::Vector4 &v1,
	const KamataEngine::Vector4 &v2
) {
	return {
		v1.x + v2.x,
		v1.y + v2.y,
		v1.z + v2.z,
		v1.w + v2.w
	};
}

inline KamataEngine::Vector4 SubtractVector(
	const KamataEngine::Vector4 &v1,
	const KamataEngine::Vector4 &v2
) {
	return {
		v1.x - v2.x,
		v1.y - v2.y,
		v1.z - v2.z,
		v1.w - v2.w
	};
}

inline KamataEngine::Vector4 Multiply(
	float scalar,
	const KamataEngine::Vector4 &v
) {
	return {
		v.x * scalar,
		v.y * scalar,
		v.z * scalar,
		v.w * scalar
	};
}

// ベクトル同士の内積を求める
template <typename VectorN>
inline float Dot(const VectorN &v1, const VectorN &v2) {
	constexpr size_t dim =
		VectorTraits<VectorN>::Dimensions;

	// Vector2
	if constexpr (dim == 2) {
		return
			v1.x * v2.x +
			v1.y * v2.y;

		// Vector3
	} else if constexpr (dim == 3) {
		return
			v1.x * v2.x +
			v1.y * v2.y +
			v1.z * v2.z;

		// Vector4
	} else if constexpr (dim == 4) {
		return
			v1.x * v2.x +
			v1.y * v2.y +
			v1.z * v2.z +
			v1.w * v2.w;
	}
}

// ベクトルの長さの二乗を求める
template <typename VectorN>
inline float LengthSquare(const VectorN &v) {

	// 自分自身との内積を利用する
	return Dot(v, v);
}

// ベクトルの長さ(ノルム)を求める
template <typename VectorN>
inline float Length(const VectorN &v) {

	// 長さの二乗の平方根を求める
	return std::sqrtf(LengthSquare(v));
}

// ベクトルを正規化する
template <typename VectorN>
inline VectorN Normalize(const VectorN &v) {

	// ベクトルの長さを取得する
	float length = Length(v);

	// 長さがほぼ0なら0ベクトルを返す
	if (length < kEpsilon) {
		return {};
	}

	// 長さが1になるよう各成分を割る
	return Multiply(1.0f / length, v);
}

//==============================
// 特定次元専用関数群
//==============================

// 3次元ベクトル同士のクロス積を求める
template <typename Vector3T>
inline Vector3T Cross(const Vector3T &v1, const Vector3T &v2) {

	// Vector3以外では使用できないようにする
	static_assert(VectorTraits<Vector3T>::Dimensions == 3, "Cross product is only supported for Vector3.");

	Vector3T result = {};

	// x成分
	result.x = v1.y * v2.z - v1.z * v2.y;

	// y成分
	result.y = v1.z * v2.x - v1.x * v2.z;

	// z成分
	result.z = v1.x * v2.y - v1.y * v2.x;

	return result;
}

// Vector3を4×4行列で座標変換する
inline KamataEngine::Vector3 TransformVector3(const KamataEngine::Vector3 &vector, const KamataEngine::Matrix4x4 &matrix) {
	KamataEngine::Vector3 result = {};

	// x座標を変換する
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + matrix.m[3][0];

	// y座標を変換する
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + matrix.m[3][1];

	// z座標を変換する
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + matrix.m[3][2];

	// 同次座標系のw成分を計算する
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + matrix.m[3][3];

	// wが0でないことを確認する
	assert(w != 0.0f);

	// 同次座標から3次元座標へ戻す
	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;
}

inline KamataEngine::Vector3 GetWorldPosition(const KamataEngine::WorldTransform &worldTransform) {
	KamataEngine::Vector3 worldPos{};
	// ワールド行列から平行移動成分を取得
	worldPos.x = worldTransform.matWorld_.m[3][0];
	worldPos.y = worldTransform.matWorld_.m[3][1];
	worldPos.z = worldTransform.matWorld_.m[3][2];
	return worldPos;
}