#pragma once

#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "UObject/Object.h"
#include "BBBWork/UBBBNexus/Customization/Appearance/Data/BBBAppearanceItem.h"
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
struct FStreamableHandle;

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

    /** @return 当前草稿是否仍在准备预览 */
    bool IsPreparing() const;

    /**
     * 关闭界面并丢弃草稿
     * @return 无
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|自定义")
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
    UFUNCTION(BlueprintCallable, Category = "BBB|自定义")
    bool CycleItem(FName Slot, int32 Direction);

    /**
     * 直接选择指定部位款式
     * @param Slot	目标部位
     * @param ItemId	目录中的款式行名
     * @return 是否成功预览
     */
    bool SelectItem(FName Slot, FName ItemId);

    /**
     * 直接选择身体或背心的徽章图案
     * @param Slot	目标部位
     * @param PatchIndex	从零开始的图案索引
     * @return 是否成功预览
     */
    bool SelectPatch(FName Slot, int32 PatchIndex);

    /**
     * 读取指定部位的可选款式
     * @param Slot	目标部位
     * @return 有效的目录行名
     */
    TArray<FName> GetItems(FName Slot) const;

    /**
     * 读取指定款式配置
     * @param ItemId	目录中的款式行名
     * @param Item	找到的款式配置
     * @return 条目是否有效
     */
    bool GetItem(FName ItemId, FBBBAppearanceItem &Item) const;

    /**
     * 切换身体或背心的徽章图案
     * @param Slot	部位名称
     * @param Direction	正数选择下一项 负数选择上一项
     * @return 是否预览成功
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|自定义")
    bool CyclePatch(FName Slot, int32 Direction);

    /**
     * 设置整体表面参数
     * @param Dirt	污渍强度
     * @param Weathering	磨损强度
     * @return 是否预览成功
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|自定义")
    bool SetSurface(float Dirt, float Weathering);

    /** @return 当前草稿 */
    UFUNCTION(BlueprintPure, Category = "BBB|自定义")
    FBBBAppearanceSelection GetDraft() const;

    /**
     * 应用并保存最终组合
     * @return 是否应用成功
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|自定义")
    bool Apply();

    /**
     * 选择预览机位
     * @param ViewName	Full Head 或 Legs
     * @return 无
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|自定义")
    void SelectView(FName ViewName);

    /**
     * 平滑查看指定穿戴部位
     * @param Slot	穿戴部位
     * @return 无
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|自定义")
    void FocusSlot(FName Slot);

    /**
     * 旋转预览人物
     * @param Degrees	水平旋转角度
     * @return 无
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|自定义")
    void RotatePreview(float Degrees);

    virtual void BeginDestroy() override;

private:
    bool CreatePreview();

    /** 创建仅由预览世界持有的灯光与背景 */
    bool ConfigurePreviewLighting();

    bool SetDraft(FBBBAppearanceSelection Selection);

    bool TickPreview(float DeltaTime);

    void UpdateCamera();

    /** 在预览更新时合并草稿变化并等待所需模型加载完成 */
    void UpdateDraft();

    void Capture();

    void CaptureInput(APlayerController &Player);

    void RestoreInput();

    /** 模块唯一的预览人物配置 */
    UPROPERTY(Config)
    TSoftClassPtr<AActor> PreviewActorClass;

    /** 独立预览世界的深蓝紫背景材质 */
    UPROPERTY(Config)
    TSoftObjectPtr<UMaterialInterface> PreviewBackdropMaterial;

    /** 预览人物脚下接收投影的暗色地面材质 */
    UPROPERTY(Config)
    TSoftObjectPtr<UMaterialInterface> PreviewFloorMaterial;

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

    /** 最后成功显示的草稿 用于资源失败时恢复界面 */
    UPROPERTY(Transient)
    FBBBAppearanceSelection DisplayedDraft;

    TWeakObjectPtr<APlayerController> Controller;

    TWeakObjectPtr<UBBBAppearanceComponent> Target;

    TWeakObjectPtr<UBBBAppearanceComponent> Preview;

    FTSTicker::FDelegateHandle TickHandle;

    FName CurrentView = TEXT("Full");

    float TimeSinceUpdate = 0.0f;

    /** 当前草稿的资源请求 只保留最后一次选择 */
    TSharedPtr<FStreamableHandle> DraftLoad;

    bool bDraftDirty = false;

    /** 运镜从当前画面接续而不是从旧机位重新开始 */
    FVector CameraStart = FVector::ZeroVector;

    FVector CameraTarget = FVector::ZeroVector;

    float RotationStart = 90.0f;

    float RotationTarget = 90.0f;

    float CameraElapsed = 0.45f;

    bool bInputCaptured = false;

    bool bPreviousCursor = false;

    bool bPreviousGameplayInput = true;
};
