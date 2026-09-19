#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/Processors/BBBCharacterInputProcessor.h"

#include "BBBWork/UBBBNexus/Character/Input/BBBCharacterPacketRegistry.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterPacketContext.h"
#include "BBBWork/UBBBNexus/Character/Runtime/State/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/Definition/BBBCharacterMontageRequest.h"

void FBBBCharacterInputProcessor::Update(
    FBBBCharacterRuntimeData &Data, UBBBEquipmentCatalog &Catalog, const bool bRestoreMode) const
{
    FBBBCharacterParseState &State = Data.Operation;
    State.BeginFrame(bRestoreMode, Data.Equipment.Equipment.GetActiveMainHandInstance());

    TArray<FBBBCharacterPacket> Pending = MoveTemp(Data.Input.Pending);

    FBBBCharacterMontagePacket::BeginFrame(Data.Animation, State);
    Data.CameraContributions.Reset();

    // 持续状态只在本地模式落黑板 还原模式的角色由还原包直写对应域
    if (!bRestoreMode)
    {
        Data.Input.States.Movement.Apply(State.Control);
        Data.Input.States.Aim.Apply(State.Control);
        State.Control.bFire = false;
        State.Control.bJump = false;
    }

    // 高优先级先判先行 同优先级保持到达顺序
    Pending.StableSort([](const FBBBCharacterPacket &Left, const FBBBCharacterPacket &Right)
    {
        return Visit([](const auto &Packet) { return Packet.Priority; }, Left)
            > Visit([](const auto &Packet) { return Packet.Priority; }, Right);
    });

    FBBBApprovedPackets Approved;
    FBBBCharacterPacketContext Context{
        State,
        Data.Equipment.Inventory,
        Data.Equipment.Equipment,
        Data.Equipment.Commands,
        Data.Equipment.Events,
        Data.Animation,
        Data.Aim,
        Data.Locomotion,
        Data.CameraContributions,
        Catalog,
        Approved};

    int32 Index = 0;
    while (Index < Pending.Num())
    {
        const int32 Priority = Visit([](const auto &Packet) { return Packet.Priority; }, Pending[Index]);

        // 请求带集中两阶段 先按序全部求值再统一提交 冲突由失败方查询已批准集合让步
        if (Priority >= BBBCharacterPacketPriority::RequestMin
            && Priority <= BBBCharacterPacketPriority::RequestMax)
        {
            int32 BandEnd = Index;
            while (BandEnd < Pending.Num())
            {
                const int32 BandPriority = Visit([](const auto &Packet) { return Packet.Priority; }, Pending[BandEnd]);
                if (BandPriority < BBBCharacterPacketPriority::RequestMin
                    || BandPriority > BBBCharacterPacketPriority::RequestMax)
                {
                    break;
                }
                ++BandEnd;
            }

            TArray<int32> Accepted;
            for (int32 Cursor = Index; Cursor < BandEnd; ++Cursor)
            {
                Visit([&](const auto &Packet)
                {
                    if (Packet.CanExecute(Context))
                    {
                        Approved.Add<std::decay_t<decltype(Packet)>>();
                        Accepted.Add(Cursor);
                    }
                }, Pending[Cursor]);
            }

            for (const int32 AcceptedIndex : Accepted)
            {
                Visit([&](auto &Packet) { Packet.Execute(Context); }, Pending[AcceptedIndex]);
            }

            Index = BandEnd;
            continue;
        }

        // 其余带惰性求值 轮到即判即行 保持帧内因果链
        const bool bAccepted = Visit([&Context](const auto &Packet) { return Packet.CanExecute(Context); }, Pending[Index]);
        if (bAccepted)
        {
            Visit([&Context](auto &Packet) { Packet.Execute(Context); }, Pending[Index]);
        }

        ++Index;
    }

    // 派生门控与控制发布只存在于本地模式
    if (!bRestoreMode)
    {
        FinalizeControl(Data);
    }
}

void FBBBCharacterInputProcessor::FinalizeControl(FBBBCharacterRuntimeData &Data)
{
    FBBBCharacterParseState &State = Data.Operation;
    State.Control.bFire = State.bFire;

    // 瞄准或开火时禁止冲刺
    State.Control.bSprint = State.Control.bSprint && !State.Control.bAim && !State.Control.bFire;
    Data.Control.Value = State.Control;
}
