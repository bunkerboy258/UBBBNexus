#include "BBBWork/UBBBNexus/Item/Base/Equipment/BBBEquipmentInstance.h"

#include "Animation/AnimMontage.h"
#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Item/Base/Equipment/BBBEquipmentActor.h"
#include "BBBWork/UBBBNexus/Item/Base/Equipment/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Item/Definition/BBBItemRuntimeData.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"

//创建运行时装备实例并初始化运行时数据
UBBBEquipmentInstance *UBBBEquipmentInstance::Create(
    UObject &Outer,
    UBBBEquipmentDefinition &InDefinition,
    int32 InStackCount)
{
    UBBBEquipmentInstance *Instance = NewObject<UBBBEquipmentInstance>(&Outer);
    if (!ensureMsgf(Instance, TEXT("[UBBBI]Equipment instance creation failed")))
    {return nullptr;}

    //生成实例唯一标识
    Instance->InstanceId = FGuid::NewGuid();

    Instance->Definition = &InDefinition;
    Instance->StackCount = FMath::Clamp(InStackCount, 1, InDefinition.MaxStack);
    Instance->RuntimeData = NewObject<UBBBItemRuntimeData>(Instance);

    if (!ensureMsgf(Instance->RuntimeData, TEXT("[UBBBI]Equipment runtime data creation failed")))
    {return nullptr;}

    Instance->RuntimeData->Initialize(InDefinition);
    Instance->bIsRuntimeDataInitialized = true;

    return Instance;
}

//创建镜像装备实例仅用于展示不含运行时数据
UBBBEquipmentInstance *UBBBEquipmentInstance::CreateMirror(
    UObject &Outer,
    UBBBEquipmentDefinition &InDefinition)
{
    UBBBEquipmentInstance *Instance = NewObject<UBBBEquipmentInstance>(&Outer);
    if (!ensureMsgf(Instance, TEXT("[UBBBI]Equipment mirror creation failed")))
    {
        return nullptr;
    }

    //生成实例唯一标识
    Instance->InstanceId = FGuid::NewGuid();

    Instance->Definition = &InDefinition;
    Instance->StackCount = 1;
    Instance->InstanceMode = EInstanceMode::Mirror;
    return Instance;
}

//装备到角色网格并生成模型 Actor
void UBBBEquipmentInstance::Equip(
    USkeletalMeshComponent &CharacterMesh,
    FBBBCharacterExternalAPI &CharacterAPI,
    FName AttachmentSocketName,
    float WorldTimeSeconds)
{
    UBBBEquipmentDefinition *EquipmentDefinition = GetEquipmentDefinition();
    if (!ensureMsgf(EquipmentDefinition, TEXT("[UBBBI]Equipment instance definition is invalid")))
    { return; }

    //运行时模式必须已完成运行时数据初始化
    if (InstanceMode == EInstanceMode::Runtime
        && !ensureMsgf(RuntimeData && bIsRuntimeDataInitialized, TEXT("[UBBBI]Equipment runtime data is not initialized")))
    {return;}

    //模型已存在则直接返回避免重复生成
    if (ModelActor)
    { return; }

    if (!ensureMsgf(
        EquipmentDefinition->EquipmentActorClass && CharacterMesh.GetOwner() && CharacterMesh.GetWorld(),
        TEXT("[UBBBI]Equipment model creation dependencies are invalid")))
    { return; }

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Owner = CharacterMesh.GetOwner();
    SpawnParameters.Instigator = Cast<APawn>(CharacterMesh.GetOwner());
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    //生成装备模型 Actor
    ModelActor = CharacterMesh.GetWorld()->SpawnActor<ABBBEquipmentActor>(
        EquipmentDefinition->EquipmentActorClass,
        FTransform::Identity,
        SpawnParameters);

    if (!ensureMsgf(ModelActor, TEXT("[UBBBI]Equipment model actor creation failed")))
    { return; }

    //挂接到角色网格指定插槽
    ModelActor->AttachToComponent(
        &CharacterMesh,
        FAttachmentTransformRules::SnapToTargetNotIncludingScale,
        AttachmentSocketName);

    //应用定义中的生成偏移
    ModelActor->SetActorRelativeTransform(EquipmentDefinition->SpawnOffset);

    //镜像模式禁用 Tick
    ModelActor->SetActorTickEnabled(InstanceMode == EInstanceMode::Runtime);

    ModelActor->Initialize(*this, CharacterAPI);
    ModelActor->Equip();

    //依据装备动作 Montage 计算装备结束时间
    bIsEquipping = EquipmentDefinition->EquipMontage != nullptr;
    EquipEndTime = bIsEquipping
        ? WorldTimeSeconds + EquipmentDefinition->EquipMontage->GetPlayLength()
        : 0.0f;
}

//释放模型 Actor 并复位装备状态
void UBBBEquipmentInstance::ReleaseModel()
{
    if (ModelActor)
    {
        ModelActor->Destroy();
        ModelActor = nullptr;
    }

    bIsEquipping = false;
    EquipEndTime = 0.0f;
}

//每帧更新装备过程状态
void UBBBEquipmentInstance::Update(float WorldTimeSeconds)
{
    //到达装备结束时间则结束装备中状态
    if (bIsEquipping && WorldTimeSeconds >= EquipEndTime)
    {
        bIsEquipping = false;
        EquipEndTime = 0.0f;
    }
}

//获取装备定义
UBBBEquipmentDefinition *UBBBEquipmentInstance::GetEquipmentDefinition() const
{
    return Cast<UBBBEquipmentDefinition>(Definition);
}

//获取模型 Actor
ABBBEquipmentActor *UBBBEquipmentInstance::GetModelActor() const
{
    return ModelActor;
}

//是否处于装备过程中
bool UBBBEquipmentInstance::IsEquipping() const
{
    return bIsEquipping;
}
