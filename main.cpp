#include "KamataEngine.h"
#include <Windows.h>

#include "Scene/SceneManager.h"
#include "StageManager.h"

using namespace KamataEngine;

// グローバル変数
StageManager *stageManager = nullptr; // ステージマネージャ

// 設定ファイルの読み込み
static void LoadDebugSettings() {
	std::ifstream file("debugSettings.ini");
	if (!file.is_open()) {
		return;
	}

	std::string line;
	while (std::getline(file, line)) {
		if (line.empty()) continue;

		std::stringstream lineStream(line);
		std::string key, value;

		if (lineStream >> key >> value) {
			if (key == "InitialStage") {
				stageManager->SetCurrentStageIndexByName(value);
			}
		}
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// インスタンス生成
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();   // DirectXCommon
	ImGuiManager* imguiManager = ImGuiManager::GetInstance(); // ImGuiManager

	// エンジンの初期化
	KamataEngine::Initialize(L"Title");

	stageManager = new StageManager;
	stageManager->LoadStageData();
	LoadDebugSettings();

	// SceneManager生成/初期化
	SceneManager* sceneManager = new SceneManager();
	sceneManager->Initialize(stageManager);// メインループ
	
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