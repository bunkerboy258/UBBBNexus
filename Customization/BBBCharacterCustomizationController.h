#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBWork/UBBBNexus/Appearance/Data/BBBAppearanceSelection.h"
#include "BBBCharacterCustomizationController.generated.h"

class AActor;
class APlayerController;
class UBBBAppearanceComponent;
class UBBBCharacterCustomizationWidget;
class ULevelStreamingDynamic;

/** 管理一次本地换装的打开 预览 应用和关闭 */
UCLASS(Blueprintable)
class ABBB_EVAC_API UBBBCharacterCustomizationController : public UObject
{
    GENERATED_BODY()

public:
    /**
     * 打开展示场景
     * @param Player	本地玩家控制器
     * @return 是否开始打开
     */
    bool Open(APlayerController &Player);

    /**
     * 关闭界面并丢弃未应用草稿
     * @return 无
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|Customization")
    void Close();

    /** @return 是否正在打开或已经打开 */
    bool IsOpen() const;

    /**
     * 切换某个部位的候选条目
     * @param Slot	部位名称
     * @param Direction	正数下一项 负数上一项
     * @return 是否预览成功
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|Customization")
    bool CycleItem(FName Slot, int32 Direction);

    /**
     * 设置某个部位的染色
     * @param Slot	部位名称
     * @param Region	染色区域编号
     * @param Color	目标颜色
     * @return 是否预览成功
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|Customization")
    bool SetColor(FName Slot, int32 Region, FLinearColor Color);

    /**
     * 设置整体表面参数
     * @param Dirt	污渍强度
     * @param Weathering	磨损强度
     * @return 是否预览成功
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|Customization")
    bool SetSurface(float Dirt, float Weathering);

    /**
     * 设置完整草稿供蓝图扩展控件使用
     * @param Selection	待预览组合
     * @return 是否预览成功
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|Customization")
    bool SetDraft(FBBBAppearanceSelection Selection);

    /** @return 当前预览组合 */
    UFUNCTION(BlueprintPure, Category = "BBB|Customization")
    FBBBAppearanceSelection GetDraft() const;

    /**
     * 应用并保存当前草稿
     * @return 是否应用成功
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|Customization")
    bool Apply();

    /**
     * 选择展示机位
     * @param CameraTag	展示相机标签
     * @return 无
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|Customization")
    void SelectView(FName CameraTag);

    /**
     * 旋转展示人物
     * @param Degrees	水平旋转角度
     * @return 无
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|Customization")
    void RotatePreview(float Degrees);

protected:
    /** 只包含展示内容的本地子关卡 */
    UPROPERTY(EditDefaultsOnly, Category = "BBB|Customization")
    TSoftObjectPtr<UWorld> StageLevel;

    /** 本地展示位置 与实际玩法空间分开 */
    UPROPERTY(EditDefaultsOnly, Category = "BBB|Customization")
    FVector StageLocation = FVector(0.0, 0.0, -50000.0);

    /** 换装界面蓝图 */
    UPROPERTY(EditDefaultsOnly, Category = "BBB|Customization")
    TSubclassOf<UBBBCharacterCustomizationWidget> WidgetClass;

private:
    /** 场景可见后才建立预览与切换视角 */
    UFUNCTION()
    void OnStageShown();

    /** 当前本地控制器 */
    TWeakObjectPtr<APlayerController> Controller;

    /** 真实目标的外观入口 不依赖人物控制器类型 */
    TWeakObjectPtr<UBBBAppearanceComponent> Target;

    /** 本地展示对象的外观入口 */
    TWeakObjectPtr<UBBBAppearanceComponent> Preview;

    /** 打开前的视角目标 */
    TWeakObjectPtr<AActor> PreviousViewTarget;

    /** 当前加载的展示子关卡 */
    UPROPERTY(Transient)
    TObjectPtr<ULevelStreamingDynamic> Stage;

    /** 当前界面 */
    UPROPERTY(Transient)
    TObjectPtr<UBBBCharacterCustomizationWidget> Widget;

    /** 尚未应用的完整草稿 */
    UPROPERTY(Transient)
    FBBBAppearanceSelection Draft;

    /** 当前是否已接管玩家输入 */
    bool bInputCaptured = false;

    /** 打开前鼠标是否可见 */
    bool bPreviousCursor = false;

    /** 打开前玩法输入是否启用 */
    bool bPreviousGameplayInput = true;
};
