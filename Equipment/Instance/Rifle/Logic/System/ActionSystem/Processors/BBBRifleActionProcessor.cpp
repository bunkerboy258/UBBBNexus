#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ActionSystem/Processors/BBBRifleActionProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/Update/BBBRifleUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/RuntimeData/BBBRifleRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Definition/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

namespace
{
    void Clear(FBBBRifleActionInputState &Input)
    {
        Input.bEquipRequested = false;
        Input.bFireRequested = false;
        Input.bReloadRequested = false;
        Input.bDetachMagazineRequested = false;
        Input.bLoadMagazineRequested = false;
        Input.bInterruptReloadRequested = false;
        Input.bHasNetworkState = false;
        Input.LoadedAmmo = 0;
        Input.FireSequence = 0;
        Input.ReloadSequence = 0;
        Input.bIsReloading = false;
        Input.bMagazineDetached = false;
    }
}

void FBBBRifleActionProcessor::Initialize(FBBBRifleRuntimeData &Data, const UBBBRifleDefinition &Definition)
{
    auto &State = Data.Action.ActionState;
    State.AmmoCapacity = FMath::Max(1, Definition.AmmoCapacity);
    State.LoadedAmmo = State.AmmoCapacity;
}

void FBBBRifleActionProcessor::Stop(FBBBRifleRuntimeData &Data)
{
    Data.Action.ActionState.bIsReloading = false;
    Data.Action.ActionState.bMagazineDetached = false;
}

void FBBBRifleActionProcessor::Update(FBBBRifleUpdateContext &Context)
{
    auto &Input = Context.RuntimeData.Action.ActionInputState;
    auto &State = Context.RuntimeData.Action.ActionState;
    State.bEquippedThisFrame = Input.bEquipRequested;

    if (Context.Equipment.IsMirror())
    {
        if (Input.bHasNetworkState)
        {
            if (State.FireSequence != Input.FireSequence
                && Context.RuntimeData.Animation.ReadRifleAnimationState().bInitialized)
            {
                State.LastFireTimeSeconds = Context.World.GetTimeSeconds();
            }

            State.LoadedAmmo = Input.LoadedAmmo;
            State.FireSequence = Input.FireSequence;
            State.ReloadSequence = Input.ReloadSequence;
            State.bIsReloading = Input.bIsReloading;
            State.bMagazineDetached = Input.bMagazineDetached;
        }

        Clear(Input);
        return;
    }

    // 弹匣通知先于本帧新动作 防止同一帧旧通知完成刚开始的换弹
    if (Input.bDetachMagazineRequested && State.bIsReloading)
    {
        State.bMagazineDetached = true;
    }

    if (Input.bLoadMagazineRequested && State.bIsReloading && State.bMagazineDetached)
    {
        State.LoadedAmmo = State.AmmoCapacity;
        State.bIsReloading = false;
        State.bMagazineDetached = false;
    }

    if (Input.bInterruptReloadRequested)
    {
        Stop(Context.RuntimeData);
    }

    if (Input.bReloadRequested && !State.bIsReloading && State.LoadedAmmo < State.AmmoCapacity)
    {
        if (ensureMsgf(Context.Definition.CharacterReloadMontage && Context.Definition.EquipmentReloadMontage,
            TEXT("步枪换弹缺少角色或装备蒙太奇")))
        {
            State.bIsReloading = true;
            State.bMagazineDetached = false;
            ++State.ReloadSequence;
        }
    }

    if (Input.bFireRequested && !State.bIsReloading && State.LoadedAmmo > 0
        && Context.World.GetTimeSeconds() - State.LastFireTimeSeconds >= Context.Definition.FireInterval)
    {
        if (ensureMsgf(Context.WeaponMesh.DoesSocketExist(Context.Definition.MuzzleSocketName),
            TEXT("步枪缺少枪口 Socket")))
        {
            --State.LoadedAmmo;
            ++State.FireSequence;
            State.LastFireTimeSeconds = Context.World.GetTimeSeconds();

            // 仅本机已成立的开火进入发射扩展 镜像分支在前面返回
            Context.Equipment.EmitShot(Context.WeaponMesh.GetSocketTransform(Context.Definition.MuzzleSocketName));
        }
    }

    Clear(Input);
}
