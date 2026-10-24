#include "BBBWork/UBBBNexus/Debug/Mass/BBBZombieHitTrajectoryComponent.h"

#include "Animation/AnimSingleNodeInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Dom/JsonObject.h"
#include "Engine/World.h"
#include "HAL/FileManager.h"
#include "HitReactProfile.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "PhysicsEngine/BodyInstance.h"
#include "PhysicsEngine/ConstraintInstance.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "PhysicsEngine/PhysicsConstraintTemplate.h"
#include "PhysicsEngine/SkeletalBodySetup.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "BBBWork/UBBBNexus/Debug/Mass/BBBZombieHitReactionTarget.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterHitReactionComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPhysicalAnimationComponent.h"

namespace BBBZombieTrajectory
{
    TArray<TSharedPtr<FJsonValue>> Vector(const FVector& V)
    {
        return {MakeShared<FJsonValueNumber>(V.X), MakeShared<FJsonValueNumber>(V.Y), MakeShared<FJsonValueNumber>(V.Z)};
    }
    TArray<TSharedPtr<FJsonValue>> Quat(const FQuat& Q)
    {
        return {MakeShared<FJsonValueNumber>(Q.X), MakeShared<FJsonValueNumber>(Q.Y), MakeShared<FJsonValueNumber>(Q.Z), MakeShared<FJsonValueNumber>(Q.W)};
    }
    double Number(const TSharedPtr<FJsonObject>& O, const TCHAR* Key, double Default)
    {
        double Value = Default;
        O->TryGetNumberField(Key, Value);
        return Value;
    }
    FString Json(const TSharedPtr<FJsonObject>& O)
    {
        FString Result;
        FJsonSerializer::Serialize(O.ToSharedRef(), TJsonWriterFactory<>::Create(&Result));
        return Result;
    }
}

UBBBZombieHitTrajectoryComponent::UBBBZombieHitTrajectoryComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;
    PrimaryComponentTick.TickGroup = TG_PostUpdateWork;
}

