#include "BBBWork/UBBBNexus/Customization/Appearance/BBBAppearanceComponent.h"
#include "GameFramework/Pawn.h"
#include "Engine/SkeletalMesh.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogBBBAppearance, Log, All);

UBBBAppearanceComponent::UBBBAppearanceComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UBBBAppearanceComponent::BeginPlay()
{
    Super::BeginPlay();

    //属性可能先于 BeginPlay 到达 有效的接收结果不能被默认组合覆盖
    if (!CurrentSelection.IsValid())
    {
        CurrentSelection = DefaultSelection;
    }
    OnRep_Selection();
}

void UBBBAppearanceComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION(UBBBAppearanceComponent, CurrentSelection, COND_SkipOwner);
}

FBBBAppearanceSelection UBBBAppearanceComponent::GetAppearance() const
{
    return CurrentSelection.IsValid() ? CurrentSelection : DefaultSelection;
}

bool UBBBAppearanceComponent::GetItem(const FName ItemId, FBBBAppearanceItem &Item) const
{
    if (!Catalog || Catalog->GetRowStruct() != FBBBAppearanceItem::StaticStruct())
    {
        UE_LOG(LogBBBAppearance, Error, TEXT("外观目录缺失或行类型不符 Owner=%s"), *GetNameSafe(GetOwner()));
        return false;
    }

    const FBBBAppearanceItem *Row = Catalog->FindRow<FBBBAppearanceItem>(ItemId, TEXT("Appearance"), false);
    if (!Row)
    {
        UE_LOG(LogBBBAppearance, Error, TEXT("外观条目不存在 Item=%s"), *ItemId.ToString());
        return false;
    }
    Item = *Row;
    return true;
}

TArray<FName> UBBBAppearanceComponent::GetItems(const FName Slot) const
{
    TArray<FName> Items;
    if (!Catalog || Catalog->GetRowStruct() != FBBBAppearanceItem::StaticStruct())
    {
        return Items;
    }
    for (const FName Name : Catalog->GetRowNames())
    {
        const FBBBAppearanceItem *Item = Catalog->FindRow<FBBBAppearanceItem>(Name, TEXT("Appearance"), false);
        if (Item && Item->Slot == Slot)
        {
            Items.Add(Name);
        }
    }
    Items.Sort(FNameLexicalLess());
    return Items;
}

bool UBBBAppearanceComponent::LoadItem(const FName ItemId, USkeletalMesh *&Mesh, TArray<UMaterialInterface *> &Materials) const
{
    Mesh = nullptr;
    Materials.Reset();
    FBBBAppearanceItem Item;
    if (!GetItem(ItemId, Item))
    {
        return false;
    }
    if (Item.Mesh.IsNull())
    {
        return true;
    }
    Mesh = Item.Mesh.LoadSynchronous();
    if (!Mesh)
    {
        UE_LOG(LogBBBAppearance, Error, TEXT("外观模型加载失败 Item=%s Path=%s"), *ItemId.ToString(), *Item.Mesh.ToString());
        return false;
    }
    for (const FSkeletalMaterial &Material : Mesh->GetMaterials())
    {
        Materials.Add(Material.MaterialInterface);
    }
    return true;
}

bool UBBBAppearanceComponent::PrepareSelection(FBBBAppearanceSelection &Selection) const
{
    if (!Selection.IsValid() || Selection.Parts.Num() != DefaultSelection.Parts.Num())
    {
        return false;
    }

    FBBBAppearancePart *Legs = nullptr;
    FName RequiredLegStyle;
    for (FBBBAppearancePart &Part : Selection.Parts)
    {
        const bool bKnownSlot = DefaultSelection.Parts.ContainsByPredicate(
            [&Part](const FBBBAppearancePart &Default) { return Default.Slot == Part.Slot; });
        FBBBAppearanceItem Item;
        if (!bKnownSlot || !GetItem(Part.Item, Item) || Item.Slot != Part.Slot)
        {
            return false;
        }
        if (Part.Slot == TEXT("Legs"))
        {
            Legs = &Part;
        }
        if (Part.Slot == TEXT("Boots"))
        {
            RequiredLegStyle = Item.RequiredLegStyle;
        }
    }

    //同款裤腿的关系由表显式配置 不再依赖资源数组下标的奇偶性
    if (Legs && !RequiredLegStyle.IsNone())
    {
        FBBBAppearanceItem Item;
        if (!GetItem(Legs->Item, Item))
        {
            return false;
        }
        if (Item.LegStyle != RequiredLegStyle)
        {
            FBBBAppearanceItem Alternate;
            if (!GetItem(Item.AlternateLegItem, Alternate)
                || Alternate.Slot != Legs->Slot || Alternate.LegStyle != RequiredLegStyle)
            {
                UE_LOG(LogBBBAppearance, Warning, TEXT("裤靴搭配配置不完整 Item=%s"), *Legs->Item.ToString());
                return false;
            }
            Legs->Item = Item.AlternateLegItem;
        }
    }

    if (!Selection.Attachments.IsNone())
    {
        FBBBAppearanceItem Item;
        if (!GetItem(Selection.Attachments, Item) || Item.Slot != TEXT("Attachments"))
        {
            return false;
        }
    }
    return true;
}

bool UBBBAppearanceComponent::PreviewAppearance(const FBBBAppearanceSelection &Selection)
{
    if (!Selection.IsValid() || !GetOwner() || GetOwner()->GetIsReplicated())
    {
        UE_LOG(LogBBBAppearance, Warning, TEXT("预览只能作用于本地展示对象"));
        return false;
    }
    return ApplyAppearance(Selection);
}

bool UBBBAppearanceComponent::CommitAppearance(const FBBBAppearanceSelection &Selection)
{
    const APawn *Pawn = Cast<APawn>(GetOwner());
    if (!Selection.IsValid() || !Pawn || !Pawn->IsLocallyControlled())
    {
        UE_LOG(LogBBBAppearance, Warning, TEXT("外观应用被拒绝 目标不是本机控制的人物或组合结构无效"));
        return false;
    }
    if (!ApplyAppearance(Selection))
    {
        UE_LOG(LogBBBAppearance, Error, TEXT("蓝图外观组装失败 Owner=%s"), *GetNameSafe(GetOwner()));
        return false;
    }

    CurrentSelection = Selection;
    if (GetOwner()->HasAuthority())
    {
        GetOwner()->FlushNetDormancy();
        GetOwner()->ForceNetUpdate();
        return true;
    }

    //本机结果已经成立 服务端不重新执行搭配或预测回滚
    ServerCommitAppearance(Selection);
    return true;
}

void UBBBAppearanceComponent::ServerCommitAppearance_Implementation(FBBBAppearanceSelection Selection)
{
    if (!Selection.IsValid())
    {
        UE_LOG(LogBBBAppearance, Warning, TEXT("网络外观组合结构无效 Owner=%s"), *GetNameSafe(GetOwner()));
        return;
    }
    CurrentSelection = MoveTemp(Selection);
    OnRep_Selection();
    GetOwner()->FlushNetDormancy();
    GetOwner()->ForceNetUpdate();
}

void UBBBAppearanceComponent::OnRep_Selection()
{
    //专用服务器只保存结果 后加入客户端仍通过属性复制获得完整组合
    if (!HasBegunPlay() || GetNetMode() == NM_DedicatedServer)
    {
        return;
    }
    if (!CurrentSelection.IsValid() || !ApplyAppearance(CurrentSelection))
    {
        UE_LOG(LogBBBAppearance, Error, TEXT("无法显示外观组合 Owner=%s"), *GetNameSafe(GetOwner()));
    }
}
