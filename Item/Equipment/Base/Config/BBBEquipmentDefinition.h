#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/BBBItemDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Config/BBBEquipmentType.h"
#include "BBBEquipmentDefinition.generated.h"

class UBBBEquipmentAnimInstance;
class UAnimInstance;
class USkeletalMesh;
class UTexture2D;

/** 所有装备实例共享的静态资源配置 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBEquipmentDefinition : public UBBBItemDefinition
{
    GENERATED_BODY()

public:
    UBBBEquipmentDefinition()
    {
        ItemType = EBBBItemType::Equipment;
    }

    /** 装备所属类别 供角色动画读取统一分类 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|装备|分类", meta = (DisplayName = "装备类型", ToolTip = "供角色动画选择通用持枪类别 具体武器玩法由对应 Actor 实现 不靠此枚举切换"))
    EBBBEquipmentType EquipmentType = EBBBEquipmentType::None;

    /** 角色装备后链接的动画层类型 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|装备|动画|角色", meta = (DisplayName = "角色动画层类", ToolTip = "装备激活后链接到角色主动画实例的动画层类 不作为武器网格的动画实例"))
    TSubclassOf<UAnimInstance> CharacterAnimationLayerClass;

    /** 装备骨骼网格使用的动画实例类型 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|装备|动画|武器", meta = (DisplayName = "武器动画类", ToolTip = "本武器骨骼网格创建的动画实例类 必须继承此装备类型的动画基类 每件装备独立实例"))
    TSubclassOf<UBBBEquipmentAnimInstance> EquipmentAnimationClass;

    /** 装备使用的骨骼网格 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|装备|外观", meta = (DisplayName = "武器骨骼网格", ToolTip = "本装备实际显示及读取枪口 握持和攻击插槽的骨骼网格"))
    TObjectPtr<USkeletalMesh> EquipmentMesh = nullptr;

    /** 装备挂接到角色后的相对变换 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|装备|挂接", meta = (DisplayName = "装备挂接偏移", ToolTip = "相对角色装备挂接插槽的局部变换 位移单位厘米 旋转单位度 缩放无单位 不是世界空间"))
    FTransform SpawnOffset = FTransform::Identity;

};
