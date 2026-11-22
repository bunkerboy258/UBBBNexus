#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBPlayerItemPortrait.generated.h"

class FPreviewScene;
class APawn;
class ABBBCharacter;
class ABBBEquipment;
class AActor;
class ABBBPlayerItemStage;
class ULevelStreamingDynamic;
class UTextureRenderTarget2D;
class USkeletalMeshComponent;
class UWidgetComponent;
class USceneComponent;
class USceneCaptureComponent2D;
class SWidget;

/** 在本地独立预览世界捕获背包画面 只复制外观结果 */
UCLASS()
class ABBB_EVAC_API UBBBPlayerItemPortrait final : public UObject
{
    GENERATED_BODY()
public:
    /** @param Pawn 当前世界的玩家角色 @return 显示资源是否准备完成 */
    bool Prepare(APawn *Pawn);
    /** @param bInActive 是否需要显示预览 @return 无 关闭时暂停捕获 */
    void SetActive(bool bInActive);
    /** @param Pawn 当前拥有角色 @return 预览资源是否仍属于此角色 */
    bool IsForPawn(const APawn* Pawn) const;
    /** @param DeltaTime 帧时长 @param Cursor 图像内的归一化光标位置 空值保持机位 @return 本帧是否实际切换了持枪状态 */
    bool Update(float DeltaTime, const TOptional<FVector2D>& Cursor);
    /** @return 无 销毁所属世界内的临时显示演员 */
    void Close();
    /** @return 整备间最终画面 */
    UTextureRenderTarget2D *GetTexture() const { return Texture; }
    /** @return 整备间用于视口投影的相机 */
    const USceneCaptureComponent2D *GetCamera() const;
    /** @return 预览关卡内的背包视觉配置 */
    const ABBBPlayerItemStage* GetStage() const { return Stage; }
    /** @return 可交互的空间面板 */
    const TArray<TObjectPtr<UWidgetComponent>>& GetPanels() const { return Panels; }
    /** @param Storage 存储面板 @param Loadout 装备面板 @param Navigation 页签面板 @return 无 */
    void SetPanels(const TSharedPtr<SWidget>& Storage, const TSharedPtr<SWidget>& Loadout,
        const TSharedPtr<SWidget>& Navigation);
    virtual void BeginDestroy() override;

private:
#if WITH_DEV_AUTOMATION_TESTS
    friend class FBBBItemRoomReview;
    float ReviewPoseTime = -1.0f;
    uint64 CaptureRevision = 0;
#endif
    TSharedPtr<FPreviewScene> PreviewScene;
    TWeakObjectPtr<APawn> Source;
    UPROPERTY(Transient)
    TObjectPtr<AActor> Display;
    UPROPERTY(Transient)
    TObjectPtr<ABBBPlayerItemStage> Stage;
    UPROPERTY(Transient)
    TObjectPtr<UTextureRenderTarget2D> Texture;
    UPROPERTY(Transient)
    TArray<TObjectPtr<UWidgetComponent>> Panels;
    bool PrepareStage();
    void ApplyCameraParallax(FVector2D Offset);
    void SyncAppearance(const ABBBCharacter* Character);
    bool SyncHeldWeapon(const ABBBCharacter& Character);
    void ApplyHeldGrip();
    UPROPERTY(Transient)
    TObjectPtr<ULevelStreamingDynamic> StageLevel;
    UPROPERTY(Transient)
    TArray<TObjectPtr<USkeletalMeshComponent>> Components;
    UPROPERTY(Transient)
    TObjectPtr<USkeletalMeshComponent> Pose;
    UPROPERTY(Transient)
    TObjectPtr<USkeletalMeshComponent> Weapon;
    TWeakObjectPtr<const ABBBEquipment> HeldEquipment;
    TMap<int32, FQuat> FingerGrip;
    FTransform HomeCamera = FTransform::Identity;
    FVector2D CameraOffset = FVector2D::ZeroVector;
    FVector2D CameraOffsetVelocity = FVector2D::ZeroVector;
    uint64 ObservedAppearanceRevision = 0;
    bool bActive = false;
    bool bDirty = true;
    bool bStageLoadFailed = false;
    bool bHoldingWeapon = false;
    bool bHoldInitialized = false;
};
