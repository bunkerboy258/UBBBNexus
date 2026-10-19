#include "BBBZombieSurfaceWoundComponent.h"

#include "BBBZombieGoreSurfaceDefinition.h"
#include "BBBZombieGibPoseAnimInstance.h"
#include "BBBZombieHitReactionTarget.h"
#include "BBBZombieGoreDebugComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterBloodPresentationSubsystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "Rendering/SkeletalMeshLODRenderData.h"
#include "Rendering/SkinWeightVertexBuffer.h"

UBBBZombieSurfaceWoundComponent::UBBBZombieSurfaceWoundComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SelectedTiers.Init(0, 6);
    DripRemaining.Init(0, 6);
}

bool UBBBZombieSurfaceWoundComponent::InitializeSurface(UBBBZombieGoreSurfaceDefinition& Definition)
{
    auto* Target = Cast<ABBBZombieHitReactionTarget>(GetOwner());
    if (!Target || !Definition.Body || Definition.Cuts.Num() != 9 || Definition.AtlasSize != 2048)
    {
        return false;
    }
    Resources = &Definition;
    Body = Target->TargetMesh;
    Body->SetSkeletalMesh(Definition.Body);
    Body->SetForcedLOD(1);
    Mask = UTexture2D::CreateTransient(Definition.AtlasSize, Definition.AtlasSize, PF_B8G8R8A8);
    if (!Mask)
    {
        return false;
    }
    Mask->SRGB = false;
    Mask->NeverStream = true;
    Mask->Filter = TF_Bilinear;
    Mask->AddressX = TA_Clamp;
    Mask->AddressY = TA_Clamp;
    Mask->UpdateResource();
    Pixels.Init(FColor(0, 0, 0, 255), Definition.AtlasSize * Definition.AtlasSize);
    MaskBytes = Pixels.Num() * sizeof(FColor);
    Materials.Reset();
    for (int32 Index = 0; Index < Body->GetNumMaterials(); ++Index)
    {
        auto* Material = Body->CreateDynamicMaterialInstance(Index);
        if (!Material)
        {
            return false;
        }
        Material->SetTextureParameterValue(TEXT("TargetWoundMask"), Mask);
        Materials.Add(Material);
    }
    ResetSurface();
    return true;
}

void UBBBZombieSurfaceWoundComponent::UploadMask()
{
    if (!Mask || !bMaskDirty)
    {
        return;
    }
    const uint32 Size = Resources->AtlasSize;
    auto* Data = new uint8[MaskBytes];
    FMemory::Memcpy(Data, Pixels.GetData(), MaskBytes);
    auto* Region = new FUpdateTextureRegion2D(0, 0, 0, 0, Size, Size);
    Mask->UpdateTextureRegions(0, 1, Region, Size * sizeof(FColor), sizeof(FColor), Data,
        [](uint8* Source, const FUpdateTextureRegion2D* Update)
        {
            delete[] Source;
            delete Update;
        });
    bMaskDirty = false;
}

