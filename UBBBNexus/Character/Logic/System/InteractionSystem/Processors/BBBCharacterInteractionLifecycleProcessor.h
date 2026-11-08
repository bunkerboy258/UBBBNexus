#pragma once
#include "CoreMinimal.h"
class AActor;
class ABBBCharacter;
class USkeletalMeshComponent;
struct FBBBCharacterInteractionUpdateContext;

/** 实际主手物品的创建 挂接与卸下 */
class FBBBCharacterInteractionLifecycleProcessor final
{
public:
    /** @param Context	当前交互上下文 @return 无 */
    void Update(FBBBCharacterInteractionUpdateContext &Context) const;
    /** @param Context	关闭上下文 @return 无 */
    void Shutdown(FBBBCharacterInteractionUpdateContext &Context);
private:
    static AActor *Create(ABBBCharacter &Character, TSubclassOf<AActor> ItemClass);
    static bool Attach(USkeletalMeshComponent &Mesh, FName Socket, AActor &Item);
    static void Detach(USkeletalMeshComponent &Mesh, AActor &Item);
};
