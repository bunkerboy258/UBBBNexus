#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ActionSystem/Processors/BBBRifleActionProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/BBBRifleUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/RuntimeData/BBBRifleRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Definition/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ParseSystem/Processors/BBBRifleParseProcessor.h"

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
    auto &Input = Context.RuntimeData.Parse.InputState;
    auto &State = Context.RuntimeData.Action.ActionState;
    State.bEquippedThisFrame = Input.Equip.bActive;

    if (Context.Equipment.IsMirror())
    {
        if (Input.NetworkState.bActive)
        {
            const auto &Packet = Input.NetworkState.Packet;
            if (State.FireSequence != Packet.FireSequence
                && Context.RuntimeData.Animation.ReadRifleAnimationState().bInitialized)
            {
                State.LastFireTimeSeconds = Context.World.GetTimeSeconds();
            }

            State.LoadedAmmo = Packet.LoadedAmmo;
            State.FireSequence = Packet.FireSequence;
            State.ReloadSequence = Packet.ReloadSequence;
            State.bIsReloading = Packet.bIsReloading;
            State.bMagazineDetached = Packet.bMagazineDetached;
        }

        FBBBRifleParseProcessor::Clear(Context.RuntimeData);
        return;
    }

    // 弹匣通知先于本帧新动作 防止同一帧旧通知完成刚开始的换弹
    if (Input.DetachMagazine.bActive && State.bIsReloading)
    {
        State.bMagazineDetached = true;
    }

    if (Input.LoadMagazine.bActive && State.bIsReloading && State.bMagazineDetached)
    {
        State.LoadedAmmo = State.AmmoCapacity;
        State.bIsReloading = false;
        State.bMagazineDetached = false;
    }

    if (Input.InterruptReload.bActive)
    {
        Stop(Context.RuntimeData);
    }

    if (Input.Reload.bActive && !State.bIsReloading && State.LoadedAmmo < State.AmmoCapacity)
    {
        if (ensureMsgf(Context.Definition.CharacterReloadMontage && Context.Definition.EquipmentReloadMontage,
            TEXT("步枪换弹缺少角色或装备蒙太奇")))
        {
            State.bIsReloading = true;
            State.bMagazineDetached = false;
            ++State.ReloadSequence;
        }
    }

    if (Input.Fire.bActive && !State.bIsReloading && State.LoadedAmmo > 0
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

    FBBBRifleParseProcessor::Clear(Context.RuntimeData);
}