bool UBBBZombieSurfaceWoundComponent::Stamp(const FBBBMonsterHitReactionFragment& Hit, const float Strength)
{
    if (!Resources || !Body || Hit.Position.ContainsNaN() || !FMath::IsFinite(Strength))
    {
        return false;
    }
    const auto* Data = Body->GetSkeletalMeshAsset()->GetResourceForRendering();
    if (!Data || Data->LODRenderData.IsEmpty())
    {
        return false;
    }
    const auto& LOD = Data->LODRenderData[0];
    auto* Weights = Body->GetSkinWeightBuffer(0);
    const auto* Indices = LOD.MultiSizeIndexContainer.GetIndexBuffer();
    if (!Weights || !Indices || LOD.StaticVertexBuffers.StaticMeshVertexBuffer.GetNumTexCoords() != 4)
    {
        return false;
    }
    TArray<FMatrix44f> Matrices;
    Body->CacheRefToLocalMatrices(Matrices);
    TArray<FVector3f> Positions;
    USkinnedMeshComponent::ComputeSkinnedPositions(Body, Positions, Matrices, LOD, *Weights);
    const FTransform Transform = Body->GetComponentTransform();
    TArray<FVector> WorldPositions;
    WorldPositions.Reserve(Positions.Num());
    for (const FVector3f Position : Positions)
    {
        WorldPositions.Add(Transform.TransformPosition(FVector(Position)));
    }
    FVector Center = Hit.Position;
    FVector Normal = Hit.Normal.GetSafeNormal(UE_SMALL_NUMBER, -Hit.Direction);
    float Best = TNumericLimits<float>::Max();
    int32 Nearest = INDEX_NONE;
    const auto& UVBuffer = LOD.StaticVertexBuffers.StaticMeshVertexBuffer;
    const auto IsRemoved = [this, &UVBuffer](uint32 Vertex)
    {
        const FVector2f Tag = UVBuffer.GetVertexUV(Vertex, 3);
        const int32 Region = FMath::RoundToInt(Tag.X);
        const int32 Tier = FMath::RoundToInt(Tag.Y);
        return SelectedTiers.IsValidIndex(Region) && SelectedTiers[Region] > 0
            && Tier >= SelectedTiers[Region];
    };
    for (int32 Index = 0; Index + 2 < Indices->Num(); Index += 3)
    {
        const uint32 A = Indices->Get(Index);
        const uint32 B = Indices->Get(Index + 1);
        const uint32 C = Indices->Get(Index + 2);
        if (IsRemoved(A))
        {
            continue;
        }
        const FVector2f AtlasA = UVBuffer.GetVertexUV(A, 2);
        const FVector2f AtlasB = UVBuffer.GetVertexUV(B, 2);
        const FVector2f AtlasC = UVBuffer.GetVertexUV(C, 2);
        if (FMath::Abs((AtlasB.X - AtlasA.X) * (AtlasC.Y - AtlasA.Y) - (AtlasB.Y - AtlasA.Y) * (AtlasC.X - AtlasA.X)) < 1.e-12f)
        {
            continue;
        }
        const FVector Face = FVector::CrossProduct(WorldPositions[B] - WorldPositions[A], WorldPositions[C] - WorldPositions[A]).GetSafeNormal();
        if (FVector::DotProduct(Face, Normal) < 0.1f)
        {
            continue;
        }
        const FVector Point = FMath::ClosestPointOnTriangleToPoint(Hit.Position, WorldPositions[A], WorldPositions[B], WorldPositions[C]);
        const float Distance = FVector::DistSquared(Point, Hit.Position);
        if (Distance < Best)
        {
            Best = Distance;
            Center = Point;
            Nearest = Index;
        }
    }
    LastProjectionError = FMath::Sqrt(Best);
    if (Nearest == INDEX_NONE || Best > FMath::Square(12.0f))
    {
        return false;
    }
    Normal = FVector::CrossProduct(WorldPositions[Indices->Get(Nearest + 1)] - WorldPositions[Indices->Get(Nearest)],
        WorldPositions[Indices->Get(Nearest + 2)] - WorldPositions[Indices->Get(Nearest)]).GetSafeNormal();
    const FVector Axis = FVector::CrossProduct(Normal, FMath::Abs(Normal.Z) < 0.9f ? FVector::UpVector : FVector::RightVector).GetSafeNormal();
    const FVector Side = FVector::CrossProduct(Normal, Axis);
    FRandomStream Random(Hit.Serial * 7919u + 17u);
    const float Phase = Random.FRandRange(-PI, PI);
    const float Stretch = Random.FRandRange(0.85f, 1.15f);
    const float HitRadius = FMath::Clamp(Radius * FMath::Sqrt(FMath::Clamp(Strength, 0.1f, 4.0f)), 0.2f, 10.0f);
    const auto* Target = CastChecked<ABBBZombieHitReactionTarget>(GetOwner());
    const bool bOpenHeavy = Hit.Region == EBBBMonsterHitRegion::Torso && !HeavyWound && Resources->HeavyWound
        && Target->GoreDebug->DamageRatios[0] + Target->GoreDebug->DamageStep >= 0.6f;
    const float Extent = HitRadius * 3.0f;
    const int32 Size = Resources->AtlasSize;
    bool bWritten = false;
    for (int32 Index = 0; Index + 2 < Indices->Num(); Index += 3)
    {
        const uint32 A = Indices->Get(Index);
        const uint32 B = Indices->Get(Index + 1);
        const uint32 C = Indices->Get(Index + 2);
        if (IsRemoved(A))
        {
            continue;
        }
        const FVector PA = WorldPositions[A];
        const FVector PB = WorldPositions[B];
        const FVector PC = WorldPositions[C];
        const FVector Face = FVector::CrossProduct(PB - PA, PC - PA).GetSafeNormal();
        if (FVector::DotProduct(Face, Normal) < 0.35f
            || FVector::DistSquared(FMath::ClosestPointOnTriangleToPoint(Center, PA, PB, PC), Center) > FMath::Square(Extent))
        {
            continue;
        }
        const FVector2D UA(UVBuffer.GetVertexUV(A, 2));
        const FVector2D UB(UVBuffer.GetVertexUV(B, 2));
        const FVector2D UC(UVBuffer.GetVertexUV(C, 2));
        const double Denom = (UB.Y - UC.Y) * (UA.X - UC.X) + (UC.X - UB.X) * (UA.Y - UC.Y);
        if (FMath::Abs(Denom) < 1.e-12)
        {
            continue;
        }
        const int32 MinX = FMath::Clamp(FMath::FloorToInt(FMath::Min3(UA.X, UB.X, UC.X) * Size), 0, Size - 1);
        const int32 MaxX = FMath::Clamp(FMath::CeilToInt(FMath::Max3(UA.X, UB.X, UC.X) * Size), 0, Size - 1);
        const int32 MinY = FMath::Clamp(FMath::FloorToInt(FMath::Min3(UA.Y, UB.Y, UC.Y) * Size), 0, Size - 1);
        const int32 MaxY = FMath::Clamp(FMath::CeilToInt(FMath::Max3(UA.Y, UB.Y, UC.Y) * Size), 0, Size - 1);
        for (int32 Y = MinY; Y <= MaxY; ++Y)
        {
            for (int32 X = MinX; X <= MaxX; ++X)
            {
                const FVector2D UV((X + 0.5) / Size, (Y + 0.5) / Size);
                const double W0 = ((UB.Y - UC.Y) * (UV.X - UC.X) + (UC.X - UB.X) * (UV.Y - UC.Y)) / Denom;
                const double W1 = ((UC.Y - UA.Y) * (UV.X - UC.X) + (UA.X - UC.X) * (UV.Y - UC.Y)) / Denom;
                const double W2 = 1.0 - W0 - W1;
                if (FMath::Min3(W0, W1, W2) < -0.08)
                {
                    continue;
                }
                const FVector Delta = PA * W0 + PB * W1 + PC * W2 - Center;
                if (FMath::Abs(FVector::DotProduct(Delta, Normal)) > FMath::Max(0.7f, HitRadius * 0.6f))
                {
                    continue;
                }
                const float U = FVector::DotProduct(Delta, Axis) / Stretch;
                const float V = FVector::DotProduct(Delta, Side) * Stretch;
                const float Angle = FMath::Atan2(V, U) + Phase;
                const float Edge = 1.0f + 0.12f * FMath::Sin(Angle * 3) + 0.07f * FMath::Sin(Angle * 7 + 2)
                    + 0.035f * FMath::Sin(Angle * 11 - 1);
                const float Distance = FMath::Sqrt(U * U + V * V) / (HitRadius * Edge);
                FColor& Pixel = Pixels[Y * Size + X];
                const float Growth = 1.0f + Pixel.R / 255.0f * 0.5f;
                const float Blood = FMath::Clamp((2.4f * Growth - Distance) / 1.6f, 0.0f, 1.0f);
                const float Tissue = FMath::Clamp((0.9f * Growth - Distance) * 5.0f, 0.0f, 1.0f);
                if (Blood <= 0)
                {
                    continue;
                }
                Pixel.R = FMath::Min(255, Pixel.R + FMath::RoundToInt(Blood * Accumulation * 255));
                Pixel.G = FMath::Min(255, Pixel.G + FMath::RoundToInt(Tissue * Accumulation * 255));
                if (bOpenHeavy && Distance < 0.6f)
                {
                    Pixel.B = 255;
                }
                bWritten = true;
            }
        }
    }
    bMaskDirty |= bWritten;
    if (bWritten && bOpenHeavy)
    {
        HeavyTriangle = FIntVector(Indices->Get(Nearest), Indices->Get(Nearest + 1), Indices->Get(Nearest + 2));
        HeavyBarycentric = FMath::ComputeBaryCentric2D(Center, WorldPositions[HeavyTriangle.X],
            WorldPositions[HeavyTriangle.Y], WorldPositions[HeavyTriangle.Z]);
        HeavyRadius = HitRadius;
        HeavyWound = NewObject<UStaticMeshComponent>(GetOwner());
        HeavyWound->SetStaticMesh(Resources->HeavyWound);
        HeavyWound->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        HeavyWound->SetCanEverAffectNavigation(false);
        HeavyWound->PrimaryComponentTick.bCanEverTick = false;
        HeavyWound->RegisterComponent();
        DetailMaterials.Add(HeavyWound->CreateDynamicMaterialInstance(0));
        UpdateHeavyWound();
    }
    return bWritten;
}

