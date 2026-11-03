#pragma once
#include "GameFramework/Actor.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Config/BBBWorldDropConfig.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/RuntimeData/BBBWorldDropRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/ParseSystem/BBBWorldDropParseSystem.h"
#include "BBBWorldDropItem.generated.h"
class USphereComponent;
class UMaterialBillboardComponent;
class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class UBBBWorldDropNetworkComponent;
/** 共享世界掉落物的唯一运行时根 */
UCLASS(BlueprintType, Blueprintable)
class ABBB_EVAC_API ABBBWorldDropItem : public AActor
{
    GENERATED_BODY()
public:
    ABBBWorldDropItem();
    /** @param Packet\t待贡献的独立输入 @return 无 只进入固定输入槽 */
    template<typename TPacket>
    void SubmitInput(TPacket &&Packet)
    {
        FBBBWorldDropParseSystem::Submit(RuntimeData, Forward<TPacket>(Packet));
    }
    /** @return 公开的世界掉落物当前结果 */
    const FBBBWorldDropSnapshot &ReadDropSnapshot() const
    {
        return RuntimeData.Drop.ReadWorldDropState().Snapshot;
    }
    /** @return 当前是否已落地且可受理领取 */
    bool IsAvailable() const
    {
        const auto &Snapshot = ReadDropSnapshot();
        return Snapshot.DropId.IsValid() && Snapshot.Definition && Snapshot.Quantity > 0
            && Snapshot.Quantity <= 1000000 && Snapshot.bLanded && !Snapshot.bClaimed && !Snapshot.bConsumed;
    }
    /** @return 静态生成与表现配置 */
    const FBBBWorldDropConfig &GetDropConfig() const
    {
        return Config;
    }
    /** 唯一运行时聚合黑板 */
    UPROPERTY(Transient)
    FBBBWorldDropRuntimeData RuntimeData;
    //~ Begin AActor Interface
    virtual void BeginPlay() override;
    virtual void OnConstruction(const FTransform &Transform) override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    virtual bool ShouldTickIfViewportsOnly() const override;
    //~ End AActor Interface
protected:
    /** 关卡摆放与派生蓝图的生成配置 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "掉落物", meta = (DisplayName = "掉落物配置"))
    FBBBWorldDropConfig Config;
private:
    friend class FBBBWorldDropInitializer;
    friend class FBBBWorldDropProcessor;
    friend class FBBBWorldDropPresentationProcessor;
    friend class FBBBWorldDropNetworkProcessor;
    friend class FBBBWorldDropShutdown;
    /** 唯一世界物理主体 */
    UPROPERTY(VisibleAnywhere, Category = "掉落物", meta = (DisplayName = "碰撞球"))
    TObjectPtr<USphereComponent> Body;
    /** 自动朝向本地相机的材质图标 */
    UPROPERTY(VisibleAnywhere, Category = "显示", meta = (DisplayName = "白色图标"))
    TObjectPtr<UMaterialBillboardComponent> Icon;
    /** 无碰撞的光柱网格 */
    UPROPERTY(VisibleAnywhere, Category = "显示", meta = (DisplayName = "光柱"))
    TObjectPtr<UStaticMeshComponent> Beam;
    /** 无碰撞的地面柔光平面 */
    UPROPERTY(VisibleAnywhere, Category = "显示", meta = (DisplayName = "地面微光"))
    TObjectPtr<UStaticMeshComponent> GroundGlow;
    /** 复制当前业务结果的桥接组件 */
    UPROPERTY()
    TObjectPtr<UBBBWorldDropNetworkComponent> Network;
};
