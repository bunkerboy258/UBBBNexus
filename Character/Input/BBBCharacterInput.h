#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/BBBCharacterInputRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Input/States/BBBCharacterInputStates.h"

class FBBBCharacterInitializer;

/** 角色的输入入口 */
class ABBB_EVAC_API FBBBCharacterInput final
{
public:
    /**
     * 提交外部持续状态
     * @param States	持续状态数据
     * @return 是否接受输入
     */
    bool Submit(const FBBBCharacterInputStates &States);

    /**
     * 提交离散输入包
     * @param Packet	输入包
     * @return 是否接受输入
     */
    template<typename TPacket>
    bool Submit(TPacket Packet)
    {
        // 仅游戏线程可提交 包必须通过自检
        if (!ensureMsgf(IsInGameThread() && Data, TEXT("[UBBBC]Packet input unavailable")))
        {
            return false;
        }

        if (!ensureMsgf(Packet.IsValid(), TEXT("[UBBBC]Invalid input packet")))
        {
            return false;
        }

        Data->Pending.Add(FBBBCharacterPacket(TInPlaceType<TPacket>{}, MoveTemp(Packet)));
        return true;
    }

private:
    friend class FBBBCharacterInitializer;

    void Initialize(FBBBCharacterInputRuntimeData &InData);

    FBBBCharacterInputRuntimeData *Data = nullptr;
};
