#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBPlayerItemPreview.generated.h"
class FPreviewScene;
class APawn;
class AActor;
class ASceneCapture2D;
class UTextureRenderTarget2D;
class UPoseableMeshComponent;
class USkeletalMeshComponent;
class UMaterialInstanceDynamic;

/** 独立显示世界仅复制正式角色外观 不创建角色或真实物品 */
UCLASS()
class ABBB_EVAC_API UBBBPlayerItemPreview final : public UObject
{
    GENERATED_BODY()
  public:
    /** @param Pawn 正式人物 @return 是否完成显示世界创建 */
    bool Open(APawn *Pawn);
    /** @param DeltaTime 帧时长 @return 无 同步实际部件并固定正面捕获 */
    void Update(float DeltaTime);
    /** @return 无 释放显示世界与捕获资源 */
    void Close();
    /** @return 人物捕获纹理 */
    UTextureRenderTarget2D *GetTexture() const;
    /** @return 带透明背景的人物显示材质 */
    UMaterialInstanceDynamic *GetMaterial() const;
    virtual void BeginDestroy() override;

  private:
    TUniquePtr<FPreviewScene> Scene;
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
    float Elapsed = 0.0f;
};
