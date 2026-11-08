#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Presentation/BBBProjectilePresentation.h"

#include "MassCommonFragments.h"
#include "NiagaraDataChannelFunctionLibrary.h"
#include "NiagaraDataChannelAccessor.h"
#include "NiagaraDataChannel.h"
#include "NiagaraDataChannelHandler.h"
#include "NiagaraDataChannelData.h"
#include "Misc/App.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Collision/BBBProjectileImpact.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Movement/BBBProjectileMotionFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Presentation/BBBProjectilePresentationFragment.h"

void FBBBProjectilePresentation::Publish(UWorld& World, TConstArrayView<FTransformFragment> Transforms,
    TConstArrayView<FBBBProjectileMotionFragment> Motion, TConstArrayView<FBBBProjectilePresentationFragment> Presentation)
{
    if (!ensureMsgf(!Presentation.IsEmpty()
        && Transforms.Num() == Motion.Num() && Motion.Num() == Presentation.Num(),
        TEXT("子弹表现批量数据长度不一致")))
    {
        return;
    }

    UNiagaraDataChannelAsset* Asset = Presentation[0].Channel.Get();
    UNiagaraDataChannel* Channel = Asset != nullptr ? Asset->Get() : nullptr;
    if (!ensureMsgf(Channel != nullptr, TEXT("子弹共享光效通道尚未配置")))
    {
        return;
    }

    int32 RecordCount = 0;
    for (const FBBBProjectilePresentationFragment& Visual : Presentation)
    {
        RecordCount = FMath::Max(RecordCount, Visual.Slot + 1);
    }

    FNDCAccessContextInst AccessContext(Channel->GetAccessContextType());
    UNiagaraDataChannelWriter* Writer = UNiagaraDataChannelLibrary::WriteToNiagaraDataChannel_WithContext(
        &World, Asset, AccessContext, RecordCount, false, true, false, TEXT("BBBMassProjectile"));
    if (!ensureMsgf(Writer != nullptr, TEXT("子弹共享光效通道写入失败")))
    {
        return;
    }

    // 空槽也占据固定记录索引 粒子直接以自己的槽位读取而无需全表搜索
    for (int32 Slot = 0; Slot < RecordCount; ++Slot)
    {
        Writer->WriteInt(TEXT("Spawn"), Slot, 0);
        Writer->WriteBool(TEXT("Active"), Slot, false);
        Writer->WriteBool(TEXT("Ending"), Slot, false);
    }

    for (int32 Index = 0; Index < Presentation.Num(); ++Index)
    {
        const FBBBProjectilePresentationFragment& Visual = Presentation[Index];
        const FVector End = Transforms[Index].GetTransform().GetLocation();
        const FVector Delta = End - Motion[Index].PreviousLocation;
        const FVector Direction = Delta.IsNearlyZero()
            ? Transforms[Index].GetTransform().GetUnitAxis(EAxis::X)
            : Delta.GetSafeNormal();

        const double TravelDistanceCm = FVector::Distance(End, Motion[Index].SpawnLocation);
        if (!ensureMsgf(FMath::IsFinite(TravelDistanceCm), TEXT("子弹光段的出生位置或飞行位置无效")))
        {
            continue;
        }

        // 光段只能覆盖子弹已经飞过的距离 避免出生阶段的尾端伸回枪口后方
        const double VisibleLengthCm = FMath::Min(static_cast<double>(Visual.LengthCm), TravelDistanceCm);

        Writer->WriteInt(TEXT("Spawn"), Visual.Slot, Visual.bSpawnPending ? 1 : 0);
        Writer->WriteBool(TEXT("Active"), Visual.Slot, Visual.bVisualAlive);
        Writer->WriteBool(TEXT("Ending"), Visual.Slot, !Visual.bVisualAlive);
        Writer->WritePosition(TEXT("Position"), Visual.Slot, End - Direction * VisibleLengthCm * 0.5);
        Writer->WriteVector(TEXT("SpriteAlignment"), Visual.Slot, Direction);
        Writer->WriteVector2D(TEXT("SpriteSize"), Visual.Slot, FVector2D(Visual.WidthCm, VisibleLengthCm));
        Writer->WriteLinearColor(TEXT("Color"), Visual.Slot, Visual.Color);
    }

    UNiagaraDataChannelHandler* Handler = UNiagaraDataChannelLibrary::FindDataChannelHandler(&World, Channel);
    FNiagaraDataChannelDataPtr Data = Handler != nullptr
        ? Handler->FindData(AccessContext, ENiagaraResourceAccess::WriteOnly)
        : nullptr;
    if (!ensureMsgf(Data.IsValid(), TEXT("子弹光效无法发布当帧通道缓冲")))
    {
        return;
    }

    // 帧末写入晚于普通通道收集 主动提交当前批次供后置共享组件读取
    Data->ConsumePublishRequests(Handler, TG_LastDemotable);
}

void FBBBProjectilePresentation::PublishImpacts(UWorld& World, UNiagaraDataChannelAsset& Asset,
    TConstArrayView<FBBBProjectileImpact> Impacts)
{
    if (Impacts.IsEmpty() || World.GetNetMode() == NM_DedicatedServer || !FApp::CanEverRender())
    {
        return;
    }

    UNiagaraDataChannel* Channel = Asset.Get();
    if (!ensureMsgf(Channel != nullptr, TEXT("子弹命中通道尚未配置")))
    {
        return;
    }

    UNiagaraDataChannelHandler* Handler = UNiagaraDataChannelLibrary::FindDataChannelHandler(&World, Channel);
    if (!ensureMsgf(Handler != nullptr, TEXT("子弹命中通道缺少世界处理器")))
    {
        return;
    }

    // 按通道返回的实际空间岛整理当帧索引 不将远处命中强行归到世界原点
    TMap<FNiagaraDataChannelData*, TArray<int32>> Batches;
    for (int32 Index = 0; Index < Impacts.Num(); ++Index)
    {
        FNDCAccessContextInst AccessContext(Channel->GetAccessContextType());
        AccessContext.GetChecked<FNDCAccessContextLegacy>() = FNDCAccessContextLegacy(Impacts[Index].Position);
        FNiagaraDataChannelDataPtr Data = Handler->FindData(AccessContext, ENiagaraResourceAccess::WriteOnly);
        if (ensureMsgf(Data.IsValid(), TEXT("子弹命中无法找到空间岛")))
        {
            Batches.FindOrAdd(Data.Get()).Add(Index);
        }
    }

    for (const auto& Batch : Batches)
    {
        FNDCAccessContextInst AccessContext(Channel->GetAccessContextType());
        AccessContext.GetChecked<FNDCAccessContextLegacy>() = FNDCAccessContextLegacy(Impacts[Batch.Value[0]].Position);
        UNiagaraDataChannelWriter* Writer = UNiagaraDataChannelLibrary::WriteToNiagaraDataChannel_WithContext(
            &World, &Asset, AccessContext, Batch.Value.Num(), false, true, false, TEXT("BBBMassProjectileImpact"));
        if (!ensureMsgf(Writer != nullptr, TEXT("子弹命中批次写入失败")))
        {
            continue;
        }

        for (int32 Index = 0; Index < Batch.Value.Num(); ++Index)
        {
            const FBBBProjectileImpact& Impact = Impacts[Batch.Value[Index]];
            Writer->WritePosition(TEXT("Position"), Index, Impact.Position);
            Writer->WriteVector(TEXT("Normal"), Index, Impact.Normal);
            Writer->WriteInt(TEXT("Surface"), Index, static_cast<int32>(Impact.Surface));
        }
    }
}
