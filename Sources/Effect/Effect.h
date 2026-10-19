#pragma once

class Effect {
public:
	virtual ~Effect() = default;

	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual bool IsDead() const = 0;
};