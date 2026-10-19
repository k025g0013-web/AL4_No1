#pragma once

#include <list>
#include <type_traits>

#include "KamataEngine.h"
#include "Effect/Effect.h"

class EffectManager {
public:
	EffectManager() = default;
	~EffectManager();

	void Initialize(
		KamataEngine::Model *hitModel,
		KamataEngine::Model *guardModel,
		KamataEngine::Camera *camera
	);

	void Update();
	void Draw();
	void Clear();

	// エフェクトの種類を指定して生成
	template <typename T>
	void Create(const KamataEngine::Vector3 &position) {
		static_assert(
			std::is_base_of_v<Effect, T>,
			"T must inherit from Effect"
			);

		effects_.push_back(T::Create(position));
	}

private:
	std::list<Effect *> effects_;
};