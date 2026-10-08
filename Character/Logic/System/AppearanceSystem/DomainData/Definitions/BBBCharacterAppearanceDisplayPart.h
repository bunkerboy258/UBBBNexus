#pragma once
#include "CoreMinimal.h"
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
    /** 已生成材质 顺序与模型一致 */
    UPROPERTY(BlueprintReadOnly)
    TArray<TObjectPtr<UMaterialInterface>> Materials;
    /** 已加载附件模型 蓝图只负责挂接 */
    UPROPERTY(BlueprintReadOnly)
    TMap<FName, TObjectPtr<USkeletalMesh>> Attachments;
    /** 此显示部件是否使用基础回退 */
    UPROPERTY(BlueprintReadOnly)
    bool bFallback = false;

};
