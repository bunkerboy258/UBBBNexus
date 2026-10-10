#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "UObject/Script.h"
#include "UObject/Package.h"
#include "UObject/UObjectIterator.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "AssetCompilingManager.h"
#include "Rendering/SkeletalMeshModel.h"
#include "MeshDescription.h"
#include "StaticMeshAttributes.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionCustom.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/PlayerController.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitReactionFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationActor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterSeveringPresentationComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterGibPresentationSubsystem.h"

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
        World->GetOutermost()->SetDirtyFlag(false);
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
        TestNotNull(TEXT("共享蒙皮伤口完整"), Definition->WoundGeometry.Get());
        if (Definition->WoundGeometry)
        {
            const auto* WoundModel = Definition->WoundGeometry->GetImportedModel();
            if (!TestNotNull(TEXT("共享伤口保留可核验模型"), WoundModel)
                || !TestEqual(TEXT("共享伤口只有一份精简几何"), WoundModel->LODModels.Num(), 1))
            {
                return false;
            }
            TestEqual(TEXT("共享伤口只有一个表面材质槽"), Definition->WoundGeometry->GetMaterials().Num(), 1);
            const auto* CutGeometry = Definition->WoundGeometry->GetMeshDescription(0);
            if (!TestNotNull(TEXT("断口保留可核验拓扑"), CutGeometry))
            {
                return false;
            }
            const auto CutTags = FStaticMeshConstAttributes(*CutGeometry).GetVertexInstanceUVs();
            for (const auto Instance : CutGeometry->VertexInstances().GetElementIDs())
            {
                TestTrue(TEXT("断口几何不包含躯干穿孔"), CutTags.Get(Instance, 3).X > 0.5f);
            }
            for (const auto& Section : WoundModel->LODModels[0].Sections)
            {
                TestEqual(TEXT("所有断口和创口使用同一真实材质槽"), Section.MaterialIndex, uint16(0));
            }
        }
        TestEqual(TEXT("受损材质覆盖完整身体槽位"), Definition->WoundMaterials.Num(), Mesh->GetNumMaterials());
        for (const auto& Material : Definition->WoundMaterials)
        {
            const auto* Base = Material ? Material->GetMaterial() : nullptr;
            if (!TestNotNull(TEXT("受损材质主表面"), Base))
            {
                return false;
            }
            TestTrue(TEXT("裁剪使用遮罩混合而非不透明模式"), Base->BlendMode == BLEND_Masked);
            TestTrue(TEXT("主表面运行时有效混合模式允许裁剪"), Base->GetBlendMode() == BLEND_Masked);
            TestTrue(TEXT("材质实例运行时有效混合模式允许裁剪"), Material->GetBlendMode() == BLEND_Masked);
            TestNotNull(TEXT("断肢遮罩连接真实表达式"), Base->GetEditorOnlyData()->OpacityMask.Expression);
            TestFalse(TEXT("断肢遮罩禁止常量忽略表达式"), bool(Base->GetEditorOnlyData()->OpacityMask.UseConstant));
            for (const auto& Expression : Base->GetExpressions())
            {
                const auto* Custom = Cast<UMaterialExpressionCustom>(Expression.Get());
                if (Custom && Custom->Code.Contains(TEXT("HoleUV")))
                {
                    TestFalse(TEXT("胸腹浅创口不得改变身体遮罩"), Custom->Code.Contains(TEXT("a*=")));
                    TestTrue(TEXT("胸腹浅创口保留组织色"), Custom->Code.Contains(TEXT("float wound=0")));
                }
            }
        }
        const auto* BodyGeometry = Mesh->GetSkeletalMeshAsset()->GetMeshDescription(0);
        if (!TestNotNull(TEXT("完整身体保留可核验源拓扑"), BodyGeometry))
        {
            return false;
        }
        TestEqual(TEXT("伤口标记不超过骨骼渲染支持的四组UV"),
            FStaticMeshConstAttributes(*BodyGeometry).GetVertexInstanceUVs().GetNumChannels(), 4);
        uint8 TaggedRegions = 0;
        uint8 TaggedSites = 0;
        const auto BodyUVs = FStaticMeshConstAttributes(*BodyGeometry).GetVertexInstanceUVs();
        for (const auto Instance : BodyGeometry->VertexInstances().GetElementIDs())
        {
            const FVector2f Tag = BodyUVs.Get(Instance, 3);
            const int32 Region = FMath::RoundToInt(Tag.X);
            const int32 Site = FMath::RoundToInt(Tag.Y);
            TestTrue(TEXT("身体材质部位标记在六部位范围内"), Region >= 0 && Region < 6);
            if (Region >= 0 && Region < 6)
            {
                TaggedRegions |= 1u << Region;
            }
            if (Site == 6 || Site == 7)
            {
                TaggedSites |= 1u << (Site - 6);
            }
        }
        TestEqual(TEXT("身体包含躯干与五处断开部位标记"), TaggedRegions, uint8(63));
        TestEqual(TEXT("浅创口仅使用胸腹两个固定位置"), TaggedSites, uint8(3));
        FBBBMonsterHealthFragment Health;
        FBBBMonsterHitReactionFragment Hit;
        const TArray<UMaterialInterface*> Intact = Mesh->GetMaterials();
        Severing->ApplyWoundFacts(Health, *Definition, Hit, 17u, true);
        TArray<USkeletalMeshComponent*> IntactComponents;
        Actor->GetComponents(IntactComponents);
        TestEqual(TEXT("完好外观不分配额外蒙皮载体"), IntactComponents.Num(), 1);
        Health.PartDamageRatios.Add(EBBBMonsterHitRegion::Torso, 1.0);
        Health.DestroyedParts = 1u << static_cast<uint8>(EBBBMonsterHitRegion::Torso);
        Severing->ApplyWoundFacts(Health, *Definition, Hit, 17u, true);
        IntactComponents.Reset();
        Actor->GetComponents(IntactComponents);
        TestEqual(TEXT("致死躯干伤口只改变表面 不分配蒙皮断口"), IntactComponents.Num(), 1);
        Severing->ResetPresentation();
        Health = FBBBMonsterHealthFragment();
        Health.PartDamageRatios.Add(EBBBMonsterHitRegion::LeftArm, 0.3);
        Severing->ApplyWoundFacts(Health, *Definition, Hit, 17u, true);
        IntactComponents.Reset();
        Actor->GetComponents(IntactComponents);
        TestEqual(TEXT("四肢浅伤只改变共享表面 不分配全身伤口载体"), IntactComponents.Num(), 1);
        Severing->ResetPresentation();
        Health = FBBBMonsterHealthFragment();
        if (Mesh->GetSkeletalMeshAsset()->GetLODNum() >= 3)
        {
            Mesh->SetForcedLOD(3);
            Mesh->UpdateLODStatus();
            TestEqual(TEXT("远距预算检查使用真实身体LOD2"), Mesh->GetPredictedLODLevel(), 2);
            Health.PartDamageRatios.Add(EBBBMonsterHitRegion::Torso, 0.3);
            Health.PartDamageRatios.Add(EBBBMonsterHitRegion::LeftArm, 1.0);
            Health.DestroyedParts = 1u << static_cast<uint8>(EBBBMonsterHitRegion::LeftArm);
            Severing->ApplyWoundFacts(Health, *Definition, Hit, 17u, true);
            IntactComponents.Reset();
            Actor->GetComponents(IntactComponents);
            TestEqual(TEXT("首次远距创口和断肢不分配蒙皮伤口"), IntactComponents.Num(), 1);
            TestEqual(TEXT("远距断肢仍还原缺肢表面参数"),
                Mesh->GetCustomPrimitiveData().Data[static_cast<int32>(EBBBMonsterHitRegion::LeftArm) + 1], 1.0f);
            Mesh->SetForcedLOD(1);
            Mesh->UpdateLODStatus();
            Severing->ApplyWoundFacts(Health, *Definition, Hit, 17u, false);
            IntactComponents.Reset();
            Actor->GetComponents(IntactComponents);
            TestEqual(TEXT("返回近距只分配一个当前伤口载体"), IntactComponents.Num(), 2);
            for (auto* Component : IntactComponents)
            {
                if (Component != Mesh)
                {
                    TestTrue(TEXT("近距当前伤口可见"), Component->IsVisible());
                    TestFalse(TEXT("小型创口不重复投射身体阴影"), Component->CastShadow);
                    TestFalse(TEXT("小型创口不重复身体深度预通道"), Component->bRenderInDepthPass);
                    TestFalse(TEXT("小型创口不建立独立光追对象"), Component->bVisibleInRayTracing);
                    TestFalse(TEXT("创口不叠加环境贴花"), Component->bReceivesDecals);
                    TestFalse(TEXT("创口直接使用最终骨骼而不计算动画曲线"), Component->GetAllowedAnimCurveEvaluate());
                    TestTrue(TEXT("创口不再执行身体后处理蓝图"), Component->GetDisablePostProcessBlueprint());
                    TestEqual(TEXT("近距伤口恢复当前躯干伤害阶段"), Component->GetCustomPrimitiveData().Data[1], 0.33f);
                    TestEqual(TEXT("近距伤口恢复当前断肢阶段"),
                        Component->GetCustomPrimitiveData().Data[static_cast<int32>(EBBBMonsterHitRegion::LeftArm) + 1], 1.0f);
                }
            }
            Mesh->SetForcedLOD(3);
            Mesh->UpdateLODStatus();
            Severing->ApplyWoundFacts(Health, *Definition, Hit, 17u, false);
            for (auto* Component : IntactComponents)
            {
                if (Component != Mesh)
                {
                    TestFalse(TEXT("再次远离隐藏已分配伤口"), Component->IsVisible());
                }
            }
            Mesh->SetForcedLOD(1);
            Mesh->UpdateLODStatus();
            Severing->ApplyWoundFacts(Health, *Definition, Hit, 17u, false);
            for (auto* Component : IntactComponents)
            {
                if (Component != Mesh)
                {
                    TestTrue(TEXT("未改变快照返回近距恢复同一伤口载体"), Component->IsVisible());
                    TestEqual(TEXT("未改变快照保留躯干伤口阶段"), Component->GetCustomPrimitiveData().Data[1], 0.33f);
                }
            }
            Severing->ResetPresentation();
            Health = FBBBMonsterHealthFragment();
        }
        const double DamageRatios[] = {0.19, 0.2, 0.59, 0.6, 0.99, 1.0};
        const float ExpectedStages[] = {0.0f, 0.33f, 0.33f, 0.66f, 0.66f, 1.0f};
        for (int32 StageIndex = 0; StageIndex < UE_ARRAY_COUNT(DamageRatios); ++StageIndex)
        {
            Health.PartDamageRatios = FBBBMonsterPartDamage();
            Health.PartDamageRatios.Add(EBBBMonsterHitRegion::Torso, DamageRatios[StageIndex]);
            Severing->ApplyWoundFacts(Health, *Definition, Hit, 17u, true);
            const auto& Data = Mesh->GetCustomPrimitiveData().Data;
            const float ActualStage = Data.IsValidIndex(1) ? Data[1] : 0.0f;
            TestEqual(TEXT("伤口阶段严格由当前累计部位伤害推导"), ActualStage, ExpectedStages[StageIndex]);
        }
        Severing->ResetPresentation();
        Health = FBBBMonsterHealthFragment();
        uint8 Mask = 0;
        for (const auto& Part : Definition->SeveredParts)
        {
            if (!TestNotNull(TEXT("断开部件"), Part.DetachedMesh.Get()) || !TestNotNull(TEXT("身体封口"), Part.CapMesh.Get()))
            {
                return false;
            }
            TestTrue(TEXT("部件不是空几何"), !Part.DetachedMesh->GetBoundingBox().GetSize().IsNearlyZero());
            TestFalse(TEXT("部件包围盒没有无效数值"), Part.DetachedMesh->GetBounds().ContainsNaN());
            TestTrue(Names[Index] + TEXT(" ") + Part.Bone.ToString() + TEXT("脱落部件没有未封口几何边"), HasClosedPartGeometry(*Part.DetachedMesh));
            TestTrue(TEXT("封口不是空几何"), !Part.CapMesh->GetBoundingBox().GetSize().IsNearlyZero());
            TestFalse(TEXT("封口包围盒没有无效数值"), Part.CapMesh->GetBounds().ContainsNaN());
            TestNotNull(TEXT("近距离姿态部件完整"), Part.PoseMesh.Get());
            if (Part.PoseMesh)
            {
                const auto* PoseModel = Part.PoseMesh->GetImportedModel();
                if (!TestNotNull(TEXT("姿态部件保留可核验模型"), PoseModel)
                    || !TestEqual(TEXT("姿态部件不保留全身额外LOD"), PoseModel->LODModels.Num(), 1))
                {
                    return false;
                }
                for (const auto& Section : PoseModel->LODModels[0].Sections)
                {
                    TestTrue(TEXT("姿态部件每段都有真实材质槽"), Section.MaterialIndex < Part.PoseMesh->GetMaterials().Num());
                }
                TestEqual(TEXT("一次姿态复制保留完整骨骼对应"), Part.PoseMesh->GetRefSkeleton().GetNum(),
                    Mesh->GetSkeletalMeshAsset()->GetRefSkeleton().GetNum());
                TestEqual(TEXT("姿态部件保持断开骨骼索引"), Part.PoseMesh->GetRefSkeleton().FindBoneIndex(Part.Bone),
                    Mesh->GetSkeletalMeshAsset()->GetRefSkeleton().FindBoneIndex(Part.Bone));
            }
            Mask |= 1u << static_cast<uint8>(Part.Region);
            Health.DestroyedParts = Mask;
            Health.PartDamageRatios.Add(Part.Region, 1.0);
            Severing->ApplyWoundFacts(Health, *Definition, Hit, 17u, true);
            TestEqual(TEXT("损毁按材质事实裁剪而不压缩骨骼"), Mesh->GetCustomPrimitiveData().Data[static_cast<int32>(Part.Region) + 1], 1.0f);
            TestFalse(TEXT("完整骨架可继续用于蒙皮和布娃娃"), Mesh->IsBoneHiddenByName(Part.Bone));
        }
        TArray<USkeletalMeshComponent*> Wounds;
        Actor->GetComponents(Wounds);
        TestEqual(TEXT("五个断口共用一个蒙皮载体"), Wounds.Num(), 2);
        Severing->ApplyWoundFacts(Health, *Definition, Hit, 17u, true);
        Wounds.Reset();
        Actor->GetComponents(Wounds);
        TestEqual(TEXT("同一快照不增加载体"), Wounds.Num(), 2);
        Severing->ResetPresentation();
        Severing->ApplyWoundFacts(FBBBMonsterHealthFragment(), *Definition, Hit, 29u, true);
        Wounds.Reset();
        Actor->GetComponents(Wounds);
        TestEqual(TEXT("新一代完好实例复用唯一伤口载体"), Wounds.Num(), 2);
        for (auto* Component : Wounds)
        {
            if (Component != Mesh)
            {
                TestFalse(TEXT("新一代完好实例不显示上一代伤口"), Component->IsVisible());
            }
        }
        for (int32 Slot = 0; Slot < Intact.Num(); ++Slot)
        {
            TestEqual(TEXT("复用恢复普通不透明材质"), Mesh->GetMaterial(Slot), Intact[Slot]);
        }
        for (const auto& Part : Definition->SeveredParts)
        {
            TestFalse(TEXT("复用恢复完整外观"), Mesh->IsBoneHiddenByName(Part.Bone));
            TestEqual(TEXT("复用清除上一代损毁参数"), Mesh->GetCustomPrimitiveData().Data[static_cast<int32>(Part.Region) + 1], 0.0f);
        }
        Actor->Destroy();
    }
    return true;
}

