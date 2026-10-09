#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/Definitions/BBBCharacterAppearanceDisplayAttachment.h"
class USkeletalMesh;
class UMaterialInterface;
#include "BBBCharacterAppearanceDisplayPart.generated.h"

/** 角色外观的蓝图机械显示部件 */
USTRUCT(BlueprintType)
struct FBBBCharacterAppearanceDisplayPart
{
    GENERATED_BODY()

    /** 绑定到人物蓝图组件的部件名 */
    UPROPERTY(BlueprintReadOnly)
    FName Slot;
    /** 已加载模型 空模型必须清空旧部件 */
    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<USkeletalMesh> Mesh = nullptr;
    /** 刚性部件的跟随骨骼 空名称表示使用角色蒙皮姿势 */
    UPROPERTY(BlueprintReadOnly)
    FName AttachBone;
    /** 已按模型参考姿势计算的挂接偏移 */
    UPROPERTY(BlueprintReadOnly)
    FTransform RelativeTransform = FTransform::Identity;
    /** 已生成材质 顺序与模型一致 */
    UPROPERTY(BlueprintReadOnly)
    TArray<TObjectPtr<UMaterialInterface>> Materials;
    /** 已加载附件及其材质 蓝图只负责机械应用 */
    UPROPERTY(BlueprintReadOnly)
    TMap<FName, FBBBCharacterAppearanceDisplayAttachment> Attachments;
    /** 此显示部件是否使用基础回退 */
    UPROPERTY(BlueprintReadOnly)
    bool bFallback = false;

};
