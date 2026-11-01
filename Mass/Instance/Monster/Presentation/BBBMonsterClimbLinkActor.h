#pragma once

#include "Navigation/NavLinkProxy.h"
#include "BBBMonsterClimbLinkActor.generated.h"

class UBBBMonsterDefinition;

/** 静态台沿的单向攀爬入口 不逐帧更新 不承载僵尸状态 */
UCLASS(meta = (DisplayName = "僵尸攀爬连接"))
class ABBB_EVAC_API ABBBMonsterClimbLinkActor final : public ANavLinkProxy
{
    GENERATED_BODY()

public:
    ABBBMonsterClimbLinkActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    UPROPERTY(EditAnywhere, Category = "攀爬连接", meta = (DisplayName = "入口脚底", MakeEditWidget = "true"))
    FVector Entry = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, Category = "攀爬连接", meta = (DisplayName = "出口脚底", MakeEditWidget = "true"))
    FVector Exit = FVector(150, 0, 100);

    UPROPERTY(EditAnywhere, Category = "攀爬连接", meta = (DisplayName = "台沿抓握点", MakeEditWidget = "true"))
    FVector Ledge = FVector(70, 0, 100);

    /** 自动制作入口的归属标签 手工连接不受批量重建影响 */
    UPROPERTY()
    FName BuildGroup;

    /** @return 当前世界空间入口 */
    FVector GetEntry() const { return GetActorTransform().TransformPosition(Entry); }

    /** @return 当前世界空间出口 */
    FVector GetExit() const { return GetActorTransform().TransformPosition(Exit); }

    /** @return 当前世界空间抓握点 */
    FVector GetLedge() const { return GetActorTransform().TransformPosition(Ledge); }

    virtual void OnConstruction(const FTransform& Transform) override;
};
