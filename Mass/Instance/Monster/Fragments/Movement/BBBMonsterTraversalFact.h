#pragma once

#include "CoreMinimal.h"
#include "Engine/NetSerialization.h"
#include "BBBMonsterTraversalFact.generated.h"

/** 当前上台结果 网络和表现只还原这一份动作事实 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterTraversalFact
{
    GENERATED_BODY()

    /** 零为正常地面移动 一为快速上台 二为较高攀爬 */
    UPROPERTY()
    uint8 Kind = 0;

    /** 当前动作身份 */
    UPROPERTY()
    uint32 Serial = 0;

    /** 所在机器的世界时间 网络接收时转换 */
    UPROPERTY()
    float StartedAt = 0.0f;

    UPROPERTY()
    float Duration = 0.0f;

    /** 脚底起点 */
    UPROPERTY()
    FVector Start = FVector::ZeroVector;

    /** 脚底终点 */
    UPROPERTY()
    FVector End = FVector::ZeroVector;

    /** 台沿中央抓握点 */
    UPROPERTY()
    FVector Ledge = FVector::ZeroVector;

    /** @return 当前事实是否有限且类型合法 */
    bool IsValid() const
    {
        return Kind <= 2 && FMath::IsFinite(StartedAt) && FMath::IsFinite(Duration)
            && !Start.ContainsNaN() && !End.ContainsNaN() && !Ledge.ContainsNaN()
            && (Kind == 0 || (Duration >= 0.3f && Duration <= 10.0f));
    }

    /**
     * @param Ar	网络归档
     * @param Map	网络对象映射
     * @param bSuccess	当前事实是否有效
     * @return 是否完成当前状态序列化
     */
    bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bSuccess)
    {
        Ar.SerializeBits(&Kind, 2);
        Ar << Serial;
        if (Kind == 0)
        {
            bSuccess = !Ar.IsError();
            return true;
        }
        Ar << StartedAt;
        Ar << Duration;
        FVector_NetQuantize10 A(Start);
        FVector_NetQuantize10 B(End);
        FVector_NetQuantize10 C(Ledge);
        bool bA = true;
        bool bB = true;
        bool bC = true;
        A.NetSerialize(Ar, Map, bA);
        B.NetSerialize(Ar, Map, bB);
        C.NetSerialize(Ar, Map, bC);
        if (Ar.IsLoading())
        {
            Start = A;
            End = B;
            Ledge = C;
        }
        bSuccess = bA && bB && bC && !Ar.IsError() && IsValid();
        return true;
    }
};

template<>
struct TStructOpsTypeTraits<FBBBMonsterTraversalFact> : TStructOpsTypeTraitsBase2<FBBBMonsterTraversalFact>
{
    enum { WithNetSerializer = true };
};
