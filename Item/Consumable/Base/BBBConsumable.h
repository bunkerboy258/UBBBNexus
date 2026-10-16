#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/LocalControl/Use/FBBBConsumablePrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/LocalControl/Use/FBBBConsumableFinishLocalControlPacket.h"
#include "BBBConsumable.generated.h"
class ABBBCharacter;
class UBBBConsumableDefinition;
/** 仅服务当次使用的消耗品运行时根 */
UCLASS(Abstract, BlueprintType)
class ABBB_EVAC_API UBBBConsumable : public UObject
{
    GENERATED_BODY()
public:
  /** @param InDefinition 静态定义 @return 是否完成本次生命周期装配 */
  bool InitializeUse(UBBBConsumableDefinition &InDefinition);
  /** @return 无 由角色主管线推进本次使用 */
  void UpdateUse();
  /** @return 无 结束本次生命周期 */
  void ShutdownUse();
  /** @return 当次使用的角色 */
  ABBBCharacter *GetCharacter() const;
  /** @return 共享静态定义 */
  UBBBConsumableDefinition *GetDefinition() const;
  /** @return 本次使用是否仍在进行 */
  virtual bool IsUsing() const PURE_VIRTUAL(UBBBConsumable::IsUsing, return false;);
  /** @param Packet 输入包 @return 无 仅贡献输入 */
  template <typename TPacket> void SubmitInput(TPacket &&Packet)
  {
      QueueInput(Forward<TPacket>(Packet));
  }
protected:
  virtual bool InitializeRuntimeData() PURE_VIRTUAL(UBBBConsumable::InitializeRuntimeData, return false;);
  virtual void UpdateRuntimeData() PURE_VIRTUAL(UBBBConsumable::UpdateRuntimeData, );
  virtual void ShutdownRuntimeData() PURE_VIRTUAL(UBBBConsumable::ShutdownRuntimeData, );
  virtual void QueueInput(FBBBConsumablePrimaryLocalControlPacket<> Packet) PURE_VIRTUAL(UBBBConsumable::QueueInput, );
  virtual void QueueInput(FBBBConsumableFinishLocalControlPacket<> Packet) PURE_VIRTUAL(UBBBConsumable::QueueInput, );

private:
  /** 当前使用引用的静态定义 */
  UPROPERTY(Transient)
  TObjectPtr<UBBBConsumableDefinition> Definition = nullptr;
};
