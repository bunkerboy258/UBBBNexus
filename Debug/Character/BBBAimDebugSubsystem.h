#pragma once
#include "Subsystems/WorldSubsystem.h"
#include "BBBAimDebugShot.h"
#include "BBBAimDebugSubsystem.generated.h"
class USkeletalMeshComponent;
class ABBBEquipment;
class APawn;

/** 世界级本地瞄准观察 不修改业务状态 */
UCLASS()
class UBBBAimDebugSubsystem final : public UWorldSubsystem
{
    GENERATED_BODY()
public:
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    /** @param World 发射世界 @param Pawn 发射者 @param Entity 弹丸身份 @param Muzzle 冻结枪口 @param Direction 散布后方向 @param AttackId 同次开火身份 @return 无 */
    static void RecordLaunch(UWorld* World, APawn* Pawn, FMassEntityHandle Entity, const FTransform& Muzzle, const FVector& Direction, const FGuid& AttackId);
    /** @param World 碰撞世界 @param Entity 弹丸身份 @param Position 接触点 @param Normal 法线 @return 无 */
    static void RecordHit(UWorld* World, FMassEntityHandle Entity, const FVector& Position, const FVector& Normal);
    /** @return 清除订阅及射击标记 */
    void Clear();
private:
    void Draw(UWorld* World, ELevelTick Tick, float Delta);
    FDelegateHandle PostTick;
    TWeakObjectPtr<USkeletalMeshComponent> SelectedMesh;
    TWeakObjectPtr<ABBBEquipment> SelectedWeapon;
    TArray<FBBBAimDebugShot> Shots;
    uint64 Sequence = 0;
};
