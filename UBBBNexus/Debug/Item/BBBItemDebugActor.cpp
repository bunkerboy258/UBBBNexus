#include "BBBWork/UBBBNexus/Debug/Item/BBBItemDebugActor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Item/FBBBCharacterItemMoveLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Item/BBBItemDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Catalog/BBBEquipmentCatalog.h"
#include "BBBWork/UBBBNexus/Item/Wearable/Catalog/BBBWearableCatalog.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Catalog/BBBConsumableCatalog.h"
#include "BBBWork/UBBBNexus/Item/Misc/Catalog/BBBMiscCatalog.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
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
    if (PlayerIndex < 0 || WearablePage < 0 || !FMath::IsFinite(WaitTimeout) || WaitTimeout <= 0.0f)
    {
        UE_LOG(LogBBBItemDebug, Error, TEXT("%s 物品调试配置无效 请检查玩家索引 配装页和等待时限"), *GetPathName());
        SetActorTickEnabled(false);
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
    if (!Character && ReviewCharacterClass)
    {
        double Nearest = TNumericLimits<double>::Max();
        for (TActorIterator<ABBBCharacter> It(GetWorld(), ReviewCharacterClass); It; ++It)
        {
            if (It->GetClass() == ReviewCharacterClass && It->IsLocallyControlled())
            {
                const double Distance = FVector::DistSquared(GetActorLocation(), It->GetActorLocation());
                if (Distance < Nearest)
                {
                    Character = *It;
                    Nearest = Distance;
                }
            }
        }
    }
    else if (!Character)
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
    if (NextBatchIndex == ItemBatches.Num())
    {
        ApplyReviewWearables(*Character);
        SetActorTickEnabled(false);
        return;
    }
    Character->SubmitInput(MoveTemp(ItemBatches[NextBatchIndex]));
    ++NextBatchIndex;
    if (NextBatchIndex == ItemBatches.Num())
    {
        UE_LOG(LogBBBItemDebug, Display, TEXT("%s 已向角色 %s 提交全部 %d 批入包输入"),
            *GetPathName(), *Character->GetPathName(), NextBatchIndex);
        if (ReviewWearables.IsEmpty())
        {
            ItemBatches.Reset();
            SetActorTickEnabled(false);
        }
    }
}

bool ABBBItemDebugActor::PrepareItemBatches(const ABBBCharacter &Character)
{
    const auto &Config = Character.GetCharacterConfig().Item;
    TArray<const UBBBItemDefinition *> Definitions;
    if (!ReviewWearables.IsEmpty())
    {
        for (const FName Id : ReviewWearables)
        {
            const auto *Entry = Config.WearableCatalog ? Config.WearableCatalog->FindItem(Id) : nullptr;
            Definitions.Add(Entry ? Entry->Definition.Get() : nullptr);
        }
    }
    else
    {
        if (const auto *Catalog = Config.EquipmentCatalog.Get())
        {
            for (const auto &Class : Catalog->Items)
            {
                const auto *Equipment = Class.GetDefaultObject();
                Definitions.Add(Equipment ? Equipment->GetDefinition() : nullptr);
            }
        }
        if (const auto *Catalog = Config.ConsumableCatalog.Get())
        {
            for (const UBBBConsumableDefinition *Definition : Catalog->Items)
            {
                Definitions.Add(Definition);
            }
        }
        if (const auto *Catalog = Config.MiscCatalog.Get())
        {
            for (const UBBBItemDefinition *Definition : Catalog->Items)
            {
                Definitions.Add(Definition);
            }
        }
        if (const auto *Catalog = Config.WearableCatalog.Get())
        {
            const int32 PageSize = Character.RuntimeData.Item.ReadCharacterItemInventoryState().WearableSlotCount;
            const int64 First = static_cast<int64>(WearablePage) * PageSize;
            const int32 Total = Catalog->Items.Num();
            if (Total && (PageSize <= 0 || First >= Total))
            {
                UE_LOG(LogBBBItemDebug, Error, TEXT("%s 配装页 %d 超出 %d 件配装与 %d 格背包的范围"),
                    *GetPathName(), WearablePage, Total, PageSize);
                return false;
            }
            const int64 Last = FMath::Min(First + PageSize, static_cast<int64>(Total));
            for (int32 Index = static_cast<int32>(First); Index < Last; ++Index)
            {
                Definitions.Add(Catalog->Items[Index].Definition);
            }
            if (PageSize > 0)
            {
                UE_LOG(LogBBBItemDebug, Display, TEXT("%s 配装第 %d/%d 页 本页 %lld 件 目录共 %d 件"),
                    *GetPathName(), WearablePage + 1, FMath::DivideAndRoundUp(Total, PageSize), Last - First, Total);
            }
        }
    }
    TSet<FName> ItemIds;
    for (const UBBBItemDefinition *Definition : Definitions)
    {
        if (!IsValid(Definition) || Definition->ItemId.IsNone() || ItemIds.Contains(Definition->ItemId))
        {
            UE_LOG(LogBBBItemDebug, Error, TEXT("%s 调试目录包含空物品或重复物品 %s"),
                *GetPathName(), *GetNameSafe(Definition));
            return false;
        }
        ItemIds.Add(Definition->ItemId);
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

void ABBBItemDebugActor::ApplyReviewWearables(ABBBCharacter &Character) const
{
    const auto &Inventory = Character.RuntimeData.Item.ReadCharacterItemInventoryState();
    FBBBCharacterItemMoveLocalControlPacket Packet;
    for (const FName Id : ReviewWearables)
    {
        const int32 Source = Inventory.Slots.IndexOfByPredicate([Id](const FBBBCharacterItem &Item)
        {
            return Item.Definition && Item.Definition->ItemId == Id;
        });
        if (Source == INDEX_NONE)
        {
            UE_LOG(LogBBBItemDebug, Error, TEXT("%s 缺少审核物品 %s"), *GetPathName(), *Id.ToString());
            return;
        }
        const auto &Item = Inventory.Slots[Source];
        const int32 WearIndex = Inventory.WearSlots.Find(Item.Definition->WearSlot);
        if (WearIndex == INDEX_NONE || Packet.Targets.Contains(Inventory.BackpackSlotCount + WearIndex))
        {
            UE_LOG(LogBBBItemDebug, Error, TEXT("%s 审核配装的穿戴位置不合法 %s"), *GetPathName(), *Id.ToString());
            return;
        }
        const int32 Target = Inventory.BackpackSlotCount + WearIndex;
        if (Source != Target)
        {
            Packet.Sources.Add(Source);
            Packet.Targets.Add(Target);
            Packet.Instances.Add(Item.InstanceId);
        }
    }
    if (Packet.IsValid())
    {
        Character.SubmitInput(MoveTemp(Packet));
    }
}
