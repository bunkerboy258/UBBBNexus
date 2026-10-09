#include "BBBWork/UBBBNexus/Debug/Item/BBBItemDebugActor.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Config/Item/BBBItemDefinition.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Item/FBBBItemAddLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Catalog/BBBItemCatalog.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogBBBItemDebug, Log, All);

ABBBItemDebugActor::ABBBItemDebugActor()
{
    SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickGroup = TG_PostUpdateWork;
    bReplicates = false;
}

void ABBBItemDebugActor::BeginPlay()
{
    Super::BeginPlay();
    if (!GetWorld() || !GetWorld()->IsGameWorld())
    {
        SetActorTickEnabled(false);
        return;
    }
    if (ItemDefinitions.IsEmpty() || PlayerIndex < 0 || !FMath::IsFinite(WaitTimeout) || WaitTimeout <= 0.0f)
    {
        UE_LOG(LogBBBItemDebug, Error, TEXT("%s 物品调试配置无效 请检查物品列表 玩家索引和等待时限"), *GetPathName());
        SetActorTickEnabled(false);
        return;
    }
    for (const UBBBItemDefinition *Definition : ItemDefinitions)
    {
        if (!IsValid(Definition) || Definition->ItemId.IsNone())
        {
            UE_LOG(LogBBBItemDebug, Error, TEXT("%s 物品列表包含无效定义或物品 ID 不提交入包输入"), *GetPathName());
            SetActorTickEnabled(false);
            return;
        }
    }
}

void ABBBItemDebugActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (GetGameTimeSinceCreation() >= WaitTimeout)
    {
        UE_LOG(LogBBBItemDebug, Error, TEXT("%s 等待物品注入目标就绪超时 %.2f 秒"), *GetPathName(), WaitTimeout);
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
        UE_LOG(LogBBBItemDebug, Warning, TEXT("%s 拒绝向镜像角色 %s 注入物品"), *GetPathName(), *Character->GetPathName());
        SetActorTickEnabled(false);
        return;
    }
    if (Character->RuntimeData.Item.ReadItemInventoryState().Slots.IsEmpty()
        || !Character->GetMesh()
        || !Cast<UBBBAnimInstance>(Character->GetMesh()->GetAnimInstance()))
    {
        return;
    }
    const UBBBItemCatalog *Catalog = Character->GetCharacterConfig().Item.Catalog;
    if (!IsValid(Catalog) || ItemDefinitions.IsEmpty())
    {
        UE_LOG(LogBBBItemDebug, Error, TEXT("%s 物品目录或待注入物品列表无效 不提交入包输入"), *GetPathName());
        SetActorTickEnabled(false);
        return;
    }
    FBBBItemAddLocalControlPacket Packet;
    Packet.ItemIds.Reserve(ItemDefinitions.Num());
    for (const UBBBItemDefinition *Definition : ItemDefinitions)
    {
        const FBBBItemCatalogEntry *Entry = IsValid(Definition) ? Catalog->FindItem(Definition->ItemId) : nullptr;
        if (!Entry || Entry->Definition != Definition)
        {
            UE_LOG(LogBBBItemDebug, Error, TEXT("%s 所选物品未登记在角色目录或定义不一致 不提交入包输入"), *GetPathName());
            SetActorTickEnabled(false);
            return;
        }
        Packet.ItemIds.Add(Definition->ItemId);
    }
    const FString ItemNames = FString::JoinBy(Packet.ItemIds, TEXT(" "), [](const FName Id)
    {
        return Id.ToString();
    });
    const bool bSubmitted = Character->SubmitInput(MoveTemp(Packet));
    SetActorTickEnabled(false);
    if (!bSubmitted)
    {
        UE_LOG(LogBBBItemDebug, Error, TEXT("%s 向角色 %s 提交物品 %s 失败"), *GetPathName(), *Character->GetPathName(), *ItemNames);
        return;
    }
    UE_LOG(LogBBBItemDebug, Display, TEXT("%s 已向角色 %s 提交物品 %s 的入包输入"), *GetPathName(), *Character->GetPathName(), *ItemNames);
}
