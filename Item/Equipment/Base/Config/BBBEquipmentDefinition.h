#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/BBBItemDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Config/BBBEquipmentType.h"
#include "BBBEquipmentDefinition.generated.h"

class UBBBEquipmentAnimInstance;
class UAnimInstance;
class USkeletalMesh;
class UTexture2D;
class UAnimMontage;
class UAnimSequence;

/** 所有装备实例共享的静态资源配置 */
UCLASS(BlueprintType, PrioritizeCategories = ("物品资料", "装配与握持", "角色动画", "武器动画", "开火与装填", "持枪表现", "镜头后坐力", "专属机制"))
class ABBB_EVAC_API UBBBEquipmentDefinition : public UBBBItemDefinition
{
    GENERATED_BODY()

public:
  /** 当前装备姿态的角色快速近战动作 */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "角色动画", meta = (DisplayPriority = "7", DisplayName = "快速近战动作"))
  TObjectPtr<UAnimMontage> QuickMeleeMontage = nullptr;
  /** 一次使用结束后覆盖上半身循环的收势动作 */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "角色动画", meta = (DisplayPriority = "8", DisplayName = "使用结束收势"))
  TObjectPtr<UAnimMontage> UseReturnMontage = nullptr;
  UBBBEquipmentDefinition()
  {
      ItemType = EBBBItemType::Equipment;
  }

    /** 装备所属类别 供角色动画读取统一分类 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "装配与握持", meta = (DisplayPriority = "1", DisplayName = "装备类型", ToolTip = "供角色动画选择通用持枪类别 具体武器玩法由对应 Actor 实现 不靠此枚举切换"))
    EBBBEquipmentType EquipmentType = EBBBEquipmentType::None;

    /** 角色装备后链接的动画层类型 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "角色动画", meta = (DisplayPriority = "1", DisplayName = "角色动画层类", ToolTip = "装备激活后链接到角色主动画实例的动画层类 不作为武器网格的动画实例"))
    TSubclassOf<UAnimInstance> CharacterAnimationLayerClass;

    /** 当前装备供角色手指层固定采样的双手握姿 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "角色动画", meta = (DisplayPriority = "2", DisplayName = "角色双手握姿", ToolTip = "使用角色骨架的非加法静态序列 左右手指分别混合 未配置时保留当前动作的手指姿势"))
    TObjectPtr<UAnimSequence> CharacterGripPose = nullptr;

    /** 开火意图持续时供角色手指层固定采样的绝对握姿 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "角色动画", meta = (DisplayPriority = "3", DisplayName = "角色开火握姿", ToolTip = "只覆盖手指 挂接 手腕和左手IK沿用未开火握姿的同一套标定"))
    TObjectPtr<UAnimSequence> CharacterFireGripPose = nullptr;

    /** 在父骨骼空间叠加到角色原始右腕动作的握持旋转 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "装配与握持", meta = (DisplayPriority = "4", DisplayName = "右腕握持旋转偏移", ToolTip = "相对右前臂的增量旋转 单位度 保留动作原右腕轨迹 挂接偏移须按相同参考姿势反补偿 零值不改变动作"))
    FRotator RightHandGripRotationOffset = FRotator::ZeroRotator;

    /** 装备骨骼网格使用的动画实例类型 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "武器动画", meta = (DisplayPriority = "1", DisplayName = "武器动画类", ToolTip = "本武器骨骼网格创建的动画实例类 必须继承此装备类型的动画基类 每件装备独立实例"))
    TSubclassOf<UBBBEquipmentAnimInstance> EquipmentAnimationClass;

    /** 装备使用的骨骼网格 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "装配与握持", meta = (DisplayPriority = "2", DisplayName = "武器骨骼网格", ToolTip = "本装备实际显示及读取枪口 握持和攻击插槽的骨骼网格"))
    TObjectPtr<USkeletalMesh> EquipmentMesh = nullptr;

    /** 装备挂接到角色后的相对变换 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "装配与握持", meta = (DisplayPriority = "3", DisplayName = "装备挂接偏移", ToolTip = "相对角色装备挂接插槽的局部变换 位移单位厘米 旋转单位度 缩放无单位 不是世界空间"))
    FTransform SpawnOffset = FTransform::Identity;

};
