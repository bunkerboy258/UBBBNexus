#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

/**
 * 相机表现贡献包 提交一次冲量与恢复速度
 */
struct FBBBCameraLocalControlPacket final
{

    FVector Impulse = FVector::ZeroVector;

    /** @return 包内容是否合法 */
    bool IsValid() const
    {
        return !Impulse.ContainsNaN();
    }

    /**
     * 检查本帧是否允许执行
     * @param Context	黑板上下文
     * @return 是否允许执行
     */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return true;
    }

    /**
     * 追加相机表现贡献
     * @param Context	黑板上下文
     */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        // 相机输入由提交方预先累计，角色黑板只保留最后一次完整结果
        Context.Camera = FBBBPlayerCameraInput{Impulse};
    }
};
