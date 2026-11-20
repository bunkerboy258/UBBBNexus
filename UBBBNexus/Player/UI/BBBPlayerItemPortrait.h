#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBPlayerItemPortrait.generated.h"

class FPreviewScene;
class APawn;
class AActor;
class ASceneCapture2D;
class UTextureRenderTarget2D;
class USkeletalMeshComponent;
class UWidgetComponent;
class USceneComponent;
class SWidget;
struct FStreamableHandle;

/** 在本地独立预览世界捕获固定正面显示 只复制外观结果 */
UCLASS()
class ABBB_EVAC_API UBBBPlayerItemPortrait final : public UObject
{
    GENERATED_BODY()
public:
    /** @param Pawn 当前世界的玩家角色 @return 显示资源是否准备完成 */
    bool Prepare(APawn *Pawn);
    /** @param bInActive 是否需要显示预览 @return 无 关闭时停用灯光与捕获 */
    void SetActive(bool bInActive);
    /** @param DeltaTime 帧时长 @return 无 同步穿戴与材质 */
    void Update(float DeltaTime);
    /** @return 无 销毁所属世界内的临时显示演员 */
    void Close();
    /** @return 整备间最终画面 */
    UTextureRenderTarget2D *GetTexture() const { return Texture; }
    /** @return 整备间用于视口投影的相机 */
    const ASceneCapture2D *GetCamera() const { return Capture; }
    /** @return 可交互的空间面板 */
    const TArray<TObjectPtr<UWidgetComponent>>& GetPanels() const { return Panels; }
    /** @param Content 三块面板的控件树 @return 无 */
    void SetPanels(const TArray<TSharedPtr<SWidget>>& Content);
    virtual void BeginDestroy() override;

private:
    TSharedPtr<FPreviewScene> PreviewScene;
    TWeakObjectPtr<APawn> Source;
    UPROPERTY(Transient)
    TObjectPtr<AActor> Display;
    UPROPERTY(Transient)
    TObjectPtr<ASceneCapture2D> Capture;
    UPROPERTY(Transient)
    TObjectPtr<UTextureRenderTarget2D> Texture;
    UPROPERTY(Transient)
    TArray<TObjectPtr<UWidgetComponent>> Panels;
    UPROPERTY(Transient)
    TArray<TObjectPtr<UWidgetComponent>> RoomSigns;
    void BuildRoom(USceneComponent& Root);
    UPROPERTY(Transient)
    TArray<TObjectPtr<USkeletalMeshComponent>> Components;
    UPROPERTY(Transient)
    TObjectPtr<USkeletalMeshComponent> Pose;
    FVector Origin = FVector::ZeroVector;
    float Elapsed = 0.0f;
    TSharedPtr<FStreamableHandle> ResourceLoad;
    uint64 ObservedAppearanceRevision = 0;
    bool bActive = false;
    bool bDirty = true;
};
