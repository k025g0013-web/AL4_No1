#include "Easing.h"

#include "KamataEngine.h"
using namespace KamataEngine;

#include "Vector.h"

static float InterpolateValue(float start, float end, float rate) {
    return start + (end - start) * rate;
}

static Vector2 InterpolateValue(const Vector2 &start, const Vector2 &end, float rate) {
    Vector2 result{};

    result.x = start.x + (end.x - start.x) * rate;
    result.y = start.y + (end.y - start.y) * rate;

    return result;
}

static Vector3 InterpolateValue(const Vector3 &start, const Vector3 &end, float rate) {
    Vector3 result{};

    result.x = start.x + (end.x - start.x) * rate;
    result.y = start.y + (end.y - start.y) * rate;
    result.z = start.z + (end.z - start.z) * rate;

    return result;
}

static Vector4 InterpolateValue(const Vector4 &start, const Vector4 &end, float rate) {
    Vector4 result{};

    result.x = start.x + (end.x - start.x) * rate;
    result.y = start.y + (end.y - start.y) * rate;
    result.z = start.z + (end.z - start.z) * rate;
    result.w = start.w + (end.w - start.w) * rate;

    return result;
}

template <typename T>
T Easing::Interpolate(EasingType type, float t, const T &start, const T &end) {
    t = std::clamp(t, 0.0f, 1.0f);

    float rate = t;

    switch (type) {
    case EasingType::Linear:
        rate = t;
        break;

    case EasingType::easeInSine:
        rate = InSine(t);
        break;

    case EasingType::easeOutSine:
        rate = OutSine(t);
        break;

    case EasingType::easeInOutSine:
        rate = InOutSine(t);
        break;

    case EasingType::easeInQuad:
        rate = InQuad(t);
        break;

    case EasingType::easeOutQuad:
        rate = OutQuad(t);
        break;

    case EasingType::easeInOutQuad:
        rate = InOutQuad(t);
        break;

    case EasingType::easeInCubic:
        rate = InCubic(t);
        break;

    case EasingType::easeOutCubic:
        rate = OutCubic(t);
        break;

    case EasingType::easeInOutCubic:
        rate = InOutCubic(t);
        break;

    case EasingType::easeInQuart:
        rate = InQuart(t);
        break;

    case EasingType::easeOutQuart:
        rate = OutQuart(t);
        break;

    case EasingType::easeInOutQuart:
        rate = InOutQuart(t);
        break;

    case EasingType::easeInQuint:
        rate = InQuint(t);
        break;

    case EasingType::easeOutQuint:
        rate = OutQuint(t);
        break;

    case EasingType::easeInOutQuint:
        rate = InOutQuint(t);
        break;

    case EasingType::easeInExpo:
        rate = InExpo(t);
        break;

    case EasingType::easeOutExpo:
        rate = OutExpo(t);
        break;

    case EasingType::easeInOutExpo:
        rate = InOutExpo(t);
        break;

    case EasingType::easeInCirc:
        rate = InCirc(t);
        break;

    case EasingType::easeOutCirc:
        rate = OutCirc(t);
        break;

    case EasingType::easeInOutCirc:
        rate = InOutCirc(t);
        break;

    case EasingType::easeInBack:
        rate = InBack(t);
        break;

    case EasingType::easeOutBack:
        rate = OutBack(t);
        break;

    case EasingType::easeInOutBack:
        rate = InOutBack(t);
        break;

    case EasingType::easeInElastic:
        rate = InElastic(t);
        break;

    case EasingType::easeOutElastic:
        rate = OutElastic(t);
        break;

    case EasingType::easeInOutElastic:
        rate = InOutElastic(t);
        break;

    case EasingType::easeInBounce:
        rate = InBounce(t);
        break;

    case EasingType::easeOutBounce:
        rate = OutBounce(t);
        break;

    case EasingType::easeInOutBounce:
        rate = InOutBounce(t);
        break;
    }

    return InterpolateValue(start, end, rate);
}

#pragma region イージング関数群

//=============================================================================
// Sine
//=============================================================================

