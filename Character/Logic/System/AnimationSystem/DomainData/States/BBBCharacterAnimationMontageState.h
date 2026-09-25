#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterAnimationMontageState.generated.h"

class UAnimMontage;
class FBBBCharacterAnimationMontageProcessor;
struct FBBBFullBodyMontageLocalControlPacket;
struct FBBBUpperBodyMontageLocalControlPacket;
struct FBBBFullBodyAdditivePreAimMontageLocalControlPacket;
struct FBBBUpperBodyAdditiveMontageLocalControlPacket;
struct FBBBAdditiveHitReactMontageLocalControlPacket;
struct FBBBFullBodyMontageAuthorityFactPacket;
struct FBBBUpperBodyMontageAuthorityFactPacket;
struct FBBBFullBodyAdditivePreAimMontageAuthorityFactPacket;
struct FBBBUpperBodyAdditiveMontageAuthorityFactPacket;
struct FBBBAdditiveHitReactMontageAuthorityFactPacket;

/** 动画系统固定槽位待消费的蒙太奇请求 */
USTRUCT()
struct FBBBCharacterAnimationMontageState final
{
    GENERATED_BODY()

private:
    friend class FBBBCharacterAnimationMontageProcessor;
    friend struct FBBBFullBodyMontageLocalControlPacket;
    friend struct FBBBUpperBodyMontageLocalControlPacket;
    friend struct FBBBFullBodyAdditivePreAimMontageLocalControlPacket;
    friend struct FBBBUpperBodyAdditiveMontageLocalControlPacket;
    friend struct FBBBAdditiveHitReactMontageLocalControlPacket;
    friend struct FBBBFullBodyMontageAuthorityFactPacket;
    friend struct FBBBUpperBodyMontageAuthorityFactPacket;
    friend struct FBBBFullBodyAdditivePreAimMontageAuthorityFactPacket;
    friend struct FBBBUpperBodyAdditiveMontageAuthorityFactPacket;
    friend struct FBBBAdditiveHitReactMontageAuthorityFactPacket;

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
