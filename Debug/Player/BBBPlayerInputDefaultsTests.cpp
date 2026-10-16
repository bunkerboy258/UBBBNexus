#include "CoreMinimal.h"

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "BBBWork/UBBBNexus/PlayerCamera/BBBPlayerCameraSystem.h"
#include "Game/BBBGameMode.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/PlayerCamera/BBBPlayerCameraSystem.h"
#include "BBBWork/UBBBNexus/PlayerInput/BBBPlayerInputSystem.h"
#include "EnhancedInputComponent.h"

/** 验证默认玩法生成的控制器能够绑定输入并提供玩家相机配置 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBPlayerInputDefaultsTest, "UBBB.PlayerInput.DefaultControllerBindings",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBPlayerInputDefaultsTest::RunTest(const FString& Parameters)
{
    const auto* Mode = GetDefault<ABBBGameMode>();
    const auto ControllerClass = Mode->PlayerControllerClass;
    if (!TestNotNull(TEXT("默认玩家控制器类"), ControllerClass.Get()))
    {
        return false;
    }
    if (!TestTrue(TEXT("默认控制器支持项目玩家输入"), ControllerClass->IsChildOf(ABBBPlayerController::StaticClass())))
    {
        return false;
    }
    auto* Controller = ControllerClass->GetDefaultObject<APlayerController>();
    auto* Input = Controller->FindComponentByClass<UBBBPlayerInputSystem>();
    if (!TestNotNull(TEXT("默认控制器输入组件"), Input))
    {
        return false;
    }
    const bool bMoveReady = TestNotNull(TEXT("移动输入动作"), Input->Config.MoveAction.Get());
    const bool bLookReady = TestNotNull(TEXT("视角输入动作"), Input->Config.LookAction.Get());
    TestNotNull(TEXT("玩家相机类"), Input->CameraClass.Get());
    if (!bMoveReady || !bLookReady)
    {
        return false;
    }
    auto* Bindings = NewObject<UEnhancedInputComponent>();
    Input->Bind(*Bindings);
    TestTrue(TEXT("默认控制器已建立增强输入事件绑定"), Bindings->GetActionEventBindings().Num() > 0);
    return true;
}

#endif