void UBBBZombieSurfaceWoundComponent::UpdateHeavyWound()
{
    if (!HeavyWound || !Body)
    {
        return;
    }
    const auto* Data = Body->GetSkeletalMeshAsset()->GetResourceForRendering();
    auto* Weights = Body->GetSkinWeightBuffer(0);
    if (!Data || !Weights)
    {
        return;
    }
    TArray<FMatrix44f> Matrices;
    Body->CacheRefToLocalMatrices(Matrices);
    const auto& LOD = Data->LODRenderData[0];
    const FVector A(USkinnedMeshComponent::GetSkinnedVertexPosition(Body, HeavyTriangle.X, LOD, *Weights, Matrices));
    const FVector B(USkinnedMeshComponent::GetSkinnedVertexPosition(Body, HeavyTriangle.Y, LOD, *Weights, Matrices));
    const FVector C(USkinnedMeshComponent::GetSkinnedVertexPosition(Body, HeavyTriangle.Z, LOD, *Weights, Matrices));
    const FTransform Transform = Body->GetComponentTransform();
    const FVector Position = Transform.TransformPosition(A * HeavyBarycentric.X + B * HeavyBarycentric.Y + C * HeavyBarycentric.Z);
    const FVector Normal = Transform.TransformVectorNoScale(FVector::CrossProduct(B - A, C - A).GetSafeNormal());
    HeavyWound->SetWorldTransform(FTransform(FRotationMatrix::MakeFromZ(Normal).ToQuat(), Position,
        FVector(HeavyRadius * Transform.GetScale3D().GetMin())));
}

