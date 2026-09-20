#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Core/Runtime/BBBEquipmentRuntimeContext.h"

/** 由具体武器签名静态定义的运行时容器 */
template<typename TSignature>
class TBBBEquipmentRuntime final
{
public:
    using FState = typename TSignature::FState;
    using FInputFrame = typename TSignature::FInputFrame;

    template<typename TPacket>
    void SubmitInput(const TPacket &Packet)
    {
        InputFrame.template Submit<TPacket>(Packet);
    }

    template<typename TDefinition>
    void Initialize(const TDefinition &Definition)
    {
        TSignature::Initialize(State, Definition);
    }

    void Update(FBBBEquipmentRuntimeContext &Context)
    {
        TSignature::Parse(State, InputFrame, Context);
        InputFrame.Reset();
    }

    /** @return 武器跨帧状态 */
    const FState &GetState() const
    {
        return State;
    }

private:
    FState State;

    FInputFrame InputFrame;
};
