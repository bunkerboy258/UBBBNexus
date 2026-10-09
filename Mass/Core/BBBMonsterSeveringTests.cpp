#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "UObject/Script.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "MeshDescription.h"
#include "StaticMeshAttributes.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationActor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterSeveringPresentationComponent.h"

namespace
{
    /**
     * @param Mesh	正式脱落部件
     * @return 是否存在完整几何且所有边均被封闭
     */
    bool HasClosedPartGeometry(UStaticMesh& Mesh)
    {
        const FMeshDescription* Description = Mesh.GetMeshDescription(0);
        if (!Description || Description->Triangles().Num() == 0)
        {
            return false;
        }
        const auto Positions = FStaticMeshConstAttributes(*Description).GetVertexPositions();
        TMap<FIntVector, int32> Vertices;
        TMap<uint64, int32> Edges;
        for (const FTriangleID Triangle : Description->Triangles().GetElementIDs())
        {
            TArray<int32> Indices;
            for (const FVertexInstanceID Instance : Description->GetTriangleVertexInstances(Triangle))
            {
                const FVector3f Position = Positions[Description->GetVertexInstanceVertex(Instance)] * 1000.0f;
                const FIntVector Key(FMath::RoundToInt(Position.X), FMath::RoundToInt(Position.Y), FMath::RoundToInt(Position.Z));
                int32* Index = Vertices.Find(Key);
                if (!Index)
                {
                    const int32 NewIndex = Vertices.Num();
                    Index = &Vertices.Add(Key, NewIndex);
                }
                Indices.Add(*Index);
            }
            for (int32 Index = 0; Index < 3; ++Index)
            {
                const int32 A = Indices[Index];
                const int32 B = Indices[(Index + 1) % 3];
                if (A == B)
                {
                    continue;
                }
                const uint64 Key = (uint64(uint32(FMath::Min(A, B))) << 32) | uint32(FMath::Max(A, B));
                ++Edges.FindOrAdd(Key);
            }
        }
        for (const auto& Edge : Edges)
        {
            if (Edge.Value < 2 || (Edge.Value & 1) != 0)
            {
                UE_LOG(LogTemp, Warning, TEXT("[BBBSealedGeometry] Mesh=%s UnpairedEdge=%llu Faces=%d"), *Mesh.GetPathName(), Edge.Key, Edge.Value);
                return false;
            }
        }
        return !Edges.IsEmpty();
    }
}

/** 验证十种正式外观的五处断肢封口和池化清理 不创建伤害事实 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterSeveringTest, "UBBB.Mass.ZombieSevering",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterSeveringTest::RunTest(const FString&)
{
    FEditorScriptExecutionGuard ScriptGuard;
    const auto Initialization = UWorld::InitializationValues().AllowAudioPlayback(false)
        .CreatePhysicsScene(true).CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(false);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("断肢隔离表现世界"), World))
    {
        return false;
    }
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
    };
    const TArray<FString> Names = {TEXT("Connor"), TEXT("Jiho"), TEXT("Michael"), TEXT("Morita"), TEXT("Renzo"), TEXT("Sakurada"),
        TEXT("Alice"), TEXT("Kiyo"), TEXT("Setsuko"), TEXT("Yuina")};
    for (int32 Index = 0; Index < Names.Num(); ++Index)
    {
        const FString Gender = Index < 6 ? TEXT("Male") : TEXT("Female");
        const FString Root = TEXT("/Game/_Project/System/Mass/Monster/Zombie/") + Gender + TEXT("/Variants/") + Names[Index] + TEXT("/");
        const FString ActorName = TEXT("BP_BBBZombie") + Names[Index] + TEXT("Presentation");
        const FString DefinitionName = TEXT("DA_BBBZombie") + Names[Index] + TEXT("Definition");
        UClass* Class = LoadClass<ABBBMonsterPresentationActor>(nullptr, *(Root + ActorName + TEXT(".") + ActorName + TEXT("_C")));
        auto* Definition = LoadObject<UBBBMonsterDefinition>(nullptr, *(Root + DefinitionName + TEXT(".") + DefinitionName));
        if (!TestNotNull(Names[Index] + TEXT("表现类"), Class) || !TestNotNull(Names[Index] + TEXT("正式定义"), Definition))
        {
            return false;
        }
        if (!TestEqual(TEXT("头 两臂 两腿封口资源完整"), Definition->SeveredParts.Num(), 5))
        {
            return false;
        }
        auto* Actor = World->SpawnActor<ABBBMonsterPresentationActor>(Class);
        if (!TestNotNull(TEXT("生成封口测试载体"), Actor))
        {
            return false;
        }
        auto* Mesh = Actor->GetMonsterMesh();
        auto* Severing = Actor->GetMonsterSevering();
        uint8 Mask = 0;
        for (const auto& Part : Definition->SeveredParts)
        {
            if (!TestNotNull(TEXT("断开部件"), Part.DetachedMesh.Get()) || !TestNotNull(TEXT("身体封口"), Part.CapMesh.Get()))
            {
                return false;
            }
            TestTrue(TEXT("部件不是空几何"), !Part.DetachedMesh->GetBoundingBox().GetSize().IsNearlyZero());
            TestTrue(Names[Index] + TEXT(" ") + Part.Bone.ToString() + TEXT("脱落部件没有未封口几何边"), HasClosedPartGeometry(*Part.DetachedMesh));
            TestTrue(TEXT("封口不是空几何"), !Part.CapMesh->GetBoundingBox().GetSize().IsNearlyZero());
            Mask |= 1u << static_cast<uint8>(Part.Region);
            Severing->ApplyDestroyedParts(Mask, Definition->SeveredParts, FVector::ForwardVector);
            TestTrue(TEXT("已损毁部位不继续显示"), Mesh->IsBoneHiddenByName(Part.Bone));
        }
        TArray<UStaticMeshComponent*> Caps;
        Actor->GetComponents(Caps);
        TestEqual(TEXT("每个断开部位只有一个封口"), Caps.Num(), 5);
        for (const auto* Cap : Caps)
        {
            TestTrue(TEXT("身体封口仍附着在活体骨架"), Cap->GetAttachParent() == Mesh);
            TestFalse(TEXT("封口不附着在已隐藏骨骼"), Mesh->IsBoneHiddenByName(Cap->GetAttachSocketName()));
            TestEqual(TEXT("封口不参与玩法碰撞"), Cap->GetCollisionEnabled(), ECollisionEnabled::NoCollision);
        }
        Severing->ApplyDestroyedParts(Mask, Definition->SeveredParts, FVector::ForwardVector);
        Caps.Reset();
        Actor->GetComponents(Caps);
        TestEqual(TEXT("同一损毁快照不重复生成部件"), Caps.Num(), 5);
        Severing->ResetPresentation();
        Caps.Reset();
        Actor->GetComponents(Caps);
        TestEqual(TEXT("复用销毁旧封口"), Caps.Num(), 0);
        for (const auto& Part : Definition->SeveredParts)
        {
            TestFalse(TEXT("复用恢复完整外观"), Mesh->IsBoneHiddenByName(Part.Bone));
        }
        Actor->Destroy();
    }
    return true;
}

#endif
