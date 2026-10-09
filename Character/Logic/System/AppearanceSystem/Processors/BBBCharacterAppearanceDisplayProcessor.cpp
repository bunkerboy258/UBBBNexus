#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/Processors/BBBCharacterAppearanceDisplayProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/Context/BBBCharacterAppearanceUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"

namespace
{
    USkeletalMeshComponent *FindComponent(const TArray<USkeletalMeshComponent *> &Components, const FName Name)
    {
        USkeletalMeshComponent *Found = nullptr;
        for (auto *Component : Components)
        {
            if (Component->GetFName() == Name)
            {
                if (Found)
                {
                    return nullptr;
                }
                Found = Component;
            }
        }
        return Found;
    }

    bool CanApply(const ABBBCharacter &Character, const TArray<FBBBCharacterAppearanceDisplayPart> &Parts,
        const FBBBCharacterAppearanceConfig &Config, const TArray<USkeletalMeshComponent *> &Components)
    {
        TSet<FName> Targets;
        for (const auto &Part : Parts)
        {
            const FName *Name = Config.ComponentNames.Find(Part.Slot);
            if (!Name && (Part.Mesh || !Part.Materials.IsEmpty()))
            {
                return false;
            }
            if (Name)
            {
                const auto *Component = FindComponent(Components, *Name);
                if (!Component || Component == Character.GetMesh() || Targets.Contains(*Name))
                {
                    return false;
                }
                Targets.Add(*Name);
            }
            for (const auto &Attachment : Part.Attachments)
            {
                const auto *Component = FindComponent(Components, Attachment.Key);
                if (!Component || !Config.ComponentNames.FindKey(Attachment.Key)
                    || Targets.Contains(Attachment.Key) || !Character.GetMesh()
                    || Component == Character.GetMesh() || Character.GetMesh()->IsAttachedTo(Component)
                    || !Character.GetMesh()->DoesSocketExist(Attachment.Key))
                {
                    return false;
                }
                Targets.Add(Attachment.Key);
            }
        }
        return true;
    }

    void SetMesh(USkeletalMeshComponent &Component, USkeletalMesh *Mesh,
        const TArray<TObjectPtr<UMaterialInterface>> &Materials)
    {
        if (Component.GetSkeletalMeshAsset() != Mesh)
        {
            Component.SetSkeletalMesh(Mesh);
        }
        Component.SetVisibility(Mesh != nullptr);
        Component.SetHiddenInGame(Mesh == nullptr);
        Component.EmptyOverrideMaterials();
        for (int32 Index = 0; Index < Materials.Num(); ++Index)
        {
            Component.SetMaterial(Index, Materials[Index]);
        }
    }

    bool ApplyNative(ABBBCharacter &Character, const TArray<FBBBCharacterAppearanceDisplayPart> &Parts,
        const FBBBCharacterAppearanceConfig &Config, const bool bAvailableOnly)
    {
        TArray<USkeletalMeshComponent *> Components;
        Character.GetComponents(Components);
        if (!bAvailableOnly && !CanApply(Character, Parts, Config, Components))
        {
            return false;
        }
        TSet<FName> Updated;
        for (const auto &Part : Parts)
        {
            const FName *Name = Config.ComponentNames.Find(Part.Slot);
            if (auto *Component = Name ? FindComponent(Components, *Name) : nullptr)
            {
                if (Component != Character.GetMesh() && !Updated.Contains(*Name))
                {
                    SetMesh(*Component, Part.Mesh, Part.Materials);
                    Component->SetLeaderPoseComponent(Character.GetMesh());
                    Updated.Add(*Name);
                }
            }
            for (const auto &Attachment : Part.Attachments)
            {
                auto *Component = FindComponent(Components, Attachment.Key);
                if (!Component || !Character.GetMesh() || Component == Character.GetMesh()
                    || Updated.Contains(Attachment.Key) || Character.GetMesh()->IsAttachedTo(Component)
                    || !Character.GetMesh()->DoesSocketExist(Attachment.Key))
                {
                    continue;
                }
                Component->SetLeaderPoseComponent(nullptr);
                if (!Component->AttachToComponent(Character.GetMesh(),
                    FAttachmentTransformRules::SnapToTargetNotIncludingScale, Attachment.Key))
                {
                    if (!bAvailableOnly)
                    {
                        return false;
                    }
                    continue;
                }
                Component->SetRelativeTransform(FTransform::Identity);
                SetMesh(*Component, Attachment.Value.Mesh, Attachment.Value.Materials);
                Updated.Add(Attachment.Key);
            }
        }
        for (const auto &Mapping : Config.ComponentNames)
        {
            if (!Updated.Contains(Mapping.Value))
            {
                if (auto *Component = FindComponent(Components, Mapping.Value))
                {
                    if (Component != Character.GetMesh())
                    {
                        SetMesh(*Component, nullptr, {});
                    }
                }
            }
        }
        return true;
    }
}

void FBBBCharacterAppearanceDisplayProcessor::Update(FBBBCharacterAppearanceUpdateContext &Context) const
{
    if (!Context.bPrepareDisplay)
    {
        return;
    }
    const auto &Snapshot = Context.Data.Appearance.ReadCharacterAppearanceSelectionState().Snapshot;
    auto &Display = Context.Data.Appearance.DisplayState;
    Display.AttemptedRevision = Snapshot.Revision;
    Display.bFallbackApplied = false;
    if (Context.Character.GetNetMode() == NM_DedicatedServer)
    {
        Display.Parts.Reset();
        Display.FallbackParts.Reset();
        Display.AppliedRevision = Snapshot.Revision;
        Display.bApplied = true;
        return;
    }
    TArray<USkeletalMeshComponent *> Components;
    Context.Character.GetComponents(Components);
    Display.bApplied = Context.bResourcesReady
        && CanApply(Context.Character, Display.Parts, Context.Config, Components)
        && Context.Character.ApplyAppearanceDisplay(Display.Parts);
    if (Display.bApplied)
    {
        Display.AppliedRevision = Snapshot.Revision;
        Display.RetryAfterTime = 0.0f;
        return;
    }
    Display.bFallbackApplied = ApplyNative(Context.Character, Display.FallbackParts, Context.Config, true);
    Display.RetryAfterTime = Context.Data.External.ReadWorldState().WorldTimeSeconds + 1.0f;
    if (Context.bReportFailure)
    {
        UE_LOG(LogTemp, Error, TEXT("外观应用失败 已对可用组件执行基础回退 将间隔重试 Character=%s Revision=%llu"),
            *Context.Character.GetName(), Snapshot.Revision);
    }
}

void FBBBCharacterAppearanceDisplayProcessor::Shutdown(FBBBCharacterAppearanceUpdateContext &Context)
{
    ApplyNative(Context.Character, {}, Context.Config, true);
    auto &Display = Context.Data.Appearance.DisplayState;
    Display.Parts.Reset();
    Display.FallbackParts.Reset();
    Display.PreparedRevision = 0;
    Display.AppliedRevision = 0;
    Display.AttemptedRevision = 0;
    Display.RetryAfterTime = 0.0f;
    Display.bApplied = false;
    Display.bFallbackApplied = false;
}

bool FBBBCharacterAppearanceDisplayProcessor::ApplyDisplay(ABBBCharacter &Character,
    const TArray<FBBBCharacterAppearanceDisplayPart> &Parts, const FBBBCharacterAppearanceConfig &Config)
{
    return ApplyNative(Character, Parts, Config, false);
}
