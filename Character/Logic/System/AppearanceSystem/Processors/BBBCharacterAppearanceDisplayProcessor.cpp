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
                if (!Component || !Character.GetMesh() || Component == Character.GetMesh()
                    || Targets.Contains(*Name) || Character.GetMesh()->IsAttachedTo(Component)
                    || (!Part.AttachBone.IsNone() && !Character.GetMesh()->DoesSocketExist(Part.AttachBone)))
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

    bool ApplyMesh(USkeletalMeshComponent &Component, USkeletalMeshComponent *Body, USkeletalMesh *Mesh,
        const TArray<TObjectPtr<UMaterialInterface>> &Materials, const FName Socket,
        const FTransform &RelativeTransform, const bool bRigid)
    {
        if (!Body || &Component == Body || Body->IsAttachedTo(&Component)
            || (!Socket.IsNone() && !Body->DoesSocketExist(Socket)))
        {
            return false;
        }
        Component.SetLeaderPoseComponent(nullptr);
        if (!Component.AttachToComponent(Body, FAttachmentTransformRules::SnapToTargetNotIncludingScale, Socket))
        {
            return false;
        }
        Component.SetRelativeTransform(RelativeTransform);
        Component.SetForceRefPose(bRigid);
        if (bRigid)
        {
            Component.SetAnimationMode(EAnimationMode::AnimationSingleNode);
            Component.SetAnimation(nullptr);
        }
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
        Component.SetLeaderPoseComponent(Mesh && !bRigid ? Body : nullptr);
        return true;
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
                    if (ApplyMesh(*Component, Character.GetMesh(), Part.Mesh, Part.Materials,
                        Part.AttachBone, Part.RelativeTransform, !Part.AttachBone.IsNone()))
                    {
                        Updated.Add(*Name);
                    }
                    if (!bAvailableOnly && !Updated.Contains(*Name))
                    {
                        return false;
                    }
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
                if (!ApplyMesh(*Component, Character.GetMesh(), Attachment.Value.Mesh,
                    Attachment.Value.Materials, Attachment.Key, FTransform::Identity, true))
                {
                    if (!bAvailableOnly)
                    {
                        return false;
                    }
                    continue;
                }
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
                        ApplyMesh(*Component, Character.GetMesh(), nullptr, {}, NAME_None,
                            FTransform::Identity, false);
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
    Display.bApplied = Context.bResourcesReady
        && ApplyNative(Context.Character, Display.Parts, Context.Config, false);
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