bool UBBBZombieSurfaceWoundComponent::Sever(const EBBBMonsterHitRegion Region,
    const FBBBMonsterHitReactionFragment& Hit, const int32 TierOverride)
{
    const int32 R = static_cast<int32>(Region);
    if (!Resources || !Body || R < 1 || R > 5 || SelectedTiers[R] != 0)
    {
        return false;
    }
    int32 Tier = 1;
    const FName Joint[] = {NAME_None, TEXT("head"), TEXT("lowerarm_l"), TEXT("lowerarm_r"), TEXT("calf_l"), TEXT("calf_r")};
    const FName End[] = {NAME_None, TEXT("head"), TEXT("hand_l"), TEXT("hand_r"), TEXT("foot_l"), TEXT("foot_r")};
    if (R > 1)
    {
        const FVector J = Body->GetSocketLocation(Joint[R]);
        const FVector E = Body->GetSocketLocation(End[R]);
        Tier = FVector::DotProduct(Hit.Position - J, (E - J).GetSafeNormal()) >= -3.0f ? 2 : 1;
    }
    if (TierOverride == 1 || (R > 1 && TierOverride == 2))
    {
        Tier = TierOverride;
    }
    const auto* Cut = Resources->Cuts.FindByPredicate([Region, Tier](const auto& Item)
    {
        return Item.Region == Region && Item.Tier == Tier;
    });
    if (!Cut || !Cut->Cap || !Cut->Piece)
    {
        return false;
    }
    auto* Cap = NewObject<USkeletalMeshComponent>(GetOwner());
    Cap->SetSkeletalMesh(Cut->Cap);
    Cap->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Cap->SetCanEverAffectNavigation(false);
    Cap->SetupAttachment(Body);
    Cap->RegisterComponent();
    Cap->SetLeaderPoseComponent(Body);
    for (int32 Index = 0; Index < Cap->GetNumMaterials(); ++Index)
    {
        DetailMaterials.Add(Cap->CreateDynamicMaterialInstance(Index));
    }
    Cap->PrimaryComponentTick.bCanEverTick = false;
    Caps.Add(Cap);
    SelectedTiers[R] = Tier;
    Body->SetCustomPrimitiveDataFloat(R + 1, Tier);
    if (Body->IsPhysicsStateCreated())
    {
        Body->SetAllBodiesBelowPhysicsDisabled(Cut->Bone, true);
    }
    while (Pieces.Num() >= FMath::Clamp(MaximumPieces, 5, 24))
    {
        Pieces[0]->DestroyComponent();
        Pieces.RemoveAt(0);
        PieceAges.RemoveAt(0);
    }
    auto* Piece = NewObject<USkeletalMeshComponent>(GetOwner());
    Piece->SetSkeletalMesh(Cut->Piece);
    Piece->SetWorldTransform(Body->GetComponentTransform());
    Piece->SetCollisionObjectType(ECC_PhysicsBody);
    Piece->SetCollisionResponseToAllChannels(ECR_Ignore);
    Piece->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    Piece->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
    Piece->SetCanEverAffectNavigation(false);
    Piece->SetGenerateOverlapEvents(false);
    Piece->SetLinearDamping(0.35f);
    Piece->SetAngularDamping(1.5f);
    Piece->bPauseAnims = false;
    Piece->RegisterComponent();
    Piece->SetAnimInstanceClass(UBBBZombieGibPoseAnimInstance::StaticClass());
    if (auto* Animation = Cast<UBBBZombieGibPoseAnimInstance>(Piece->GetAnimInstance()))
    {
        Animation->Capture(*Body);
    }
    Piece->TickAnimation(0.0f, false);
    Piece->RefreshBoneTransforms();
    const auto& SourceSlots = Body->GetSkeletalMeshAsset()->GetMaterials();
    const auto& PieceSlots = Cut->Piece->GetMaterials();
    for (int32 Index = 0; Index < PieceSlots.Num(); ++Index)
    {
        const FName Name = PieceSlots[Index].MaterialSlotName;
        const int32 SourceIndex = SourceSlots.IndexOfByPredicate([Name](const auto& Slot)
        {
            return Slot.MaterialSlotName == Name;
        });
        if (SourceIndex != INDEX_NONE)
        {
            Piece->SetMaterial(Index, Body->GetMaterial(SourceIndex));
        }
        if (SourceIndex == INDEX_NONE)
        {
            DetailMaterials.Add(Piece->CreateDynamicMaterialInstance(Index));
        }
    }
    Piece->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
    Piece->SetEnableGravity(true);
    Piece->SetAllBodiesSimulatePhysics(true);
    Piece->SetAllBodiesPhysicsBlendWeight(1.0f);
    Piece->SetAllPhysicsLinearVelocity(Hit.Direction.GetSafeNormal() * FMath::Clamp(LaunchSpeed, 0.0f, 1800.0f)
        + FVector::UpVector * FMath::Clamp(LiftSpeed, 0.0f, 800.0f));
    Piece->SetAllPhysicsAngularVelocityInDegrees(FVector::RightVector * FMath::Clamp(SpinSpeed, 0.0f, 1200.0f));
    Piece->WakeAllRigidBodies();
    Pieces.Add(Piece);
    PieceAges.Add(0);
    DripRemaining[R] = FMath::Clamp(DripDuration, 0.0f, 10.0f);
    return true;
}