FString UBBBZombieHitTrajectoryComponent::Start(const FString& RequestJson)
{
    using namespace BBBZombieTrajectory;
    auto* Target = Cast<ABBBZombieHitReactionTarget>(GetOwner());
    if (!Target || !GetWorld() || !GetWorld()->IsGameWorld() || IsComponentTickEnabled())
    {
        return TEXT("拒绝 需要空闲的游戏世界调试靶");
    }
    if (!Target->bConfigurationReady || !Target->TestAnimation)
    {
        return TEXT("拒绝 请先应用有效版本并指定兼容的测试动画");
    }
    if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(RequestJson), Request) || !Request.IsValid())
    {
        return TEXT("拒绝 配置不是有效对象");
    }
    const FString Id = Request->GetStringField(TEXT("id"));
    bool bSafeId = !Id.IsEmpty() && Id.Len() <= 96;
    for (const TCHAR C : Id)
    {
        bSafeId &= FChar::IsAlnum(C) || C == '_' || C == '-';
    }
    const TArray<TSharedPtr<FJsonValue>>* Directions = nullptr;
    if (!Request->TryGetArrayField(TEXT("direction"), Directions) || Directions->Num() != 3)
    {
        return TEXT("拒绝 方向必须为三元数组");
    }
    double DirectionLength = 0;
    for (const auto& Value : *Directions)
    {
        if (!FMath::IsFinite(Value->AsNumber()))
        {
            return TEXT("拒绝 方向存在非有限数值");
        }
        DirectionLength += FMath::Square(Value->AsNumber());
    }
    if (!bSafeId || DirectionLength < .001
        || Number(Request, TEXT("shots"), 1) < 0 || Number(Request, TEXT("shots"), 1) > 12
        || Number(Request, TEXT("region"), 0) < 0 || Number(Request, TEXT("region"), 0) > 5
        || Number(Request, TEXT("duration"), 3) < 1 || Number(Request, TEXT("duration"), 3) > 10
        || Number(Request, TEXT("interval"), .1) < .05 || Number(Request, TEXT("interval"), .1) > .5)
    {
        return TEXT("拒绝 试验标识或采样范围无效");
    }
    OutputPath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("temp/ZombieHitTrajectory20261016"), Id + TEXT(".json"));
    if (FPaths::FileExists(OutputPath))
    {
        return TEXT("拒绝 禁止覆盖已有轨迹");
    }

    Target->HitReaction->ResetPresentation();
    auto* Animation = Target->TargetMesh->AnimationData.AnimToPlay.Get();
    const float Phase = Number(Request, TEXT("phase"), 0);
    Target->TargetMesh->bPauseAnims = false;
    Target->TargetMesh->PlayAnimation(Animation, true);
    Target->TargetMesh->SetPosition(Phase, false);
    Target->TargetMesh->TickAnimation(0, false);
    Target->TargetMesh->RefreshBoneTransforms();
    // 每次从当前版本重新取得来源 防止切换版本后复用旧副本
    SourcePhysics = Target->TargetMesh->GetPhysicsAsset();
    SourceProfiles.Reset();
    for (auto& Reference : Target->HitReaction->AvailableProfiles)
    {
        SourceProfiles.Add(Reference.LoadSynchronous());
    }
    ProbePhysics = DuplicateObject<UPhysicsAsset>(SourcePhysics, this,
        MakeUniqueObjectName(this, UPhysicsAsset::StaticClass(), TEXT("ProbePhysics")));
    const TSharedPtr<FJsonObject>* TunePtr = nullptr;
    if (Request->TryGetObjectField(TEXT("tune"), TunePtr))
    {
        const auto& Tune = *TunePtr;
        for (USkeletalBodySetup* Body : ProbePhysics->SkeletalBodySetups)
        {
            const FString Bone = Body->BoneName.ToString();
            auto* Profile = Body->FindPhysicalAnimationProfile(TEXT("BBBHitReact"));
            if (Profile)
            {
                auto& Data = Profile->PhysicalAnimationData;
                double Factor = Bone.Contains(TEXT("thigh")) || Bone.Contains(TEXT("calf")) || Bone.Contains(TEXT("foot")) ? .5 : 1;
                if (Bone == TEXT("head"))
                {
                    Factor = .65;
                }
                Data.OrientationStrength = Number(Tune, TEXT("orientation"), Data.OrientationStrength / Factor) * Factor;
                const double DampingFactor = Factor == .5 ? .75 : 1;
                Data.AngularVelocityStrength = Number(Tune, TEXT("driveDamping"), Data.AngularVelocityStrength / DampingFactor) * DampingFactor;
            }
            Body->DefaultInstance.LinearDamping = Number(Tune, TEXT("linearDamping"), Body->DefaultInstance.LinearDamping);
            Body->DefaultInstance.AngularDamping = Number(Tune, TEXT("angularDamping"), Body->DefaultInstance.AngularDamping);
            Body->DefaultInstance.bEnableGravity = false;
            Body->DefaultInstance.SetOverrideIterationCounts(true);
            Body->DefaultInstance.SetPositionSolverIterationCount(Number(Tune, TEXT("positionIterations"), 16));
            Body->DefaultInstance.SetVelocitySolverIterationCount(Number(Tune, TEXT("velocityIterations"), 4));
            Body->DefaultInstance.SetProjectionSolverIterationCount(Number(Tune, TEXT("projectionIterations"), 2));
            Body->DefaultInstance.InertiaTensorScale = FVector(Number(Tune, TEXT("inertiaScale"), 1));
            Body->DefaultInstance.bOverrideMaxAngularVelocity = true;
            Body->DefaultInstance.MaxAngularVelocity = Number(Tune, TEXT("maxAngularVelocity"), 1200);
        }
        for (UPhysicsConstraintTemplate* Joint : ProbePhysics->ConstraintSetup)
        {
            auto& C = Joint->DefaultInstance;
            const FString Bone = C.ConstraintBone1.ToString();
            if (Tune->HasField(TEXT("neutralFrames")) && Tune->GetBoolField(TEXT("neutralFrames")))
            {
                const FTransform ChildPose = Target->TargetMesh->GetSocketTransform(C.ConstraintBone1, RTS_World);
                const FTransform ParentPose = Target->TargetMesh->GetSocketTransform(C.ConstraintBone2, RTS_World);
                C.SetRefFrame(EConstraintFrame::Frame2,
                    (C.GetRefFrame(EConstraintFrame::Frame1) * ChildPose).GetRelativeTransform(ParentPose));
            }
            const TSharedPtr<FJsonObject>* Limits = nullptr;
            if (Tune->TryGetObjectField(TEXT("limits"), Limits))
            {
                FString Group = TEXT("hand");
                if (Bone.Contains(TEXT("spine"))) { Group = TEXT("spine"); }
                if (Bone == TEXT("head")) { Group = TEXT("head"); }
                if (Bone.Contains(TEXT("upperarm"))) { Group = TEXT("upperarm"); }
                if (Bone.Contains(TEXT("lowerarm"))) { Group = TEXT("lowerarm"); }
                if (Bone.Contains(TEXT("thigh"))) { Group = TEXT("thigh"); }
                if (Bone.Contains(TEXT("calf"))) { Group = TEXT("calf"); }
                if (Bone.Contains(TEXT("foot"))) { Group = TEXT("foot"); }
                if ((*Limits)->HasField(Bone))
                {
                    Group = Bone;
                }
                const TArray<TSharedPtr<FJsonValue>>* Angles = nullptr;
                if ((*Limits)->TryGetArrayField(Group, Angles) && Angles->Num() == 3)
                {
                    C.SetAngularSwing1Limit(ACM_Limited, (*Angles)[0]->AsNumber());
                    C.SetAngularSwing2Limit(ACM_Limited, (*Angles)[1]->AsNumber());
                    C.SetAngularTwistLimit(ACM_Limited, (*Angles)[2]->AsNumber());
                }
            }
            C.SetLinearLimits(LCM_Locked, LCM_Locked, LCM_Locked, 0);
            C.ProfileInstance.bDisableCollision = true;
            C.ProfileInstance.bParentDominates = Tune->HasField(TEXT("parentDominates")) && Tune->GetBoolField(TEXT("parentDominates"));
            C.ProfileInstance.bUseLinearJointSolver = !Tune->HasField(TEXT("linearSolver")) || Tune->GetBoolField(TEXT("linearSolver"));
            C.ProfileInstance.ConeLimit.bSoftConstraint = false;
            C.ProfileInstance.TwistLimit.bSoftConstraint = false;
            C.SetProjectionParams(true, Number(Tune, TEXT("linearProjection"), .8),
                Number(Tune, TEXT("angularProjection"), .2), Number(Tune, TEXT("linearTolerance"), .1),
                Number(Tune, TEXT("angularTolerance"), 1));
#if WITH_EDITOR
            Joint->SetDefaultProfile(C);
#endif
        }
    }
    Target->TargetMesh->SetPhysicsAsset(ProbePhysics, true);
    ProbeProfiles.Reset();
    for (UHitReactProfile* Source : SourceProfiles)
    {
        auto* Profile = DuplicateObject<UHitReactProfile>(Source, this,
            MakeUniqueObjectName(this, UHitReactProfile::StaticClass(), TEXT("ProbeProfile")));
        if (TunePtr)
        {
            const auto& Tune = *TunePtr;
            Profile->MaxBlendWeight = Number(Tune, TEXT("weight"), Profile->MaxBlendWeight);
            Profile->BoneBlendRate = Number(Tune, TEXT("rate"), Profile->BoneBlendRate);
            Profile->BlendParams.BlendIn.BlendTime = Number(Tune, TEXT("in"), Profile->BlendParams.BlendIn.BlendTime);
            Profile->BlendParams.BlendHoldTime = Number(Tune, TEXT("hold"), Profile->BlendParams.BlendHoldTime);
            Profile->BlendParams.BlendOut.BlendTime = Number(Tune, TEXT("out"), Profile->BlendParams.BlendOut.BlendTime);
            if (Tune->HasField(TEXT("subsequent")))
            {
                Profile->SubsequentImpulseScalars = {FHitReactSubsequentImpulse(.2f,
                    Number(Tune, TEXT("subsequent"), 1))};
            }
            if (Tune->HasField(TEXT("cleanOverrides")) && Tune->GetBoolField(TEXT("cleanOverrides")))
            {
                Profile->BoneOverrides.Reset();
            }
        }
        ProbeProfiles.Add(Profile);
    }
    Target->HitReaction->AvailableProfiles.Reset();
    for (UHitReactProfile* Profile : ProbeProfiles)
    {
        Target->HitReaction->AvailableProfiles.Add(TSoftObjectPtr<UHitReactProfile>(Profile));
    }
    Target->HitReaction->Activate(true);
    Target->HitCount = 0;

    if (ReferenceActor)
    {
        ReferenceActor->Destroy();
    }
    ReferenceActor = GetWorld()->SpawnActor<AActor>();
    ReferenceMesh = NewObject<USkeletalMeshComponent>(ReferenceActor, TEXT("TrajectoryReference"));
    ReferenceActor->SetRootComponent(ReferenceMesh);
    ReferenceMesh->SetWorldTransform(Target->TargetMesh->GetComponentTransform());
    ReferenceMesh->SetSkeletalMesh(Target->TargetMesh->GetSkeletalMeshAsset());
    ReferenceMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ReferenceMesh->SetHiddenInGame(true);
    ReferenceMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
    ReferenceMesh->RegisterComponent();
    ReferenceMesh->PlayAnimation(Animation, true);
    Target->TargetMesh->SetPosition(Phase, false);
    ReferenceMesh->SetPosition(Phase, false);
    const bool bFrozen = Request->HasField(TEXT("frozen")) && Request->GetBoolField(TEXT("frozen"));
    Target->TargetMesh->bPauseAnims = bFrozen;
    ReferenceMesh->bPauseAnims = bFrozen;
    Target->TargetMesh->SetForcedLOD(1);
    ReferenceMesh->SetForcedLOD(1);
    AddTickPrerequisiteComponent(Target->TargetMesh);
    AddTickPrerequisiteComponent(ReferenceMesh);
    StartSeconds = GetWorld()->GetTimeSeconds() + .5;
    Fired = 0;
    Report = MakeShared<FJsonObject>();
    Report->SetNumberField(TEXT("schema"), 2);
    Report->SetStringField(TEXT("status"), TEXT("running"));
    Report->SetObjectField(TEXT("request"), Request);
    Report->SetStringField(TEXT("path"), FPaths::ConvertRelativePathToFull(OutputPath));
    Report->SetStringField(TEXT("samplingPhase"), TEXT("PostUpdateWork"));
    Report->SetArrayField(TEXT("samples"), {});
    Report->SetArrayField(TEXT("shots"), {});
    TArray<TSharedPtr<FJsonValue>> Configuration;
    for (const UPhysicsConstraintTemplate* Joint : ProbePhysics->ConstraintSetup)
    {
        const auto& C = Joint->DefaultInstance;
        auto Row = MakeShared<FJsonObject>();
        Row->SetStringField(TEXT("bone"), C.ConstraintBone1.ToString());
        Row->SetArrayField(TEXT("pos1"), Vector(C.Pos1));
        Row->SetArrayField(TEXT("pos2"), Vector(C.Pos2));
        Row->SetArrayField(TEXT("priAxis1"), Vector(C.PriAxis1));
        Row->SetArrayField(TEXT("secAxis1"), Vector(C.SecAxis1));
        Row->SetArrayField(TEXT("priAxis2"), Vector(C.PriAxis2));
        Row->SetArrayField(TEXT("secAxis2"), Vector(C.SecAxis2));
        Configuration.Add(MakeShared<FJsonValueObject>(Row));
    }
    Report->SetArrayField(TEXT("configuration"), Configuration);
    SetComponentTickEnabled(true);
    return Status();
}

