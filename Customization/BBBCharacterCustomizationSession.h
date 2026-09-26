#pragma once

#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "UObject/Object.h"
#include "BBBWork/UBBBNexus/Customization/Appearance/Data/BBBAppearanceSelection.h"
#include "BBBCharacterCustomizationSession.generated.h"

class AActor;
class APlayerController;
class ASceneCapture2D;
class FPreviewScene;
class UMaterialInterface;
class UBBBAppearanceComponent;
class UBBBCharacterCustomizationView;
class UTextureRenderTarget2D;

/**
 * 本地换装会话
 * 独立预览世界只负责显示草稿
 */
UCLASS(Config = Game)
class ABBB_EVAC_API UBBBCharacterCustomizationSession : public UObject
{
    GENERATED_BODY()

public:
    /**
     * 打开本地换装界面
     * @param Player	本地玩家控制器
     * @return 是否打开成功
     */
    bool Open(APlayerController &Player);

    /** @return 界面是否打开 */
    bool IsOpen() const;

    /**
     * 关闭界面并丢弃草稿
     * @return 无
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|Customization")
    void Close();

    /**
     * 释放独立预览世界
     * @return 无
     */
    void Shutdown();

    /**
     * 切换部位条目
     * @param Slot	部位名称
     * @param Direction	正数选择下一项 负数选择上一项
     * @return 是否预览成功
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|Customization")
    bool CycleItem(FName Slot, int32 Direction);

    /**
     * 切换身体或背心的徽章图案
     * @param Slot	部位名称
     * @param Direction	正数选择下一项 负数选择上一项
     * @return 是否预览成功
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|Customization")
    bool CyclePatch(FName Slot, int32 Direction);

    /**
     * 设置整体表面参数
     * @param Dirt	污渍强度
     * @param Weathering	磨损强度
     * @return 是否预览成功
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|Customization")
    bool SetSurface(float Dirt, float Weathering);

    /** @return 当前草稿 */
    UFUNCTION(BlueprintPure, Category = "BBB|Customization")
    FBBBAppearanceSelection GetDraft() const;

    /**
     * 应用并保存最终组合
     * @return 是否应用成功
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|Customization")
    bool Apply();

    /**
     * 选择预览机位
     * @param ViewName	Full Head 或 Legs
     * @return 无
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|Customization")
    void SelectView(FName ViewName);

    /**
     * 旋转预览人物
     * @param Degrees	水平旋转角度
     * @return 无
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|Customization")
    void RotatePreview(float Degrees);

    virtual void BeginDestroy() override;

private:
    bool CreatePreview();

    /** 创建仅由预览世界持有的灯光与背景 */
    bool ConfigurePreviewLighting();

    bool SetDraft(FBBBAppearanceSelection Selection);

    bool TickPreview(float DeltaTime);

    void UpdateCamera();

    void Capture();

    void CaptureInput(APlayerController &Player);

    void RestoreInput();

    /** 模块唯一的预览人物配置 */
    UPROPERTY(Config)
    TSoftClassPtr<AActor> PreviewActorClass;

    /** 独立预览世界的深蓝紫背景材质 */
    UPROPERTY(Config)
    TSoftObjectPtr<UMaterialInterface> PreviewBackdropMaterial;

    TUniquePtr<FPreviewScene> Scene;

    UPROPERTY(Transient)
    TObjectPtr<AActor> PreviewActor;

    UPROPERTY(Transient)
    TObjectPtr<ASceneCapture2D> CaptureActor;

    UPROPERTY(Transient)
    TObjectPtr<UTextureRenderTarget2D> PreviewTexture;

    UPROPERTY(Transient)
    TObjectPtr<UBBBCharacterCustomizationView> View;

    UPROPERTY(Transient)
    FBBBAppearanceSelection Draft;

    TWeakObjectPtr<APlayerController> Controller;

    TWeakObjectPtr<UBBBAppearanceComponent> Target;

    TWeakObjectPtr<UBBBAppearanceComponent> Preview;

    FTSTicker::FDelegateHandle TickHandle;

    FName CurrentView = TEXT("Full");

    float TimeSinceUpdate = 0.0f;

    bool bInputCaptured = false;

    bool bPreviousCursor = false;

    bool bPreviousGameplayInput = true;
};
