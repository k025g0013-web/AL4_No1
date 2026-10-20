#include "KamataEngine.h"
#include <Windows.h>

#include "Manager/SceneManager.h"
#include "Manager/StageManager.h"
#include "Manager/DebugSettingManager.h"

using namespace KamataEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// インスタンス生成
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();   // DirectXCommon
	ImGuiManager* imguiManager = ImGuiManager::GetInstance(); // ImGuiManager

	// エンジンの初期化
	KamataEngine::Initialize(L"Title");

	// ステージマネージャ
	StageManager *stageManager = nullptr; 
	stageManager = new StageManager;
	stageManager->LoadStageData();

	// デバッグ設定の読み込み・管理
	LoadDebugSettings(stageManager);

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