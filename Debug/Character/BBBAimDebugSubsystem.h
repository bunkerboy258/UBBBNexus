#pragma once
#include "Subsystems/WorldSubsystem.h"
#include "BBBAimDebugSubsystem.generated.h"
/** 世界级本地瞄准观察 不修改业务状态 */
UCLASS()
class UBBBAimDebugSubsystem final : public UWorldSubsystem
{
    GENERATED_BODY()
public:
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
private:
    void Draw(UWorld* World, ELevelTick Tick, float Delta);
    FDelegateHandle PostTick;
};