void UBBBZombieSurfaceWoundComponent::Advance(const float DeltaSeconds)
{
    UploadMask();
    UpdateHeavyWound();
    for (int32 Index = Pieces.Num() - 1; Index >= 0; --Index)
    {
        PieceAges[Index] += DeltaSeconds;
        if (PieceAges[Index] >= FMath::Clamp(PieceLifetime, 1.0f, 120.0f))
        {
            Pieces[Index]->DestroyComponent();
            Pieces.RemoveAt(Index);
            PieceAges.RemoveAt(Index);
        }
    }
    DripClock += DeltaSeconds;
    if (DripClock < 0.2f || !Body)
    {
        return;
    }
    const float Step = DripClock;
    DripClock = 0;
    const auto* Target = Cast<ABBBZombieHitReactionTarget>(GetOwner());
    if (!Target || !Target->GoreDebug->BloodSettings)
    {
        return;
    }
    TArray<FBBBMonsterBloodImpact> Impacts;
    for (int32 R = 1; R < 6; ++R)
    {
        if (DripRemaining[R] <= 0)
        {
            continue;
        }
        DripRemaining[R] = FMath::Max(0.0f, DripRemaining[R] - Step);
        const auto* Cut = Resources->Cuts.FindByPredicate([this, R](const auto& Item)
        {
            return static_cast<int32>(Item.Region) == R && Item.Tier == SelectedTiers[R];
        });
        if (Cut)
        {
            FBBBMonsterBloodImpact Impact;
            Impact.Position = Body->GetSocketTransform(Cut->Bone).TransformPosition(Cut->LocalCenter);
            Impact.Direction = -FVector::UpVector;
            Impact.Normal = FVector::UpVector;
            Impact.Region = Cut->Region;
            Impact.Seed = GetTypeHash(DripRemaining[R]) ^ R;
            Impact.Source = GetOwner();
            Impact.bDripOnly = true;
            Impacts.Add(Impact);
        }
    }
    if (!Impacts.IsEmpty())
    {
        GetWorld()->GetSubsystem<UBBBMonsterBloodPresentationSubsystem>()->Publish(*Target->GoreDebug->BloodSettings, Impacts);
    }
}

