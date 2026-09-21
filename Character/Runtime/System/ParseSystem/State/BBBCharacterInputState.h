#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/State/BBBCharacterAimPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/State/BBBCharacterMovementPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/State/BBBAimStatePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/State/BBBEquipmentStatePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/State/BBBLocomotionStatePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBEquipFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBFireFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBMagazineDetachedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBMagazineLoadedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBReloadCancelledFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBReloadStartedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Presentation/BBBCameraPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Presentation/BBBMontagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Command/BBBEquipSlotPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Command/BBBFirePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Command/BBBJumpPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Command/BBBReloadPacket.h"

/**
 * 固定输入槽位
 *
 * 数据始终驻留在角色黑板中，提交操作只覆盖数据并激活槽位，不申请堆内存
 */
template<typename TPacket>
struct TBBBCharacterInputSlot final
{
    /** 本帧或上一处理阶段之后是否收到过该类型输入 */
    bool bActive = false;

    /** 最近一次提交的数据，同类型输入遵循最后一次覆盖规则 */
    TPacket Data;

    /**
     * 覆盖槽位数据并激活
     *
     * @param Packet    最新输入数据
     */
    void Submit(TPacket Packet)
    {
        Data = MoveTemp(Packet);
        bActive = true;
    }

    /** 标记当前数据已经被消费，保留对象存储供后续提交复用 */
    void Consume()
    {
        bActive = false;
    }
};

/**
 * 角色全部输入的固定聚合帧
 *
 * 字段顺序只描述驻留布局，真正应用顺序由FBBBCharacterInputProcessor::Update显式声明
 */
struct FBBBCharacterInputState final
{
    TBBBCharacterInputSlot<FBBBEquipmentStatePacket> EquipmentState;
    TBBBCharacterInputSlot<FBBBAimStatePacket> AimState;
    TBBBCharacterInputSlot<FBBBLocomotionStatePacket> LocomotionState;

    TBBBCharacterInputSlot<FBBBEquipFactPacket> EquipFact;
    TBBBCharacterInputSlot<FBBBFireFactPacket> FireFact;
    TBBBCharacterInputSlot<FBBBReloadStartedFactPacket> ReloadStartedFact;
    TBBBCharacterInputSlot<FBBBMagazineDetachedFactPacket> MagazineDetachedFact;
    TBBBCharacterInputSlot<FBBBMagazineLoadedFactPacket> MagazineLoadedFact;
    TBBBCharacterInputSlot<FBBBReloadCancelledFactPacket> ReloadCancelledFact;

    TBBBCharacterInputSlot<FBBBCharacterMovementPacket> Movement;
    TBBBCharacterInputSlot<FBBBCharacterAimPacket> Aim;

    TBBBCharacterInputSlot<FBBBEquipSlotPacket> EquipSlot;
    TBBBCharacterInputSlot<FBBBReloadPacket> Reload;
    TBBBCharacterInputSlot<FBBBFirePacket> Fire;
    TBBBCharacterInputSlot<FBBBJumpPacket> Jump;

    TBBBCharacterInputSlot<FBBBFullBodyMontagePacket> FullBodyMontage;
    TBBBCharacterInputSlot<FBBBUpperBodyMontagePacket> UpperBodyMontage;
    TBBBCharacterInputSlot<FBBBFullBodyAdditivePreAimMontagePacket> FullBodyAdditivePreAimMontage;
    TBBBCharacterInputSlot<FBBBUpperBodyAdditiveMontagePacket> UpperBodyAdditiveMontage;
    TBBBCharacterInputSlot<FBBBAdditiveHitReactMontagePacket> AdditiveHitReactMontage;

    TBBBCharacterInputSlot<FBBBCameraPacket> Camera;

    /** @return 输入处理器当前是否正在读取固定槽位 */
    bool IsProcessing() const
    {
        return bProcessing;
    }

    /** 开始只读解析阶段，提交闸口将在该阶段拒绝重入写入 */
    void BeginProcessing()
    {
        bProcessing = true;
    }

    /** 结束只读解析阶段，允许后续领域系统为下一次解析提交输入 */
    void EndProcessing()
    {
        bProcessing = false;
    }

    void Submit(FBBBEquipmentStatePacket Packet)
    {
        EquipmentState.Submit(MoveTemp(Packet));
    }

    void Submit(FBBBAimStatePacket Packet)
    {
        AimState.Submit(MoveTemp(Packet));
    }

    void Submit(FBBBLocomotionStatePacket Packet)
    {
        LocomotionState.Submit(MoveTemp(Packet));
    }

    void Submit(FBBBEquipFactPacket Packet)
    {
        EquipFact.Submit(MoveTemp(Packet));
    }

    void Submit(FBBBFireFactPacket Packet)
    {
        FireFact.Submit(MoveTemp(Packet));
    }

    void Submit(FBBBReloadStartedFactPacket Packet)
    {
        ReloadStartedFact.Submit(MoveTemp(Packet));
    }

    void Submit(FBBBMagazineDetachedFactPacket Packet)
    {
        MagazineDetachedFact.Submit(MoveTemp(Packet));
    }

    void Submit(FBBBMagazineLoadedFactPacket Packet)
    {
        MagazineLoadedFact.Submit(MoveTemp(Packet));
    }

    void Submit(FBBBReloadCancelledFactPacket Packet)
    {
        ReloadCancelledFact.Submit(MoveTemp(Packet));
    }

    void Submit(FBBBCharacterMovementPacket Packet)
    {
        Movement.Submit(MoveTemp(Packet));
    }

    void Submit(FBBBCharacterAimPacket Packet)
    {
        Aim.Submit(MoveTemp(Packet));
    }

    void Submit(FBBBEquipSlotPacket Packet)
    {
        EquipSlot.Submit(MoveTemp(Packet));
    }

    void Submit(FBBBReloadPacket Packet)
    {
        Reload.Submit(MoveTemp(Packet));
    }

    void Submit(FBBBFirePacket Packet)
    {
        Fire.Submit(MoveTemp(Packet));
    }

    void Submit(FBBBJumpPacket Packet)
    {
        Jump.Submit(MoveTemp(Packet));
    }

    void Submit(FBBBFullBodyMontagePacket Packet)
    {
        FullBodyMontage.Submit(MoveTemp(Packet));
    }

    void Submit(FBBBUpperBodyMontagePacket Packet)
    {
        UpperBodyMontage.Submit(MoveTemp(Packet));
    }

    void Submit(FBBBFullBodyAdditivePreAimMontagePacket Packet)
    {
        FullBodyAdditivePreAimMontage.Submit(MoveTemp(Packet));
    }

    void Submit(FBBBUpperBodyAdditiveMontagePacket Packet)
    {
        UpperBodyAdditiveMontage.Submit(MoveTemp(Packet));
    }

    void Submit(FBBBAdditiveHitReactMontagePacket Packet)
    {
        AdditiveHitReactMontage.Submit(MoveTemp(Packet));
    }

    void Submit(FBBBCameraPacket Packet)
    {
        Camera.Submit(MoveTemp(Packet));
    }

    /** 防止数据包应用期间重入覆盖正在读取的槽位数据 */
    bool bProcessing = false;
};
