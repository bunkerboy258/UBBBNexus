#pragma once

#include "CoreMinimal.h"

#include "Subsystems/WorldSubsystem.h"
#include "BBBEquipmentDisplaySubsystem.generated.h"

class AStaticMeshActor;
class UAnimSequenceBase;
class UStaticMesh;

/** 动画通知的世界级物理显示池 与玩法物品实例无关 */
UCLASS()
class ABBB_EVAC_API UBBBEquipmentDisplaySubsystem final : public UTickableWorldSubsystem
{
    GENERATED_BODY()
public:
    /** @param Definition 已加载的装备配置 @return 无 只读取动画引用并排队准备 */
    void Prepare(const UObject &Definition);
    /** @param Mesh 物体网格 @return 无 每帧至多创建一个池对象 */
    void PrepareMesh(UStaticMesh *Mesh);
    /** @param Mesh 网格 @param Transform 世界变换 @param Linear 线速度 @param Angular 弧度角速度 @param Lifetime 寿命 @param bStaticOnly 只碰撞静态世界 @return 是否取得显示对象 */
    bool Emit(UStaticMesh *Mesh, const FTransform &Transform, const FVector &Linear, const FVector &Angular,
        float Lifetime, bool bStaticOnly);
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override;
    virtual TStatId GetStatId() const override;
    virtual void Deinitialize() override;
protected:
    virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override;
private:
    /** @param Index 池索引 @return 无 清除上一轮速度与碰撞 */
    void Release(int32 Index);
    UPROPERTY(Transient)
    TArray<TObjectPtr<AStaticMeshActor>> Objects;
    /** 负值代表可复用对象 */
    TArray<double> ExpiresAt;
    UPROPERTY(Transient)
    TArray<TObjectPtr<UStaticMesh>> PendingMeshes;
    UPROPERTY(Transient)
    TArray<TObjectPtr<UAnimSequenceBase>> PendingAnimations;
    TSet<TWeakObjectPtr<UAnimSequenceBase>> PreparedAnimations;
    TSet<TWeakObjectPtr<UObject>> PreparedEffects;
};
