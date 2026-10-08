#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBPlayerItemPortrait.generated.h"

class APawn;
class AActor;
class ASceneCapture2D;
class UTextureRenderTarget2D;
class UPoseableMeshComponent;
class USkeletalMeshComponent;
class UMaterialInstanceDynamic;

/** 在所属玩家世界内捕获固定正面显示 只复制外观结果 */
UCLASS()
class ABBB_EVAC_API UBBBPlayerItemPortrait final : public UObject
{
    GENERATED_BODY()
public:
    /** @param Pawn 所属玩家角色 @return 本地捕获是否建立 */
    bool Open(APawn *Pawn);
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
    TArray<TObjectPtr<UPoseableMeshComponent>> Components;
    UPROPERTY(Transient)
    TObjectPtr<USkeletalMeshComponent> Pose;
    FVector Origin = FVector::ZeroVector;
    float Elapsed = 0.0f;
};
