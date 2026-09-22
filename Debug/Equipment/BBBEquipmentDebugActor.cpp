#include "BBBWork/UBBBNexus/Debug/Equipment/BBBEquipmentDebugActor.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/AnimationInstance/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/State/BBBEquipmentStatePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Catalog/BBBEquipmentCatalog.h"
#include "BBBWork/UBBBNexus/Equipment/Template/Definition/BBBEquipmentDefinition.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogBBBEquipmentDebug, Log, All);

ABBBEquipmentDebugActor::ABBBEquipmentDebugActor()
{
    SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickGroup = TG_PostUpdateWork;
    bReplicates = false;
}

void ABBBEquipmentDebugActor::BeginPlay()
{
    Super::BeginPlay();

    if (!GetWorld() || !GetWorld()->IsGameWorld())
    {
        SetActorTickEnabled(false);
        return;
    }

    if (!IsValid(EquipmentDefinition) || EquipmentDefinition->EquipmentId.IsNone()
        || PlayerIndex < 0 || !FMath::IsFinite(WaitTimeout) || WaitTimeout <= 0.0f)
    {
        UE_LOG(LogBBBEquipmentDebug, Error, TEXT("%s 装备注入配置无效 请检查装备 ID 玩家索引和等待时限"), *GetPathName());
        SetActorTickEnabled(false);
    }
}

void ABBBEquipmentDebugActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (GetGameTimeSinceCreation() >= WaitTimeout)
    {
        UE_LOG(LogBBBEquipmentDebug, Error, TEXT("%s 等待装备注入目标就绪超时 %.2f 秒"), *GetPathName(), WaitTimeout);
        SetActorTickEnabled(false);
        return;
    }

    ABBBCharacter *Character = TargetCharacter;
    if (!Character)
    {
        APlayerController *Controller = UGameplayStatics::GetPlayerController(this, PlayerIndex);
        if (!Controller || !Controller->IsLocalController())
        {
            return;
        }

        Character = Cast<ABBBCharacter>(Controller->GetPawn());
    }

    if (!IsValid(Character) || !Character->HasActorBegunPlay()
        || Character->RuntimeData.External.ReadWorldState().FrameDeltaSeconds <= 0.0f)
    {
        return;
    }

    if (Character->RuntimeData.External.ReadNetworkIdentityState().bIsMirror)
    {
        UE_LOG(LogBBBEquipmentDebug, Warning, TEXT("%s 拒绝向镜像角色 %s 注入装备"), *GetPathName(), *Character->GetPathName());
        SetActorTickEnabled(false);
        return;
    }

    const UBBBEquipmentCatalog *Catalog = Character->GetCharacterConfig().Equipment.EquipmentCatalog;
    if (!IsValid(EquipmentDefinition) || !IsValid(Catalog)
        || Catalog->FindDefinition(EquipmentDefinition->EquipmentId) != EquipmentDefinition)
    {
        UE_LOG(LogBBBEquipmentDebug, Error, TEXT("%s 所选装备未登记在目标角色目录或 ID 对应配置不一致 不修改角色配置"), *GetPathName());
        SetActorTickEnabled(false);
        return;
    }

    if (Character->RuntimeData.Equipment.ReadEquipmentInventoryState().Slots.IsEmpty()
        || !Character->GetMesh()
        || !Cast<UBBBAnimInstance>(Character->GetMesh()->GetAnimInstance()))
    {
        return;
    }

    FBBBEquipmentStatePacket Packet;
    Packet.EquipmentId = EquipmentDefinition->EquipmentId;
    const bool bSubmitted = Character->SubmitInput(MoveTemp(Packet));
    SetActorTickEnabled(false);

    if (!bSubmitted)
    {
        UE_LOG(LogBBBEquipmentDebug, Error, TEXT("%s 向角色 %s 提交装备 %s 失败"), *GetPathName(), *Character->GetPathName(), *EquipmentDefinition->EquipmentId.ToString());
        return;
    }

    UE_LOG(LogBBBEquipmentDebug, Display, TEXT("%s 已向角色 %s 提交装备 %s 的一次性状态输入 由角色管线执行后续创建与附着"), *GetPathName(), *Character->GetPathName(), *EquipmentDefinition->EquipmentId.ToString());
}