FString UBBBZombieHitTrajectoryComponent::Status() const
{
    if (!Report.IsValid())
    {
        return TEXT("{\"status\":\"idle\"}");
    }
    auto Result = MakeShared<FJsonObject>();
    Result->SetStringField(TEXT("status"), Report->GetStringField(TEXT("status")));
    Result->SetStringField(TEXT("path"), Report->GetStringField(TEXT("path")));
    Result->SetNumberField(TEXT("frames"), Report->GetArrayField(TEXT("samples")).Num());
    return BBBZombieTrajectory::Json(Result);
}

void UBBBZombieHitTrajectoryComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (IsComponentTickEnabled() && Report.IsValid())
    {
        Complete(TEXT("采样世界已结束"));
    }
    Super::EndPlay(EndPlayReason);
}

void UBBBZombieHitTrajectoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    using namespace BBBZombieTrajectory;
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    auto* Target = CastChecked<ABBBZombieHitReactionTarget>(GetOwner());
    auto* Mesh = Target->TargetMesh.Get();
    const double Time = GetWorld()->GetTimeSeconds() - StartSeconds;
    if (Time < 0)
    {
        return;
    }
    const int32 Shots = Number(Request, TEXT("shots"), 1);
    const double Interval = Number(Request, TEXT("interval"), .1);
    bool bNewHit = false;
    if (Fired < Shots && Time >= .2 + Fired * Interval)
    {
        const FName Bones[] = {TEXT("spine_02"), TEXT("head"), TEXT("lowerarm_l"), TEXT("lowerarm_r"), TEXT("calf_l"), TEXT("calf_r")};
        const int32 Region = Number(Request, TEXT("region"), 0);
        const auto& Values = Request->GetArrayField(TEXT("direction"));
        FVector Direction(Values[0]->AsNumber(), Values[1]->AsNumber(), Values[2]->AsNumber());
        Direction = Target->GetActorTransform().TransformVectorNoScale(Direction).GetSafeNormal();
        const FBodyInstance* AimBody = Mesh->GetBodyInstance(Bones[Region]);
        const FVector Position = AimBody ? AimBody->GetCOMPosition() : Mesh->GetBoneLocation(Bones[Region]);
        FHitResult Hit;
        const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Position - Direction * 200, Position + Direction * 30, ECC_Pawn);
        const uint32 Before = Target->HitCount;
        if (bHit && Hit.GetActor() == Target)
        {
            UGameplayStatics::ApplyPointDamage(Target, 10, Direction, Hit, nullptr, nullptr, nullptr);
        }
        auto Row = MakeShared<FJsonObject>();
        Row->SetNumberField(TEXT("time"), Time);
        Row->SetBoolField(TEXT("hit"), Target->HitCount > Before);
        Row->SetNumberField(TEXT("region"), static_cast<int32>(Target->LastHitRegion));
        Row->SetStringField(TEXT("bone"), Hit.BoneName.ToString());
        auto Rows = Report->GetArrayField(TEXT("shots"));
        Rows.Add(MakeShared<FJsonValueObject>(Row));
        Report->SetArrayField(TEXT("shots"), Rows);
        ++Fired;
        bNewHit = true;
    }
    auto Sample = MakeShared<FJsonObject>();
    Sample->SetNumberField(TEXT("time"), Time);
    Sample->SetNumberField(TEXT("dt"), DeltaTime);
    Sample->SetArrayField(TEXT("root"), Vector(Target->GetActorLocation()));
    Sample->SetNumberField(TEXT("serial"), Target->HitReaction->GetObservedHitSerial());
    Sample->SetNumberField(TEXT("blends"), Target->HitReaction->GetPhysicsBlends().Num());
    auto Bones = MakeShared<FJsonObject>();
    for (USkeletalBodySetup* Setup : ProbePhysics->SkeletalBodySetups)
    {
        const FName Name = Setup->BoneName;
        const FTransform Bone = Mesh->GetSocketTransform(Name, RTS_World);
        const FTransform Reference = ReferenceMesh->GetSocketTransform(Name, RTS_World);
        auto Row = MakeShared<FJsonObject>();
        Row->SetArrayField(TEXT("p"), Vector(Bone.GetLocation()));
        Row->SetArrayField(TEXT("q"), Quat(Bone.GetRotation()));
        Row->SetArrayField(TEXT("rp"), Vector(Reference.GetLocation()));
        Row->SetArrayField(TEXT("rq"), Quat(Reference.GetRotation()));
        const FName ParentBone = Mesh->GetParentBone(Name);
        Row->SetStringField(TEXT("parent"), ParentBone.ToString());
        Row->SetArrayField(TEXT("pp"), Vector(Mesh->GetBoneLocation(ParentBone)));
        Row->SetArrayField(TEXT("rpp"), Vector(ReferenceMesh->GetBoneLocation(ParentBone)));
        if (auto* Body = Mesh->GetBodyInstance(Name))
        {
            const FTransform Pose = Body->GetUnrealWorldTransform();
            Row->SetArrayField(TEXT("bp"), Vector(Pose.GetLocation()));
            Row->SetArrayField(TEXT("bq"), Quat(Pose.GetRotation()));
            Row->SetArrayField(TEXT("v"), Vector(Body->GetUnrealWorldVelocity()));
            Row->SetArrayField(TEXT("w"), Vector(Body->GetUnrealWorldAngularVelocityInRadians()));
            Row->SetNumberField(TEXT("weight"), Body->PhysicsBlendWeight);
            Row->SetBoolField(TEXT("sim"), Body->IsInstanceSimulatingPhysics());
        }
        Bones->SetObjectField(Name.ToString(), Row);
    }
    Sample->SetObjectField(TEXT("bones"), Bones);
    TArray<TSharedPtr<FJsonValue>> Joints;
    for (UPhysicsConstraintTemplate* Setup : ProbePhysics->ConstraintSetup)
    {
        const auto& Definition = Setup->DefaultInstance;
        const auto* Child = Mesh->GetBodyInstance(Definition.ConstraintBone1);
        const auto* Parent = Mesh->GetBodyInstance(Definition.ConstraintBone2);
        if (!Child || !Parent)
        {
            continue;
        }
        const FTransform A = Definition.GetRefFrame(EConstraintFrame::Frame1) * Child->GetUnrealWorldTransform();
        const FTransform B = Definition.GetRefFrame(EConstraintFrame::Frame2) * Parent->GetUnrealWorldTransform();
        const FQuat Relative = B.GetRotation().Inverse() * A.GetRotation();
        FQuat Swing;
        FQuat Twist;
        Relative.ToSwingTwist(FVector::ForwardVector, Swing, Twist);
        auto Row = MakeShared<FJsonObject>();
        Row->SetStringField(TEXT("bone"), Definition.ConstraintBone1.ToString());
        Row->SetStringField(TEXT("parent"), Definition.ConstraintBone2.ToString());
        Row->SetBoolField(TEXT("active"), !bNewHit && (Child->IsInstanceSimulatingPhysics() || Parent->IsInstanceSimulatingPhysics()));
        Row->SetNumberField(TEXT("linearError"), FVector::Distance(A.GetLocation(), B.GetLocation()));
        Row->SetArrayField(TEXT("frameQ"), Quat(Relative));
        Row->SetNumberField(TEXT("s1"), FMath::RadiansToDegrees(FMath::UnwindRadians(2 * FMath::Atan2(Swing.Z, Swing.W))));
        Row->SetNumberField(TEXT("s2"), FMath::RadiansToDegrees(FMath::UnwindRadians(2 * FMath::Atan2(Swing.Y, Swing.W))));
        Row->SetNumberField(TEXT("twist"), FMath::RadiansToDegrees(FMath::UnwindRadians(2 * FMath::Atan2(Twist.X, Twist.W))));
        Row->SetArrayField(TEXT("limits"), {MakeShared<FJsonValueNumber>(Definition.GetAngularSwing1Limit()),
            MakeShared<FJsonValueNumber>(Definition.GetAngularSwing2Limit()), MakeShared<FJsonValueNumber>(Definition.GetAngularTwistLimit())});
        Joints.Add(MakeShared<FJsonValueObject>(Row));
    }
    Sample->SetArrayField(TEXT("joints"), Joints);
    auto Samples = Report->GetArrayField(TEXT("samples"));
    Samples.Add(MakeShared<FJsonValueObject>(Sample));
    Report->SetArrayField(TEXT("samples"), Samples);
    if (Time >= Number(Request, TEXT("duration"), 3))
    {
        Complete();
    }
}