void UBBBZombieSurfaceWoundComponent::ResetSurface()
{
    if (HeavyWound)
    {
        HeavyWound->DestroyComponent();
        HeavyWound = nullptr;
    }
    for (USkeletalMeshComponent* Cap : Caps)
    {
        Cap->DestroyComponent();
    }
    for (USkeletalMeshComponent* Piece : Pieces)
    {
        Piece->DestroyComponent();
    }
    Caps.Reset();
    Pieces.Reset();
    PieceAges.Reset();
    DetailMaterials.Reset();
    if (Resources && Body)
    {
        for (const auto& Cut : Resources->Cuts)
        {
            if (Body->IsPhysicsStateCreated())
            {
                Body->SetAllBodiesBelowPhysicsDisabled(Cut.Bone, false);
            }
        }
        for (int32 R = 1; R < 6; ++R)
        {
            Body->SetCustomPrimitiveDataFloat(R + 1, 0);
        }
    }
    SelectedTiers.Init(0, 6);
    DripRemaining.Init(0, 6);
    DripClock = 0;
    for (FColor& Pixel : Pixels)
    {
        Pixel = FColor(0, 0, 0, 255);
    }
    bMaskDirty = true;
    UploadMask();
}

void UBBBZombieSurfaceWoundComponent::SetStyle(const FLinearColor Blood, const FLinearColor Tissue, const float Scale,
    const float Roughness, const float Specular, const bool bVisible)
{
    if (HeavyWound)
    {
        HeavyWound->SetVisibility(bVisible);
    }
    const auto Apply = [Blood, Tissue, Scale, Roughness, Specular, bVisible](UMaterialInstanceDynamic* Material)
    {
        if (!Material)
        {
            return;
        }
        Material->SetVectorParameterValue(TEXT("TargetBloodTint"), Blood);
        Material->SetVectorParameterValue(TEXT("TargetTissueTint"), Tissue);
        Material->SetScalarParameterValue(TEXT("TargetWoundVisible"), bVisible ? 1.0f : 0.0f);
        Material->SetScalarParameterValue(TEXT("TargetBloodStrength"), FMath::Clamp(Scale, 0.0f, 3.0f));
        Material->SetScalarParameterValue(TEXT("TargetTissueRoughness"), FMath::Clamp(Roughness, 0.0f, 1.0f));
        Material->SetScalarParameterValue(TEXT("TargetTissueSpecular"), FMath::Clamp(Specular, 0.0f, 1.0f));
    };
    for (UMaterialInstanceDynamic* Material : Materials)
    {
        Apply(Material);
    }
    for (UMaterialInstanceDynamic* Material : DetailMaterials)
    {
        Apply(Material);
    }
}

void UBBBZombieSurfaceWoundComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    ResetSurface();
    Materials.Reset();
    Pixels.Reset();
    Mask = nullptr;
    Resources = nullptr;
    Body = nullptr;
    Super::EndPlay(EndPlayReason);
}