/** 验证碎片模拟预算和独立于原载体的复用 不作为实际画面验收 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterGibPoolTest, "UBBB.Mass.ZombieGibPool",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterGibPoolTest::RunTest(const FString&)
{
    FEditorScriptExecutionGuard ScriptGuard;
    const auto Initialization = UWorld::InitializationValues().AllowAudioPlayback(false)
        .CreatePhysicsScene(true).CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(false);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("碎片池隔离世界"), World))
    {
        return false;
    }
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
        World->GetOutermost()->SetDirtyFlag(false);
    };
    const FString Root = TEXT("/Game/_Project/System/Mass/Monster/Zombie/Male/Variants/Connor/");
    UClass* Class = LoadClass<ABBBMonsterPresentationActor>(nullptr,
        *(Root + TEXT("BP_BBBZombieConnorPresentation.BP_BBBZombieConnorPresentation_C")));
    auto* Definition = LoadObject<UBBBMonsterDefinition>(nullptr,
        *(Root + TEXT("DA_BBBZombieConnorDefinition.DA_BBBZombieConnorDefinition")));
    if (!TestNotNull(TEXT("碎片正式载体类"), Class) || !TestNotNull(TEXT("碎片正式配置"), Definition))
    {
        return false;
    }
    FAssetCompilingManager::Get().FinishAllCompilation();
    auto* Actor = World->SpawnActor<ABBBMonsterPresentationActor>(Class);
    auto* Controller = World->SpawnActor<APlayerController>();
    auto* Pool = World->GetSubsystem<UBBBMonsterGibPresentationSubsystem>();
    if (!TestNotNull(TEXT("碎片来源"), Actor) || !TestNotNull(TEXT("近处观察者"), Controller)
        || !TestNotNull(TEXT("世界独立碎片池"), Pool))
    {
        return false;
    }
    auto* Mesh = Actor->GetMonsterMesh();
    Actor->SetActorHiddenInGame(false);
    Mesh->SetHiddenInGame(false);
    Mesh->SetForcedLOD(1);
    Mesh->UpdateLODStatus();
    Mesh->SetLastRenderTime(World->GetTimeSeconds());
    TestTrue(TEXT("碎片预算来源通过最近渲染门槛"), Mesh->WasRecentlyRendered(0.2f));
    TestTrue(TEXT("碎片预算来源使用近距模型"), Mesh->GetPredictedLODLevel() <= 1);
    if (!TestEqual(TEXT("碎片正式配置有五个固定部位"), Definition->SeveredParts.Num(), 5))
    {
        return false;
    }
    const auto& Part = Definition->SeveredParts[0];
    if (!TestNotNull(TEXT("正式静态部件"), Part.DetachedMesh.Get()) || !TestNotNull(TEXT("正式姿态部件"), Part.PoseMesh.Get()))
    {
        return false;
    }
    TestFalse(TEXT("普通隔离控制器不是本机观察者"), Controller->IsLocalController());
    TestFalse(TEXT("没有本机观察者不分配世界部件"), Pool->Emit(*Mesh, *Part.DetachedMesh,
        *Part.PoseMesh, Part.Bone, FVector::ForwardVector, nullptr));
    TestEqual(TEXT("没有本机观察者不扩大池容量"), Pool->GetAllocatedCount(), 0);
    Controller->SetAsLocalPlayerController();
    World->AddController(Controller);
    TestTrue(TEXT("碎片预算测试具有本机观察者"), Controller->IsLocalController());
    TestEqual(TEXT("碎片预算观察者已注册到世界"), World->GetNumPlayerControllers(), 1);
    Mesh->SetComponentSpaceTransformsDoubleBuffering(false);
    const int32 CapturedBone = Mesh->GetBoneIndex(Part.Bone);
    Mesh->GetEditableComponentSpaceTransforms()[CapturedBone].AddToTranslation(FVector(7.0f, 3.0f, 2.0f));
    const FTransform ExpectedPose = Mesh->GetEditableComponentSpaceTransforms()[CapturedBone];
    for (int32 Index = 0; Index < 32; ++Index)
    {
        TestTrue(TEXT("模拟预算内可分配世界部件"), Pool->Emit(*Mesh, *Part.DetachedMesh,
            *Part.PoseMesh, Part.Bone, FVector::ForwardVector, nullptr));
    }
    TestEqual(TEXT("模拟碎片严格限制三十二个"), Pool->GetSimulatingCount(), 32);
    TestFalse(TEXT("超过模拟预算不额外分配组件"), Pool->Emit(*Mesh, *Part.DetachedMesh,
        *Part.PoseMesh, Part.Bone, FVector::ForwardVector, nullptr));
    TestEqual(TEXT("预算溢出不扩大池容量"), Pool->GetAllocatedCount(), 32);
    int32 CapturedPieces = 0;
    for (TObjectIterator<UPoseableMeshComponent> It; It; ++It)
    {
        if (It->GetWorld() == World && It->GetSkinnedAsset() == Part.PoseMesh)
        {
            ++CapturedPieces;
            TestTrue(TEXT("碎片捕获最终物理姿态而非动画局部姿态"),
                It->GetComponentSpaceTransforms()[CapturedBone].Equals(ExpectedPose, 0.001f));
        }
    }
    TestEqual(TEXT("最终姿态断言覆盖全部近距模拟碎片"), CapturedPieces, 32);
    Actor->GetMonsterSevering()->ResetPresentation();
    TestEqual(TEXT("来源复用不回收世界碎片"), Pool->GetSimulatingCount(), 32);
    Pool->AdvancePresentation(World->GetTimeSeconds() + 35.1f);
    TestEqual(TEXT("寿命结束停止全部模拟"), Pool->GetSimulatingCount(), 0);
    TestEqual(TEXT("寿命结束不留下可见静止碎片"), Pool->GetSettledCount(), 0);
    TestTrue(TEXT("寿命结束后复用既有槽位"), Pool->Emit(*Mesh, *Part.DetachedMesh,
        *Part.PoseMesh, Part.Bone, FVector::ForwardVector, nullptr));
    TestEqual(TEXT("复用不追加槽位"), Pool->GetAllocatedCount(), 32);
    Actor->Destroy();
    TestEqual(TEXT("来源销毁不销毁世界碎片"), Pool->GetSimulatingCount(), 1);
    return true;
}

/** 验证真实地面查询约束碎片落定 无支撑碎片不保留悬浮残留 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterGibLandingTest, "UBBB.Mass.ZombieGibLanding",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterGibLandingTest::RunTest(const FString&)
{
    FEditorScriptExecutionGuard ScriptGuard;
    const auto Initialization = UWorld::InitializationValues().AllowAudioPlayback(false)
        .CreatePhysicsScene(true).CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(false);
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("碎片地面隔离世界"), World))
    {
        return false;
    }
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
        World->GetOutermost()->SetDirtyFlag(false);
    };
    const FString Root = TEXT("/Game/_Project/System/Mass/Monster/Zombie/Male/Variants/Connor/");
    UClass* Class = LoadClass<ABBBMonsterPresentationActor>(nullptr,
        *(Root + TEXT("BP_BBBZombieConnorPresentation.BP_BBBZombieConnorPresentation_C")));
    auto* Definition = LoadObject<UBBBMonsterDefinition>(nullptr,
        *(Root + TEXT("DA_BBBZombieConnorDefinition.DA_BBBZombieConnorDefinition")));
    if (!TestNotNull(TEXT("地面验收正式载体"), Class) || !TestNotNull(TEXT("地面验收正式配置"), Definition)
        || !TestEqual(TEXT("地面验收五处部件完整"), Definition->SeveredParts.Num(), 5))
    {
        return false;
    }
    FAssetCompilingManager::Get().FinishAllCompilation();
    auto* Actor = World->SpawnActor<ABBBMonsterPresentationActor>(Class);
    auto* Controller = World->SpawnActor<APlayerController>();
    auto* FloorActor = World->SpawnActor<AActor>();
    auto* Pool = World->GetSubsystem<UBBBMonsterGibPresentationSubsystem>();
    if (!TestNotNull(TEXT("地面验收来源"), Actor) || !TestNotNull(TEXT("地面验收观察者"), Controller)
        || !TestNotNull(TEXT("地面验收支撑载体"), FloorActor) || !TestNotNull(TEXT("地面验收碎片池"), Pool))
    {
        return false;
    }
    Controller->SetAsLocalPlayerController();
    World->AddController(Controller);
    TestTrue(TEXT("地面约束测试具有本机观察者"), Controller->IsLocalController());
    TestEqual(TEXT("地面约束观察者已注册到世界"), World->GetNumPlayerControllers(), 1);
    auto* Mesh = Actor->GetMonsterMesh();
    Actor->SetActorHiddenInGame(false);
    Mesh->SetHiddenInGame(false);
    Mesh->SetForcedLOD(1);
    Mesh->UpdateLODStatus();
    const auto& Part = Definition->SeveredParts[0];
    if (!TestNotNull(TEXT("地面验收静态部件"), Part.DetachedMesh.Get())
        || !TestNotNull(TEXT("地面验收姿态部件"), Part.PoseMesh.Get()))
    {
        return false;
    }
    auto* Floor = NewObject<UBoxComponent>(FloorActor);
    FloorActor->SetRootComponent(Floor);
    Floor->SetBoxExtent(FVector(500.0f, 500.0f, 5.0f));
    Floor->SetCollisionObjectType(ECC_WorldStatic);
    Floor->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Floor->SetCollisionResponseToAllChannels(ECR_Block);
    Floor->RegisterComponent();
    FloorActor->SetActorLocation(Mesh->GetSocketLocation(Part.Bone) - FVector::UpVector * 10.0f);
    Mesh->SetLastRenderTime(World->GetTimeSeconds());
    TestTrue(TEXT("地面约束来源通过最近渲染门槛"), Mesh->WasRecentlyRendered(0.2f));
    TestTrue(TEXT("地面约束来源使用近距模型"), Mesh->GetPredictedLODLevel() <= 1);
    for (int32 Index = 0; Index < 32; ++Index)
    {
        TestTrue(TEXT("地面验收生成预算内碎片"), Pool->Emit(*Mesh, *Part.DetachedMesh,
            *Part.PoseMesh, Part.Bone, FVector::ForwardVector, nullptr));
    }
    bool bMoved = false;
    UStaticMeshComponent* UnsupportedPiece = nullptr;
    for (TObjectIterator<UStaticMeshComponent> It; It; ++It)
    {
        if (It->GetOuter() == World && It->GetStaticMesh() == Part.DetachedMesh)
        {
            It->SetWorldLocation(It->GetComponentLocation() + FVector(10000.0f, 0.0f, 0.0f), false, nullptr, ETeleportType::TeleportPhysics);
            UnsupportedPiece = *It;
            bMoved = true;
            break;
        }
    }
    TestTrue(TEXT("地面验收覆盖一个无支撑落点"), bMoved);
    TestEqual(TEXT("落地查询前全部部件已进入物理模拟"), Pool->GetSimulatingCount(), 32);
    if (UnsupportedPiece)
    {
        FHitResult Support;
        const FVector Origin = UnsupportedPiece->Bounds.Origin;
        const float Depth = FMath::Max(10.0f, UnsupportedPiece->Bounds.BoxExtent.Z + 10.0f);
        TestTrue(TEXT("无支撑碎片已经移动至测试地面外"), Origin.X > 9000.0f);
        TestFalse(TEXT("测试地面外没有独立地面查询结果"), World->LineTraceSingleByChannel(
            Support, Origin, Origin - FVector::UpVector * Depth, ECC_WorldStatic));
        UE_LOG(LogTemp, Display, TEXT("[BBBZombieGibLanding]Origin=%s Depth=%.3f Support=%s"),
            *Origin.ToString(), Depth, *GetNameSafe(Support.GetComponent()));
        UE_LOG(LogTemp, Display, TEXT("[BBBZombieGibLanding]Simulating=%d MovedSimulating=%d"),
            Pool->GetSimulatingCount(), UnsupportedPiece->IsSimulatingPhysics() ? 1 : 0);
    }
    Pool->AdvancePresentation(World->GetTimeSeconds() + 6.0f);
    TestEqual(TEXT("落定与无支撑回收后无模拟碎片"), Pool->GetSimulatingCount(), 0);
    TestEqual(TEXT("只保留三十一个有支撑碎片"), Pool->GetSettledCount(), 31);
    TestEqual(TEXT("地面约束不销毁池槽位"), Pool->GetAllocatedCount(), 32);
    TestTrue(TEXT("无支撑释放槽位可立即复用"), Pool->Emit(*Mesh, *Part.DetachedMesh,
        *Part.PoseMesh, Part.Bone, FVector::ForwardVector, nullptr));
    TestEqual(TEXT("再次生成不扩大池容量"), Pool->GetAllocatedCount(), 32);
    Actor->Destroy();
    TestEqual(TEXT("来源销毁不清除落地残留"), Pool->GetSettledCount(), 31);
    return true;
}

/** 验证男女默认生成路径直接使用完整新资源 不增加旧实现兼容 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterDefaultWoundsTest, "UBBB.Mass.ZombieDefaultWounds",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterDefaultWoundsTest::RunTest(const FString&)
{
    const FString Root = TEXT("/Game/_Project/System/Mass/Monster/Zombie/");
    for (int32 Index = 0; Index < 2; ++Index)
    {
        const FString Gender = Index == 0 ? TEXT("Male") : TEXT("Female");
        const FString Variant = Index == 0 ? TEXT("Connor") : TEXT("Kiyo");
        const FString DefaultPath = Root + Gender + TEXT("/DA_BBBZombie") + Gender + TEXT("Definition");
        const FString VariantRoot = Root + Gender + TEXT("/Variants/") + Variant + TEXT("/");
        const FString VariantPath = VariantRoot + TEXT("DA_BBBZombie") + Variant + TEXT("Definition");
        auto* Definition = LoadObject<UBBBMonsterDefinition>(nullptr, *DefaultPath);
        auto* Canonical = LoadObject<UBBBMonsterDefinition>(nullptr, *VariantPath);
        const FString ActorName = TEXT("BP_BBBZombie") + Gender + TEXT("Presentation");
        UClass* Class = LoadClass<ABBBMonsterPresentationActor>(nullptr,
            *(Root + Gender + TEXT("/") + ActorName + TEXT(".") + ActorName + TEXT("_C")));
        if (!TestNotNull(Gender + TEXT("默认配置"), Definition)
            || !TestNotNull(Gender + TEXT("标准外观配置"), Canonical)
            || !TestNotNull(Gender + TEXT("默认表现类"), Class))
        {
            return false;
        }
        const auto* Actor = Class->GetDefaultObject<ABBBMonsterPresentationActor>();
        auto* ExpectedBody = LoadObject<USkeletalMesh>(nullptr,
            *(VariantRoot + TEXT("SKM_BBBZombie") + Variant));
        TestNotNull(TEXT("标准外观身体"), ExpectedBody);
        TestEqual(TEXT("默认生成使用同一标准身体"), Actor->GetMonsterMesh()->GetSkeletalMeshAsset(), ExpectedBody);
        TestNotNull(TEXT("默认生成具有蒙皮伤口"), Definition->WoundGeometry.Get());
        TestEqual(TEXT("默认生成复用标准伤口网格"), Definition->WoundGeometry.Get(), Canonical->WoundGeometry.Get());
        if (!TestEqual(TEXT("默认生成完整受损材质槽"), Definition->WoundMaterials.Num(), Canonical->WoundMaterials.Num())
            || !TestEqual(TEXT("默认生成身体材质槽一致"), Definition->WoundMaterials.Num(), Actor->GetMonsterMesh()->GetNumMaterials())
            || !TestEqual(TEXT("默认生成五处断开完整"), Definition->SeveredParts.Num(), 5))
        {
            return false;
        }
        for (int32 Slot = 0; Slot < Definition->WoundMaterials.Num(); ++Slot)
        {
            TestEqual(TEXT("默认生成使用同一受损材质"), Definition->WoundMaterials[Slot].Get(), Canonical->WoundMaterials[Slot].Get());
        }
        for (const auto& Part : Definition->SeveredParts)
        {
            const auto* Expected = Canonical->SeveredParts.FindByPredicate([&Part](const auto& Candidate)
            {
                return Candidate.Region == Part.Region && Candidate.Bone == Part.Bone;
            });
            if (!TestNotNull(TEXT("默认断开位置对应标准部件"), Expected))
            {
                return false;
            }
            TestNotNull(TEXT("默认断开位置具有姿态部件"), Part.PoseMesh.Get());
            TestEqual(TEXT("默认断开复用静态部件"), Part.DetachedMesh.Get(), Expected->DetachedMesh.Get());
            TestEqual(TEXT("默认断开复用身体封口"), Part.CapMesh.Get(), Expected->CapMesh.Get());
            TestEqual(TEXT("默认断开复用姿态部件"), Part.PoseMesh.Get(), Expected->PoseMesh.Get());
        }
    }
    return true;
}

#endif
