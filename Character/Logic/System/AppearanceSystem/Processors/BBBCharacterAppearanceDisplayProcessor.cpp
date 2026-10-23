#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/Processors/BBBCharacterAppearanceDisplayProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/Context/BBBCharacterAppearanceUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/SkeletalMesh.h"
#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "PhysicsEngine/BodyInstance.h"
#endif

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
        const FTransform &RelativeTransform, const bool bRigid, const bool bAttachment = false)
    {
        if (!Body || &Component == Body || Body->IsAttachedTo(&Component)
            || (!Socket.IsNone() && !Body->DoesSocketExist(Socket)))
        {
            return false;
        }
        const bool bKeepSimulation = bAttachment && Mesh && Component.GetSkeletalMeshAsset() == Mesh
            && Component.GetAttachParent() == Body && Component.GetAttachSocketName() == Socket
            && Component.IsAnySimulatingPhysics();
        if (!bKeepSimulation)
        {
            Component.SetAllBodiesSimulatePhysics(false);
            Component.SetAllBodiesPhysicsBlendWeight(0.0f);
            Component.SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
        Component.SetLeaderPoseComponent(nullptr);
        if (!Component.AttachToComponent(Body, FAttachmentTransformRules::SnapToTargetNotIncludingScale, Socket))
        {
            return false;
        }
        Component.SetRelativeTransform(RelativeTransform);
        Component.SetForceRefPose(bRigid && !bAttachment);
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
        if (bAttachment && Mesh && !bKeepSimulation)
        {
            if (!Component.GetPhysicsAsset())
            {
                return false;
            }
            Component.VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
            Component.SetCollisionObjectType(ECC_PhysicsBody);
            Component.SetCollisionResponseToAllChannels(ECR_Ignore);
            Component.SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
            Component.SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
            Component.ResetAllBodiesSimulatePhysics();
            Component.SetAllBodiesPhysicsBlendWeight(1.0f);
            Component.WakeAllRigidBodies();
        }
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
                    Attachment.Value.Materials, Attachment.Key, FTransform::Identity, true, true))
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

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBWaistAttachmentPhysicsTest, "BBB.Character.Appearance.WaistAttachmentPhysics",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBWaistAttachmentPhysicsTest::RunTest(const FString &Parameters)
{
    const auto Options = UWorld::InitializationValues().AllowAudioPlayback(false).CreatePhysicsScene(true)
        .CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(true);
    UWorld *World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Options);
    if (!TestNotNull(TEXT("挂件隔离物理世界"), World))
    {
        return false;
    }
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
    };
    AActor *Actor = World->SpawnActor<AActor>();
    auto *Body = NewObject<USkeletalMeshComponent>(Actor);
    Actor->SetRootComponent(Body);
    Body->RegisterComponent();
    auto *Attachment = NewObject<USkeletalMeshComponent>(Actor);
    Attachment->RegisterComponent();
    const TCHAR *Assets[] =
    {
        TEXT("/Game/_Project/Characters/BBBC_UA/Items/Wearable/Attachments/Attachments_Gloves/Meshes/SK_UA_HangingGloves.SK_UA_HangingGloves"),
        TEXT("/Game/_Project/Characters/BBBC_UA/Items/Wearable/Attachments/Attachments_DropBag/Meshes/SK_UA_DropBag.SK_UA_DropBag"),
        TEXT("/Game/_Project/Characters/BBBC_UA/Items/Wearable/Attachments/Attachments_Carpet/Meshes/SK_UA_RolledMat.SK_UA_RolledMat")
    };
    for (const TCHAR *Path : Assets)
    {
        auto *Mesh = LoadObject<USkeletalMesh>(nullptr, Path);
        if (!TestNotNull(Path, Mesh))
        {
            continue;
        }
        TestTrue(TEXT("挂件应用成功"), ApplyMesh(*Attachment, Body, Mesh, {}, NAME_None, FTransform::Identity, true, true));
        auto *Base = Attachment->GetBodyInstance(TEXT("base"));
        auto *Link = Attachment->GetBodyInstance(TEXT("link01"));
        if (!TestNotNull(TEXT("固定端刚体"), Base) || !TestNotNull(TEXT("摆动端刚体"), Link))
        {
            continue;
        }
        TestFalse(TEXT("固定端不参与自由模拟"), Base->IsInstanceSimulatingPhysics());
        TestTrue(TEXT("摆动端已启用模拟"), Link->IsInstanceSimulatingPhysics());
        TestTrue(TEXT("组件保留腰部依附关系"), Attachment->GetAttachParent() == Body);
        Link->SetAngularVelocityInRadians(FVector(0.3f, 0.2f, 0.1f), false);
        const FVector Velocity = Link->GetUnrealWorldAngularVelocityInRadians();
        TestTrue(TEXT("材质刷新复用模拟"), ApplyMesh(*Attachment, Body, Mesh, {}, NAME_None, FTransform::Identity, true, true));
        TestTrue(TEXT("刷新不会清空摆动速度"), Velocity.Equals(Link->GetUnrealWorldAngularVelocityInRadians(), 0.001f));
        const FTransform FixedTransform = Base->GetUnrealWorldTransform();
        const FTransform MovingTransform = Link->GetUnrealWorldTransform();
        for (int32 Frame = 0; Frame < 30; ++Frame)
        {
            ++GFrameCounter;
            World->Tick(LEVELTICK_All, 1.0f / 60.0f);
        }
        TestFalse(TEXT("摆动端实际发生运动"), MovingTransform.Equals(Link->GetUnrealWorldTransform(), 0.001f));
        TestFalse(TEXT("摆动结果没有无效数值"), Link->GetUnrealWorldTransform().ContainsNaN());
        TestTrue(TEXT("固定端没有被拖走"), FixedTransform.Equals(Base->GetUnrealWorldTransform(), 0.01f));
        TestTrue(TEXT("卸下挂件成功"), ApplyMesh(*Attachment, Body, nullptr, {}, NAME_None, FTransform::Identity, false));
        TestFalse(TEXT("卸下后没有遗留模拟"), Attachment->IsAnySimulatingPhysics());
        TestFalse(TEXT("卸下后隐藏挂件"), Attachment->IsVisible());
    }
    return true;
}
#endif