void UBBBZombieHitTrajectoryComponent::Complete(const FString& Error)
{
    SetComponentTickEnabled(false);
    if (auto* Target = Cast<ABBBZombieHitReactionTarget>(GetOwner()))
    {
        Target->HitReaction->ResetPresentation();
        Target->HitReaction->Deactivate();
        Target->TargetMesh->SetPhysicsAsset(SourcePhysics, true);
        Target->HitReaction->AvailableProfiles.Reset();
        for (UHitReactProfile* Profile : SourceProfiles)
        {
            Target->HitReaction->AvailableProfiles.Add(Profile);
        }
        Target->HitReaction->Activate(true);
    }
    Report->SetStringField(TEXT("status"), Error.IsEmpty() ? TEXT("completed") : TEXT("failed"));
    Report->SetStringField(TEXT("error"), Error);
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(OutputPath), true);
    if (!FFileHelper::SaveStringToFile(BBBZombieTrajectory::Json(Report), *OutputPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
    {
        Report->SetStringField(TEXT("status"), TEXT("failed"));
        UE_LOG(LogTemp, Error, TEXT("[BBB轨迹]报告保存失败 %s"), *OutputPath);
    }
    UE_LOG(LogTemp, Display, TEXT("[BBB轨迹]采样完成 %s 帧数=%d"), *OutputPath, Report->GetArrayField(TEXT("samples")).Num());
    if (ReferenceActor)
    {
        ReferenceActor->Destroy();
        ReferenceActor = nullptr;
        ReferenceMesh = nullptr;
    }
    ProbePhysics = nullptr;
    ProbeProfiles.Reset();
    SourcePhysics = nullptr;
    SourceProfiles.Reset();
}
