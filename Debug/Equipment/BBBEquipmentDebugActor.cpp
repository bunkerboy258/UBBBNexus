#include "BBBWork/UBBBNexus/Debug/Equipment/BBBEquipmentDebugActor.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Item/FBBBItemAddLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Item/Catalog/BBBItemCatalog.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Config/BBBEquipmentDefinition.h"
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

    if (EquipmentClasses.IsEmpty() || PlayerIndex < 0 || !FMath::IsFinite(WaitTimeout) || WaitTimeout <= 0.0f)
    {
        UE_LOG(LogBBBEquipmentDebug, Error, TEXT("%s 装备注入配置无效 请检查装备 ID 玩家索引和等待时限"), *GetPathName());
        SetActorTickEnabled(false);
        return;
    }

    for (const TSubclassOf<ABBBEquipment> EquipmentClass : EquipmentClasses)
    {
        const ABBBEquipment *ClassDefault = EquipmentClass ? EquipmentClass.GetDefaultObject() : nullptr;
        if (!ClassDefault || !IsValid(ClassDefault->GetDefinition()) || ClassDefault->GetEquipmentId().IsNone())
        {
            UE_LOG(LogBBBEquipmentDebug, Error, TEXT("%s 装备列表包含无效类或装备 ID 不提交入包输入"), *GetPathName());
            SetActorTickEnabled(false);
            return;
        }
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

    const UBBBItemCatalog *Catalog = Character->GetCharacterConfig().Item.Catalog;
    if (!IsValid(Catalog) || EquipmentClasses.IsEmpty())
    {
        UE_LOG(LogBBBEquipmentDebug, Error, TEXT("%s 装备目录或待注入装备列表无效 不提交入包输入"), *GetPathName());
        SetActorTickEnabled(false);
        return;
    }

    FBBBItemAddLocalControlPacket Packet;
    Packet.ItemIds.Reserve(EquipmentClasses.Num());
    for (const TSubclassOf<ABBBEquipment> EquipmentClass : EquipmentClasses)
    {
        const ABBBEquipment *ClassDefault = EquipmentClass ? EquipmentClass.GetDefaultObject() : nullptr;
        const FName EquipmentId = ClassDefault ? ClassDefault->GetEquipmentId() : NAME_None;
        if (!ClassDefault || !IsValid(ClassDefault->GetDefinition()) || EquipmentId.IsNone()
            || Catalog->FindEquipmentClass(EquipmentId) != EquipmentClass)
        {
            UE_LOG(LogBBBEquipmentDebug, Error, TEXT("%s 所选装备未登记在目标角色目录或 ID 对应配置不一致 不修改角色配置"), *GetPathName());
            SetActorTickEnabled(false);
            return;
        }

        Packet.ItemIds.Add(EquipmentId);
    }

    if (Character->RuntimeData.Item.ReadItemInventoryState().Slots.IsEmpty()
        || !Character->GetMesh()
        || !Cast<UBBBAnimInstance>(Character->GetMesh()->GetAnimInstance()))
    {
        return;
    }

    const FString EquipmentNames = FString::JoinBy(Packet.ItemIds, TEXT(" "), [](const FName Id)
    {
        return Id.ToString();
    });
    const bool bSubmitted = Character->SubmitInput(MoveTemp(Packet));
    SetActorTickEnabled(false);

    if (!bSubmitted)
    {
        UE_LOG(LogBBBEquipmentDebug, Error, TEXT("%s 向角色 %s 提交装备 %s 失败"), *GetPathName(), *Character->GetPathName(), *EquipmentNames);
        return;
    }

    UE_LOG(LogBBBEquipmentDebug, Display, TEXT("%s 已向角色 %s 提交物品 %s 的入包输入 手持装备保持不变"), *GetPathName(), *Character->GetPathName(), *EquipmentNames);
}
