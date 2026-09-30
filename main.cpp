#include "KamataEngine.h"
#include <Windows.h>

#include "Scene/SceneManager.h"

using namespace KamataEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// インスタンス生成
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();   // DirectXCommon
	ImGuiManager* imguiManager = ImGuiManager::GetInstance(); // ImGuiManager

	// エンジンの初期化
	KamataEngine::Initialize(L"Title");

	// SceneManager生成/初期化
	SceneManager* sceneManager = new SceneManager();
	sceneManager->Initialize();// メインループ
	while (true) {
		// エンジンの更新
		if (Update()) {
			break;
		}

		// 更新処理
		//=========================
		imguiManager->Begin(); // ImGui:開始

		// シーンの更新
		sceneManager->Update();

		imguiManager->End(); // ImGui:終了

		// 描画処理
		//=========================
		dxCommon->PreDraw(); // DirectX:開始

		// 軸方向表示
		AxisIndicator::GetInstance()->Draw();

		// シーンの描画
		sceneManager->Draw();

		imguiManager->Draw();

		dxCommon->PostDraw(); // DirectX:終了

		// 強制終了コマンド(ESC)
		if (sceneManager->IsGameEnd()) {
			break;
		}
	}

	// SceneManagerの解放
	delete sceneManager;
	sceneManager = nullptr;

	// エンジンの終了処理
	KamataEngine::Finalize();
	return 0;
}