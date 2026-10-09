#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterAppearanceDisplayAttachment.generated.h"
class USkeletalMesh;
class UMaterialInterface;

/** 附件的完整机械显示结果 */
USTRUCT(BlueprintType)
struct FBBBCharacterAppearanceDisplayAttachment final
{
    GENERATED_BODY()
    /** 已加载附件模型 */
    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<USkeletalMesh> Mesh = nullptr;
    /** 按模型材质槽排列的独立实例 */
    UPROPERTY(BlueprintReadOnly)
    TArray<TObjectPtr<UMaterialInterface>> Materials;
};
