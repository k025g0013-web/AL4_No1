#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>

// 動かし方は下記のイメージ
// KamataEngine::Vector2 a = Easing::Interpolate(EasingType::easeInBack, 0.0f, Vector2{ 0.0f, 0.0f }, Vector2{ 10.0f, 10.0f });
// KamataEngine::Vector3 a = Easing::Interpolate(EasingType::easeInBack, 0.0f, Vector3{ 0.0f, 0.0f }, Vector3{ 10.0f, 10.0f });
// KamataEngine::Vector4 a = Easing::Interpolate(EasingType::easeInBack, 0.0f, Vector4{ 0.0f, 0.0f }, Vector4{ 10.0f, 10.0f });

enum class EasingType {
	// 等速補間
	Linear,

	// Sine系イージング
	easeInSine, easeOutSine, easeInOutSine,

	// 2次関数系イージング
	easeInQuad, easeOutQuad, easeInOutQuad,

	// 3次関数系イージング
	easeInCubic, easeOutCubic, easeInOutCubic,

	// 4次関数系イージング
	easeInQuart, easeOutQuart, easeInOutQuart,

	// 5次関数系イージング
	easeInQuint, easeOutQuint, easeInOutQuint,

	// 指数関数系イージング
	easeInExpo, easeOutExpo, easeInOutExpo,

	// 円運動系イージング
	easeInCirc, easeOutCirc, easeInOutCirc,

	// オーバーシュートするイージング
	easeInBack, easeOutBack, easeInOutBack,

	// バネのように振動するイージング
	easeInElastic, easeOutElastic, easeInOutElastic,

	// バウンドするイージング
	easeInBounce, easeOutBounce, easeInOutBounce,
};

/// <summary>
/// 様々なイージング補間を提供するユーティリティクラス
/// </summary>
/// <remarks>
/// インスタンスを生成せず、すべて静的関数として利用する。
/// Interpolate()から各種イージング関数を選択して補間を行う。
/// </remarks>
class Easing {
public:
	/// <summary>
	/// インスタンス化を禁止する
	/// </summary>
	Easing() = delete;

	/// <summary>
	/// 指定したイージングで値を補間する
	/// </summary>
	/// <typeparam name="T">
	/// floatやVector2、Vector3など四則演算に対応した型
	/// </typeparam>
	/// <param name="type">使用するイージングの種類</param>
	/// <param name="t">補間率(0.0～1.0)</param>
	/// <param name="start">開始値</param>
	/// <param name="end">終了値</param>
	/// <returns>補間後の値</returns>
	template <typename T>
	static T Interpolate(EasingType type, float t, const T &start, const T &end);

private:
#pragma region イージング関数群

	// Sine系イージング
	static float InSine(float t);
	static float OutSine(float t);
	static float InOutSine(float t);

	// Quad系イージング
	static float InQuad(float t);
	static float OutQuad(float t);
	static float InOutQuad(float t);

	// Cubic系イージング
	static float InCubic(float t);
	static float OutCubic(float t);
	static float InOutCubic(float t);

	// Quart系イージング
	static float InQuart(float t);
	static float OutQuart(float t);
	static float InOutQuart(float t);

	// Quint系イージング
	static float InQuint(float t);
	static float OutQuint(float t);
	static float InOutQuint(float t);

	// Expo系イージング
	static float InExpo(float t);
	static float OutExpo(float t);
	static float InOutExpo(float t);

	// Circ系イージング
	static float InCirc(float t);
	static float OutCirc(float t);
	static float InOutCirc(float t);

	// Back系イージング
	static float InBack(float t);
	static float OutBack(float t);
	static float InOutBack(float t);

	// Elastic系イージング
	static float InElastic(float t);
	static float OutElastic(float t);
	static float InOutElastic(float t);

	// Bounce系イージング
	static float OutBounce(float t);
	static float InBounce(float t);
	static float InOutBounce(float t);

#pragma endregion
};