// ゆっくり加速する補間
float Easing::InSine(float t) {
	return 1.0f - std::cos((t * static_cast<float>(M_PI)) / 2.0f);
}

// ゆっくり減速する補間
float Easing::OutSine(float t) {
	return std::sin((t * static_cast<float>(M_PI)) / 2.0f);
}

// ゆっくり加速し、ゆっくり減速する補間
float Easing::InOutSine(float t) {
	return -(std::cos(static_cast<float>(M_PI) * t) - 1.0f) / 2.0f;
}

//=============================================================================
// Quad
//=============================================================================

// 2次関数による加速補間
float Easing::InQuad(float t) {
	return t * t;
}

// 2次関数による減速補間
float Easing::OutQuad(float t) {
	return t * (2.0f - t);
}

// 2次関数による加減速補間
float Easing::InOutQuad(float t) {
	return t < 0.5f ?
		2.0f * t * t :
		1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;
}

//=============================================================================
// Cubic
//=============================================================================

// 3次関数による加速補間
float Easing::InCubic(float t) {
	return t * t * t;
}

// 3次関数による減速補間
float Easing::OutCubic(float t) {
	float invT = 1.0f - t;
	return 1.0f - (invT * invT * invT);
}

// 3次関数による加減速補間
float Easing::InOutCubic(float t) {
	return t < 0.5f ?
		4.0f * t * t * t :
		1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
}

//=============================================================================
// Quart
//=============================================================================

// 4次関数による加速補間
float Easing::InQuart(float t) {
	float t2 = t * t;
	return t2 * t2;
}

// 4次関数による減速補間
float Easing::OutQuart(float t) {
	float invT = 1.0f - t;
	float invT2 = invT * invT;
	return 1.0f - (invT2 * invT2);
}

// 4次関数による加減速補間
float Easing::InOutQuart(float t) {
	return t < 0.5f ?
		8.0f * t * t * t * t :
		1.0f - std::pow(-2.0f * t + 2.0f, 4.0f) / 2.0f;
}

//=============================================================================
// Quint
//=============================================================================

// 5次関数による加速補間
float Easing::InQuint(float t) {
	float t2 = t * t;
	return t2 * t2 * t;
}

// 5次関数による減速補間
float Easing::OutQuint(float t) {
	return 1.0f - std::pow(1.0f - t, 5.0f);
}

// 5次関数による加減速補間
float Easing::InOutQuint(float t) {
	return t < 0.5f ?
		16.0f * t * t * t * t * t :
		1.0f - std::pow(-2.0f * t + 2.0f, 5.0f) / 2.0f;
}

//=============================================================================
// Expo
//=============================================================================

// 指数関数による加速補間
float Easing::InExpo(float t) {
	return t == 0.0f ? 0.0f : std::pow(2.0f, 10.0f * t - 10.0f);
}

// 指数関数による減速補間
float Easing::OutExpo(float t) {
	return t == 1.0f ? 1.0f : 1.0f - std::pow(2.0f, -10.0f * t);
}

// 指数関数による加減速補間
float Easing::InOutExpo(float t) {
	if (t == 0.0f) return 0.0f;
	if (t == 1.0f) return 1.0f;

	return t < 0.5f ?
		std::pow(2.0f, 20.0f * t - 10.0f) / 2.0f :
		(2.0f - std::pow(2.0f, -20.0f * t + 10.0f)) / 2.0f;
}

//=============================================================================
// Circ
//=============================================================================

// 円運動を利用した加速補間
float Easing::InCirc(float t) {
	return 1.0f - std::sqrt(1.0f - (t * t));
}

// 円運動を利用した減速補間
float Easing::OutCirc(float t) {
	float tMinus1 = t - 1.0f;
	return std::sqrt(1.0f - (tMinus1 * tMinus1));
}

// 円運動を利用した加減速補間
float Easing::InOutCirc(float t) {
	return t < 0.5f ?
		(1.0f - std::sqrt(1.0f - std::pow(2.0f * t, 2.0f))) / 2.0f :
		(std::sqrt(1.0f - std::pow(-2.0f * t + 2.0f, 2.0f)) + 1.0f) / 2.0f;
}

