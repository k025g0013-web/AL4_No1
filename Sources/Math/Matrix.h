#pragma once

#include <cmath>
#include <cassert>

#include "KamataEngine.h"

#ifndef MATH_EPSILON_DEFINED
#define MATH_EPSILON_DEFINED
// 浮動小数点の比較に使用する許容誤差
inline constexpr float kEpsilon = 1e-4f;
#endif

//==============================
// 行列型ごとのサイズ情報
//==============================

// 行列の行数・列数を取得するためのトレイト
template <typename T> struct MatrixTraits;

// 4×4行列
template <>
struct MatrixTraits<KamataEngine::Matrix4x4> {
	static constexpr size_t Rows = 4;
	static constexpr size_t Cols = 4;
};

//==============================
// 計算関数群
//==============================

// 行列同士を加算する
template <typename MatrixN>
inline MatrixN AddMatrix(const MatrixN &m1, const MatrixN &m2) {
	MatrixN result = {};

	// 行列のサイズを取得する
	constexpr size_t rows = MatrixTraits<MatrixN>::Rows;
	constexpr size_t cols = MatrixTraits<MatrixN>::Cols;

	// 各要素同士を加算する
	for (size_t row = 0; row < rows; row++) {
		for (size_t column = 0; column < cols; column++) {
			result.m[row][column] = m1.m[row][column] + m2.m[row][column];
		}
	}

	return result;
}

// 行列同士を減算する
template <typename MatrixN>
inline MatrixN SubtractMatrix(const MatrixN &m1, const MatrixN &m2) {
	MatrixN result = {};

	// 行列のサイズを取得する
	constexpr size_t rows = MatrixTraits<MatrixN>::Rows;
	constexpr size_t cols = MatrixTraits<MatrixN>::Cols;

	// 各要素同士を減算する
	for (size_t row = 0; row < rows; row++) {
		for (size_t column = 0; column < cols; column++) {
			result.m[row][column] = m1.m[row][column] - m2.m[row][column];
		}
	}

	return result;
}

// 行列同士を乗算する
template <typename MatrixN>
inline MatrixN Multiply(const MatrixN &m1, const MatrixN &m2) {
	MatrixN result = {};

	// 行列のサイズを取得する
	constexpr size_t rows = MatrixTraits<MatrixN>::Rows;
	constexpr size_t cols = MatrixTraits<MatrixN>::Cols;

	// 行列積を計算する
	for (size_t row = 0; row < rows; row++) {
		for (size_t column = 0; column < cols; column++) {
			for (size_t k = 0; k < cols; k++) {
				result.m[row][column] += m1.m[row][k] * m2.m[k][column];
			}
		}
	}

	return result;
}

// 転置行列を作成する
template <typename MatrixN>
inline MatrixN Transpose(const MatrixN &m) {
	MatrixN result = {};

	// 行列のサイズを取得する
	constexpr size_t rows = MatrixTraits<MatrixN>::Rows;
	constexpr size_t cols = MatrixTraits<MatrixN>::Cols;

	// 行と列を入れ替える
	for (size_t row = 0; row < rows; row++) {
		for (size_t column = 0; column < cols; column++) {
			result.m[row][column] = m.m[column][row];
		}
	}

	return result;
}

// 単位行列を作成する
template <typename MatrixN>
inline MatrixN MakeIdentity() {
	MatrixN result = {};

	// 行数を取得する
	constexpr size_t rows = MatrixTraits<MatrixN>::Rows;

	// 対角成分のみ1.0、それ以外は0.0にする
	for (size_t i = 0; i < rows; i++) {
		result.m[i][i] = 1.0f;
	}

	return result;
}
//==============================
// 3Dグラフィックス専用関数群 (Matrix4x4)
//==============================

// 3×3行列の行列式を求める
inline float Det3(
	float a1, float a2, float a3,
	float b1, float b2, float b3,
	float c1, float c2, float c3) {

	// サラスの公式を用いて行列式を計算する
	return
		a1 * (b2 * c3 - b3 * c2) -
		a2 * (b1 * c3 - b3 * c1) +
		a3 * (b1 * c2 - b2 * c1);
}

// 4×4行列の逆行列を求める
inline KamataEngine::Matrix4x4 Inverse(const KamataEngine::Matrix4x4 &m) {
	KamataEngine::Matrix4x4 result = {};

	// 余因子行列を作成する
	KamataEngine::Matrix4x4 cofactor = {};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {

			// 小行列(3×3)を格納する
			float sub[3][3]{};
			int r = 0;

			// 指定した行・列を除いた3×3行列を作成する
			for (int row = 0; row < 4; ++row) {
				if (row == i) continue;

				int c = 0;
				for (int col = 0; col < 4; ++col) {
					if (col == j) continue;

					sub[r][c] = m.m[row][col];
					c++;
				}
				r++;
			}

			// 小行列の行列式(余因子の元)を求める
			float minor = Det3(
				sub[0][0], sub[0][1], sub[0][2],
				sub[1][0], sub[1][1], sub[1][2],
				sub[2][0], sub[2][1], sub[2][2]
			);

			// 位置に応じた符号を付けて余因子を求める
			float sign = ((i + j) % 2 == 0) ? 1.0f : -1.0f;
			cofactor.m[i][j] = sign * minor;
		}
	}

	// 余因子行列を転置して随伴行列を作成する
	KamataEngine::Matrix4x4 adjugate = {};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			adjugate.m[i][j] = cofactor.m[j][i];
		}
	}

	// 元の行列の行列式を求める
	float det = 0.0f;
	for (int j = 0; j < 4; ++j) {
		det += m.m[0][j] * cofactor.m[0][j];
	}

	// 行列式が0に近い場合は逆行列が存在しない
	if (std::fabs(det) < kEpsilon) {
		return result;
	}

	// 行列式の逆数を求める
	float invDet = 1.0f / det;

	// 随伴行列を行列式で割って逆行列を求める
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = adjugate.m[i][j] * invDet;
		}
	}

	return result;
}

