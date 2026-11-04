#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBPlayerItemPortrait.generated.h"

class APawn;
class AActor;
class ASceneCapture2D;
class UTextureRenderTarget2D;
class USkeletalMeshComponent;
class UMaterialInstanceDynamic;
struct FStreamableHandle;

/** 在所属玩家世界内捕获固定正面显示 只复制外观结果 */
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
    /** @return 透明人物捕获材质 */
    UMaterialInstanceDynamic *GetMaterial() const;
    virtual void BeginDestroy() override;

private:
    TWeakObjectPtr<APawn> Source;
    UPROPERTY(Transient)
    TObjectPtr<AActor> Display;
    UPROPERTY(Transient)
    TObjectPtr<ASceneCapture2D> Capture;
    UPROPERTY(Transient)
    TObjectPtr<UTextureRenderTarget2D> Texture;
    UPROPERTY(Transient)
    TObjectPtr<UMaterialInstanceDynamic> Material;
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