//=============================================================================
// Back
//=============================================================================

// Back・Elastic・Bounce用の定数
namespace {
	constexpr float C1 = 1.70158f;
	constexpr float C2 = C1 * 1.525f;
	constexpr float C3 = C1 + 1.0f;
	constexpr float C4 = (2.0f * static_cast<float>(M_PI)) / 3.0f;
	constexpr float C5 = (2.0f * static_cast<float>(M_PI)) / 4.5f;
}

// 一度逆方向へ動いてから加速する補間
float Easing::InBack(float t) {
	return C3 * t * t * t - C1 * t * t;
}

// 行き過ぎてから戻る減速補間
float Easing::OutBack(float t) {
	float tMinus1 = t - 1.0f;
	return 1.0f + C3 * tMinus1 * tMinus1 * tMinus1 + C1 * tMinus1 * tMinus1;
}

// Backによる加減速補間
float Easing::InOutBack(float t) {
	return t < 0.5f ?
		(std::pow(2.0f * t, 2.0f) * ((C2 + 1.0f) * 2.0f * t - C2)) / 2.0f :
		(std::pow(2.0f * t - 2.0f, 2.0f) * ((C2 + 1.0f) * (t * 2.0f - 2.0f) + C2) + 2.0f) / 2.0f;
}

//=============================================================================
// Elastic
//=============================================================================

// バネのように振動しながら加速する補間
float Easing::InElastic(float t) {
	if (t == 0.0f) return 0.0f;
	if (t == 1.0f) return 1.0f;

	return -std::pow(2.0f, 10.0f * t - 10.0f) *
		std::sin((t * 10.0f - 10.75f) * C4);
}

// バネのように振動しながら減速する補間
float Easing::OutElastic(float t) {
	if (t == 0.0f) return 0.0f;
	if (t == 1.0f) return 1.0f;

	return std::pow(2.0f, -10.0f * t) *
		std::sin((t * 10.0f - 0.75f) * C4) + 1.0f;
}

// バネのような加減速補間
float Easing::InOutElastic(float t) {
	if (t == 0.0f) return 0.0f;
	if (t == 1.0f) return 1.0f;

	return t < 0.5f ?
		-(std::pow(2.0f, 20.0f * t - 10.0f) * std::sin((20.0f * t - 11.125f) * C5)) / 2.0f :
		(std::pow(2.0f, -20.0f * t + 10.0f) * std::sin((20.0f * t - 11.125f) * C5)) / 2.0f + 1.0f;
}

//=============================================================================
// Bounce
//=============================================================================

// ボールが跳ね返るような減速補間
float Easing::OutBounce(float t) {
	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	if (t < 1.0f / d1) {
		return n1 * t * t;
	} else if (t < 2.0f / d1) {
		t -= 1.5f / d1;
		return n1 * t * t + 0.75f;
	} else if (t < 2.5f / d1) {
		t -= 2.25f / d1;
		return n1 * t * t + 0.9375f;
	} else {
		t -= 2.625f / d1;
		return n1 * t * t + 0.984375f;
	}
}

// Bounceの加速補間
float Easing::InBounce(float t) {
	return 1.0f - OutBounce(1.0f - t);
}

// Bounceによる加減速補間
float Easing::InOutBounce(float t) {
	return t < 0.5f ?
		(1.0f - OutBounce(1.0f - 2.0f * t)) / 2.0f :
		(1.0f + OutBounce(2.0f * t - 1.0f)) / 2.0f;
}

#pragma endregion

// 使用する型のテンプレートを明示的に生成する
template float Easing::Interpolate<float>(EasingType, float, const float &, const float &);
template Vector2 Easing::Interpolate<Vector2>(EasingType, float, const Vector2 &, const Vector2 &);
template Vector3 Easing::Interpolate<Vector3>(EasingType, float, const Vector3 &, const Vector3 &);
template Vector4 Easing::Interpolate<Vector4>(EasingType, float, const Vector4 &, const Vector4 &);