// 平行移動行列を作成する
inline KamataEngine::Matrix4x4 MakeTranslateMatrix(const KamataEngine::Vector3 &translate) {

	// 単位行列を基に作成する
	KamataEngine::Matrix4x4 result = MakeIdentity<KamataEngine::Matrix4x4>();

	// 平行移動量を設定する
	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;

	return result;
}

// 拡大縮小行列を作成する
inline KamataEngine::Matrix4x4 MakeScaleMatrix(const KamataEngine::Vector3 &scale) {

	// 単位行列を基に作成する
	KamataEngine::Matrix4x4 result = MakeIdentity<KamataEngine::Matrix4x4>();

	// 各軸の拡大率を対角成分へ設定する
	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;
	result.m[2][2] = scale.z;

	return result;
}

// X軸回転行列を作成する
inline KamataEngine::Matrix4x4 MakeRotateXMatrix(float radian) {

	// 単位行列を基に作成する
	KamataEngine::Matrix4x4 result = MakeIdentity<KamataEngine::Matrix4x4>();

	// YZ平面を回転させる
	result.m[1][1] = std::cos(radian);
	result.m[1][2] = std::sin(radian);

	result.m[2][1] = -std::sin(radian);
	result.m[2][2] = std::cos(radian);

	return result;
}

// Y軸回転行列を作成する
inline KamataEngine::Matrix4x4 MakeRotateYMatrix(float radian) {

	// 単位行列を基に作成する
	KamataEngine::Matrix4x4 result = MakeIdentity<KamataEngine::Matrix4x4>();

	// XZ平面を回転させる
	result.m[0][0] = std::cos(radian);
	result.m[0][2] = -std::sin(radian);

	result.m[2][0] = std::sin(radian);
	result.m[2][2] = std::cos(radian);

	return result;
}

// Z軸回転行列を作成する
inline KamataEngine::Matrix4x4 MakeRotateZMatrix(float radian) {

	// 単位行列を基に作成する
	KamataEngine::Matrix4x4 result = MakeIdentity<KamataEngine::Matrix4x4>();

	// XY平面を回転させる
	result.m[0][0] = std::cos(radian);
	result.m[0][1] = std::sin(radian);

	result.m[1][0] = -std::sin(radian);
	result.m[1][1] = std::cos(radian);

	return result;
}

// アフィン変換行列を作成する
inline KamataEngine::Matrix4x4 MakeAffineMatrix(
	const KamataEngine::Vector3 &scale,
	const KamataEngine::Vector3 &rotation,
	const KamataEngine::Vector3 &translation) {

	// 拡大縮小行列を作成する
	KamataEngine::Matrix4x4 s = MakeScaleMatrix(scale);

	// X→Y→Zの順に回転行列を合成する
	KamataEngine::Matrix4x4 r = Multiply(
		Multiply(
			MakeRotateXMatrix(rotation.x),
			MakeRotateYMatrix(rotation.y)),
		MakeRotateZMatrix(rotation.z)
	);

	// 平行移動行列を作成する
	KamataEngine::Matrix4x4 t = MakeTranslateMatrix(translation);

	// Scale→Rotate→Translateの順でアフィン変換行列を生成する
	return Multiply(Multiply(s, r), t);
}

// 透視投影行列を作成する
inline KamataEngine::Matrix4x4 MakePerspectiveFovMatrix(
	float fovY,
	float aspectRatio,
	float nearClip,
	float farClip) {

	KamataEngine::Matrix4x4 result = {};

	// 視野角から投影係数を計算する
	float fov = 1.0f / std::tan(fovY / 2.0f);

	// X方向の拡大率(アスペクト比を考慮)
	result.m[0][0] = fov / aspectRatio;

	// Y方向の拡大率
	result.m[1][1] = fov;

	// 深度値を正規化する係数
	result.m[2][2] = farClip / (farClip - nearClip);

	// 透視除算を行うための成分
	result.m[2][3] = 1.0f;

	// ニア・ファークリップを考慮したZ方向の平行移動
	result.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);

	return result;
}

// 正射影行列を作成する
inline KamataEngine::Matrix4x4 MakeOrthographicMatrix(
	float left,
	float top,
	float right,
	float bottom,
	float nearClip,
	float farClip) {

	KamataEngine::Matrix4x4 result = {};

	// 各軸を正規化する倍率
	result.m[0][0] = 2.0f / (right - left);
	result.m[1][1] = 2.0f / (top - bottom);
	result.m[2][2] = 1.0f / (farClip - nearClip);
	result.m[3][3] = 1.0f;

	// 描画範囲の中心を原点へ移動する
	result.m[3][0] = -(right + left) / (right - left);
	result.m[3][1] = -(top + bottom) / (top - bottom);
	result.m[3][2] = -nearClip / (farClip - nearClip);

	return result;
}

// ビューポート変換行列を作成する
inline KamataEngine::Matrix4x4 MakeViewportMatrix(
	float left,
	float top,
	float width,
	float height,
	float minDepth,
	float maxDepth) {

	KamataEngine::Matrix4x4 result = {};

	// 正規化デバイス座標を画面サイズへ拡大する
	result.m[0][0] = width / 2.0f;
	result.m[1][1] = -height / 2.0f;
	result.m[2][2] = maxDepth - minDepth;
	result.m[3][3] = 1.0f;

	// 描画領域の左上座標へ移動する
	result.m[3][0] = left + width / 2.0f;
	result.m[3][1] = top + height / 2.0f;
	result.m[3][2] = minDepth;

	return result;
}
