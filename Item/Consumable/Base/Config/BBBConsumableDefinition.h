#pragma once
#include "BBBWork/UBBBNexus/Item/BBBItemDefinition.h"
#include "BBBConsumableDefinition.generated.h"
class UStaticMesh;
class UBBBConsumable;
class UAnimMontage;
class UBBBAnimInstance;
class UBBBThrowableDefinition;

/** 消耗品共有的静态持有资源 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBConsumableDefinition : public UBBBItemDefinition
{
    GENERATED_BODY()
public:
  /** @param Animation 角色动画层默认对象 @return 当前动画层对应的使用动作 */
  virtual UAnimMontage *GetUseMontage(const UBBBAnimInstance *Animation) const
  {
      return nullptr;
  }
  /** @return 独立世界实体定义 */
  virtual UBBBThrowableDefinition *GetReleaseDefinition() const
  {
      return nullptr;
  }
  /** @return 相对视线的抬角 */
  virtual float GetReleasePitch() const
  {
      return 0.0f;
  }
  /** @return 本次恢复的最大生命比例 */
  virtual float GetRecoveryFraction() const
  {
      return 0.0f;
  }
    UBBBConsumableDefinition()
    {
        ItemType = EBBBItemType::Consumable;
    }

    /** 当次使用创建的轻量运行时类型 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|消耗品", meta = (DisplayName = "使用运行时类型"))
    TSubclassOf<UBBBConsumable> RuntimeClass;

    /** 是否属于两个投掷快捷位置 恢复物使用后三个位置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|消耗品", meta = (DisplayName = "投掷物"))
    bool bThrowable = true;

    /** 手持时显示的静态网格 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|消耗品", meta = (DisplayName = "手持网格"))
    TObjectPtr<UStaticMesh> Mesh = nullptr;

    /** 角色手部挂接骨骼或插槽 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|消耗品", meta = (DisplayName = "持有挂点"))
    FName HandSocketName = TEXT("hand_r");

    /** 相对角色手部挂点的变换 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|消耗品", meta = (DisplayName = "持有偏移"))
    FTransform SpawnOffset = FTransform::Identity;

};
