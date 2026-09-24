#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/BBBRifleUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/RuntimeData/BBBRifleRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Definition/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ParseSystem/BBBRifleParseSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ParseSystem/Processors/BBBRifleParseProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ActionSystem/BBBRifleActionSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ActionSystem/Processors/BBBRifleActionProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/AnimationSystem/BBBRifleAnimationSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/AnimationSystem/Processors/BBBRifleAnimationProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/NetworkSystem/BBBRifleNetworkSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/NetworkSystem/Processors/BBBRifleNetworkProcessor.h"

ABBBRifleEquipment::ABBBRifleEquipment()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;
    PrimaryActorTick.TickGroup = TG_PostUpdateWork;
    PrimaryActorTick.EndTickGroup = TG_PostUpdateWork;
    if (USkeletalMeshComponent *Mesh = GetEquipmentSkeletalMesh())
    {
        PrimaryActorTick.AddPrerequisite(Mesh, Mesh->PrimaryComponentTick);
    }
}

bool ABBBRifleEquipment::InitializeRuntimeData()
{
    const UBBBRifleDefinition *RifleDefinition = Cast<UBBBRifleDefinition>(GetDefinition());
    if (!ensureMsgf(RifleDefinition, TEXT("步枪必须配置 UBBBRifleDefinition")))
    {
        return false;
    }

    FBBBRifleActionProcessor::Initialize(RuntimeData, *RifleDefinition);
    return true;
}

void ABBBRifleEquipment::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (!IsEquipped())
    {
        return;
    }

    ABBBCharacter *Character = Cast<ABBBCharacter>(GetOwner());
    USkeletalMeshComponent *Mesh = GetEquipmentSkeletalMesh();
    const UBBBRifleDefinition *RifleDefinition = Cast<UBBBRifleDefinition>(GetDefinition());
    UWorld *World = GetWorld();
    if (!ensureMsgf(Character && Mesh && RifleDefinition && World, TEXT("步枪更新缺少必要依赖")))
    {
        FBBBRifleParseProcessor::Clear(RuntimeData);
        return;
    }

    // 镜像实例等待第一份完整状态 避免把历史开火误认为刚发生
    if (IsMirror() && !RuntimeData.Animation.ReadRifleAnimationState().bInitialized
        && !RuntimeData.Parse.ReadRifleInputState().NetworkState.bActive)
    {
        return;
    }

    FBBBRifleUpdateContext Context{*this, *Character, *Mesh, *RifleDefinition, RuntimeData, *World};
    FBBBRifleParseSystem::Update(Context);
    FBBBRifleActionSystem::Update(Context);
    FBBBRifleAnimationSystem::Update(Context);
    FBBBRifleNetworkSystem::Update(Context);
}

void ABBBRifleEquipment::SubmitEquipInput()
{
    if (IsEquipped())
    {
        FBBBRifleParseProcessor::Submit(RuntimeData, FBBBRifleEquipPacket{});
    }
}

void ABBBRifleEquipment::SubmitPrimaryInput()
{
    if (IsEquipped() && !IsMirror())
    {
        FBBBRifleParseProcessor::Submit(RuntimeData, FBBBRifleFirePacket{});
    }
}

void ABBBRifleEquipment::SubmitReloadInput()
{
    if (IsEquipped() && !IsMirror())
    {
        FBBBRifleParseProcessor::Submit(RuntimeData, FBBBRifleReloadPacket{});
    }
}

void ABBBRifleEquipment::SubmitDetachMagazineInput()
{
    if (IsEquipped() && !IsMirror())
    {
        FBBBRifleParseProcessor::Submit(RuntimeData, FBBBRifleDetachMagazinePacket{});
    }
}

void ABBBRifleEquipment::SubmitLoadMagazineInput()
{
    if (IsEquipped() && !IsMirror())
    {
        FBBBRifleParseProcessor::Submit(RuntimeData, FBBBRifleLoadMagazinePacket{});
    }
}

void ABBBRifleEquipment::SubmitInterruptReloadInput()
{
    if (IsEquipped() && !IsMirror())
    {
        FBBBRifleParseProcessor::Submit(RuntimeData, FBBBRifleInterruptReloadPacket{});
    }
}

void ABBBRifleEquipment::OnUnequipped()
{
    ABBBCharacter *Character = Cast<ABBBCharacter>(GetOwner());
    USkeletalMeshComponent *Mesh = GetEquipmentSkeletalMesh();
    const UBBBRifleDefinition *RifleDefinition = Cast<UBBBRifleDefinition>(GetDefinition());
    UWorld *World = GetWorld();
    FBBBRifleActionProcessor::Stop(RuntimeData);
    if (Character && Mesh && RifleDefinition && World)
    {
        FBBBRifleUpdateContext Context{*this, *Character, *Mesh, *RifleDefinition, RuntimeData, *World};
        FBBBRifleAnimationProcessor::Stop(Context);
    }

    // 停止蒙太奇可能同步触发结束通知 所以输入最后统一清除
    FBBBRifleParseProcessor::Clear(RuntimeData);
}

bool ABBBRifleEquipment::SubmitNetworkPayload(const TArray<uint8> &Data)
{
    return IsEquipped() && IsMirror() && FBBBRifleNetworkProcessor::Submit(RuntimeData, Data);
}

void ABBBRifleEquipment::EmitShot_Implementation(const FTransform &MuzzleTransform)
{
    // 默认不产生弹丸 此处是本次保留的唯一发射扩展
}
