#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterAnimationMontageState.generated.h"

class UAnimMontage;
class FBBBCharacterAnimationMontageProcessor;
struct FBBBFullBodyMontagePacket;
struct FBBBUpperBodyMontagePacket;
struct FBBBFullBodyAdditivePreAimMontagePacket;
struct FBBBUpperBodyAdditiveMontagePacket;
struct FBBBAdditiveHitReactMontagePacket;

/** 动画系统固定槽位待消费的蒙太奇请求 */
USTRUCT()
struct FBBBCharacterAnimationMontageState final
{
    GENERATED_BODY()

private:
    friend class FBBBCharacterAnimationMontageProcessor;
    friend struct FBBBFullBodyMontagePacket;
    friend struct FBBBUpperBodyMontagePacket;
    friend struct FBBBFullBodyAdditivePreAimMontagePacket;
    friend struct FBBBUpperBodyAdditiveMontagePacket;
    friend struct FBBBAdditiveHitReactMontagePacket;

    /** FullBody 槽位待消费的蒙太奇引用 */
    UPROPERTY(Transient)
    TObjectPtr<UAnimMontage> FullBodyMontageRequest = nullptr;

    /** FullBody 槽位是否存在待消费请求 */
    UPROPERTY(Transient)
    bool bFullBodyMontageRequestPending = false;

    /** UpperBody 槽位待消费的蒙太奇引用 */
    UPROPERTY(Transient)
    TObjectPtr<UAnimMontage> UpperBodyMontageRequest = nullptr;

    /** UpperBody 槽位是否存在待消费请求 */
    UPROPERTY(Transient)
    bool bUpperBodyMontageRequestPending = false;

    /** FullBodyAdditivePreAim 槽位待消费的蒙太奇引用 */
    UPROPERTY(Transient)
    TObjectPtr<UAnimMontage> FullBodyAdditivePreAimMontageRequest = nullptr;

    /** FullBodyAdditivePreAim 槽位是否存在待消费请求 */
    UPROPERTY(Transient)
    bool bFullBodyAdditivePreAimMontageRequestPending = false;

    /** UpperBodyAdditive 槽位待消费的蒙太奇引用 */
    UPROPERTY(Transient)
    TObjectPtr<UAnimMontage> UpperBodyAdditiveMontageRequest = nullptr;

    /** UpperBodyAdditive 槽位是否存在待消费请求 */
    UPROPERTY(Transient)
    bool bUpperBodyAdditiveMontageRequestPending = false;

    /** AdditiveHitReact 槽位待消费的蒙太奇引用 */
    UPROPERTY(Transient)
    TObjectPtr<UAnimMontage> AdditiveHitReactMontageRequest = nullptr;

    /** AdditiveHitReact 槽位是否存在待消费请求 */
    UPROPERTY(Transient)
    bool bAdditiveHitReactMontageRequestPending = false;
};
