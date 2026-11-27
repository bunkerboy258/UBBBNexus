#include "BBBWork/UBBBNexus/Debug/Item/BBBItemDebugActor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Item/BBBItemDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Catalog/BBBEquipmentCatalog.h"
#include "BBBWork/UBBBNexus/Item/Wearable/Catalog/BBBWearableCatalog.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Catalog/BBBConsumableCatalog.h"
#include "BBBWork/UBBBNexus/Item/Misc/Catalog/BBBMiscCatalog.h"
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
    TSet<FName> ItemIds;
    for (const UBBBItemDefinition *Definition : ItemDefinitions)
    {
        if (!IsValid(Definition) || Definition->ItemId.IsNone())
        {
            UE_LOG(LogBBBItemDebug, Error, TEXT("%s 物品列表包含无效定义或物品 ID 不提交入包输入"), *GetPathName());
            SetActorTickEnabled(false);
            return;
        }
        if (ItemIds.Contains(Definition->ItemId))
        {
            UE_LOG(LogBBBItemDebug, Error, TEXT("%s 物品列表重复配置 %s"), *GetPathName(), *Definition->ItemId.ToString());
            SetActorTickEnabled(false);
            return;
        }
        ItemIds.Add(Definition->ItemId);
    }
}

void ABBBItemDebugActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (NextBatchIndex == 0 && GetGameTimeSinceCreation() >= WaitTimeout)
    {
        UE_LOG(LogBBBItemDebug, Error, TEXT("%s 等待物品注入目标就绪超时 %.2f 秒"), *GetPathName(), WaitTimeout);
        SetActorTickEnabled(false);
        return;
    }
    ABBBCharacter *Character = TargetCharacter;
    if (!Character)
    {
        APlayerController *Controller = UGameplayStatics::GetPlayerController(this, PlayerIndex);
        Character = Controller && Controller->IsLocalController() ? Cast<ABBBCharacter>(Controller->GetPawn()) : nullptr;
    }
    if (NextBatchIndex > 0 && (!IsValid(Character) || BatchTarget.Get() != Character))
    {
        UE_LOG(LogBBBItemDebug, Warning, TEXT("%s 发放目标已变化 停止提交剩余 %d 批物品"),
            *GetPathName(), ItemBatches.Num() - NextBatchIndex);
        ItemBatches.Reset();
        SetActorTickEnabled(false);
        return;
    }
    if (!IsValid(Character) || !Character->HasActorBegunPlay()
        || Character->RuntimeData.External.ReadWorldState().FrameDeltaSeconds <= 0.0f)
    {
        return;
    }
    if (Character->RuntimeData.External.ReadNetworkIdentityState().bIsMirror)
    {
        UE_LOG(LogBBBItemDebug, Warning, TEXT("%s 拒绝向镜像角色 %s 注入物品"), *GetPathName(), *Character->GetPathName());
        ItemBatches.Reset();
        SetActorTickEnabled(false);
        return;
    }
    if (Character->RuntimeData.Item.ReadCharacterItemInventoryState().Slots.IsEmpty()
        || !Character->GetMesh()
        || !Cast<UBBBAnimInstance>(Character->GetMesh()->GetAnimInstance()))
    {
        return;
    }
    if (NextBatchIndex == 0)
    {
        if (!PrepareItemBatches(*Character))
        {
            ItemBatches.Reset();
            SetActorTickEnabled(false);
            return;
        }
        BatchTarget = Character;
    }
    Character->SubmitInput(MoveTemp(ItemBatches[NextBatchIndex]));
    ++NextBatchIndex;
    if (NextBatchIndex == ItemBatches.Num())
    {
        UE_LOG(LogBBBItemDebug, Display, TEXT("%s 已向角色 %s 提交全部 %d 批入包输入"),
            *GetPathName(), *Character->GetPathName(), NextBatchIndex);
        ItemBatches.Reset();
        SetActorTickEnabled(false);
    }
}

bool ABBBItemDebugActor::PrepareItemBatches(const ABBBCharacter &Character)
{
    const auto &Config = Character.GetCharacterConfig().Item;
    for (const UBBBItemDefinition *Definition : ItemDefinitions)
    {
        int32 Registered = 0;
        const UBBBItemDefinition *RegisteredDefinition = nullptr;
        if (const auto Entry = Config.EquipmentCatalog ? Config.EquipmentCatalog->FindItem(Definition->ItemId) : nullptr)
        {
            ++Registered;
            RegisteredDefinition = Entry.GetDefaultObject()->GetDefinition();
        }
        if (const auto *Entry = Config.WearableCatalog ? Config.WearableCatalog->FindItem(Definition->ItemId) : nullptr)
        {
            ++Registered;
            RegisteredDefinition = Entry->Definition;
        }
        if (const auto Entry = Config.ConsumableCatalog ? Config.ConsumableCatalog->FindItem(Definition->ItemId) : nullptr)
        {
            ++Registered;
            RegisteredDefinition = Entry;
        }
        if (const auto *Entry = Config.MiscCatalog ? Config.MiscCatalog->FindItem(Definition->ItemId) : nullptr)
        {
            ++Registered;
            RegisteredDefinition = Entry;
        }
        if (Registered == 0)
        {
            continue;
        }
        if (Registered != 1 || RegisteredDefinition != Definition)
        {
            UE_LOG(LogBBBItemDebug, Error, TEXT("物品必须唯一登记 %s"), *Definition->ItemId.ToString());
            return false;
        }
        if (ItemBatches.IsEmpty() || ItemBatches.Last().ItemIds.Num() == ItemsPerFrame)
        {
            ItemBatches.AddDefaulted();
        }
        auto &Packet = ItemBatches.Last();
        Packet.ItemIds.Add(Definition->ItemId);
        Packet.Quantities.Add(Definition->ItemType == EBBBItemType::Consumable ? Definition->MaxStack : 1);
    }
    if (ItemBatches.IsEmpty())
    {
        UE_LOG(LogBBBItemDebug, Warning, TEXT("%s 未配置适用于角色 %s 的调试物品"), *GetPathName(), *Character.GetPathName());
        return false;
    }
    for (const auto &Packet : ItemBatches)
    {
        if (!Packet.IsValid())
        {
            UE_LOG(LogBBBItemDebug, Error, TEXT("%s 物品发放清单有无效输入 请检查所配置物品的数量"), *GetPathName());
            return false;
        }
    }
    return true;
}
