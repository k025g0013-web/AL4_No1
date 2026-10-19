#include "GameCamera.h"

void GameCamera::Initialize() {
    camera_.farZ = 2000.0f;
    camera_.Initialize();
}

void GameCamera::UpdateMatrix() {
    camera_.UpdateMatrix();
}