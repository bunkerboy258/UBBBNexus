#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterPresentationDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterGoreDefinition.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/Skeleton.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "Rendering/SkeletalMeshLODRenderData.h"

/** 检查真实交付资产的八区资源 骨架单位 分区颜色和细节分级 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterDestructionAssetTest,
    "BBB.Monster.Gore.DestructionAssetContract", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterDestructionAssetTest::RunTest(const FString&)
{
    int32 Checked = 0;
    for (const FString Name : {TEXT("Alice"), TEXT("Connor"), TEXT("Jiho"), TEXT("Kiyo"), TEXT("Michael"),
        TEXT("Morita"), TEXT("Renzo"), TEXT("Sakurada"), TEXT("Setsuko"), TEXT("Yuina"), TEXT("TargetConnor"), TEXT("Male"), TEXT("Female")})
    {
        const bool bFemale = Name == TEXT("Alice") || Name == TEXT("Kiyo") || Name == TEXT("Setsuko") || Name == TEXT("Yuina") || Name == TEXT("Female");
        const bool bBase = Name == TEXT("Male") || Name == TEXT("Female");
        const FString Path = Name == TEXT("TargetConnor")
            ? TEXT("/Game/_Project/Debug/ZombieHitReaction/DA_TargetPresentation.DA_TargetPresentation")
            : bBase ? FString::Printf(TEXT("/Game/_Project/Mass/Monster/Zombie/%s/DA_BBBZombie%sPresentation.DA_BBBZombie%sPresentation"), *Name, *Name, *Name)
            : FString::Printf(TEXT("/Game/_Project/Mass/Monster/Zombie/%s/Variants/%s/DA_BBBZombie%sPresentation.DA_BBBZombie%sPresentation"),
                bFemale ? TEXT("Female") : TEXT("Male"), *Name, *Name, *Name);
        const auto* Presentation = LoadObject<UBBBMonsterPresentationDefinition>(nullptr, *Path);
        if (!TestNotNull(*Name, Presentation) || !TestTrue(*Name, Presentation->IsValid()))
        {
            continue;
        }
        const auto* Gore = Presentation->Gore.Get();
        TestEqual(TEXT("毁伤八区完整"), Gore->Destruction->Regions.Num(), 8);
        TestEqual(TEXT("九个预制断点保留"), Gore->Dismemberment->Parts.Num(), 9);
        const auto* Mesh = Gore->Destruction->Interior.Get();
        const auto& Actual = Mesh->GetRefSkeleton();
        const auto& Expected = Gore->Body->GetRefSkeleton();
        TestEqual(TEXT("共享骨架保持原始骨名数量"), Gore->Body->GetSkeleton()->GetReferenceSkeleton().GetNum(), Expected.GetNum());
        TestEqual(TEXT("组织网格保持原始骨名数量"), Actual.GetNum(), Expected.GetNum());
        for (int32 Index = 0; Index < Expected.GetNum(); ++Index)
        {
            const int32 Other = Actual.FindBoneIndex(Expected.GetBoneName(Index));
            if (!TestTrue(*Expected.GetBoneName(Index).ToString(), Other != INDEX_NONE))
            {
                continue;
            }
            TestTrue(TEXT("参考骨骼单位与姿势匹配"),
                Expected.GetRefBonePose()[Index].Equals(Actual.GetRefBonePose()[Other], 0.01f));
        }
        TestEqual(TEXT("共享组织仅一个材质槽"), Mesh->GetMaterials().Num(), 1);
        TestTrue(TEXT("三档组织精细度已生成"), Mesh->GetLODNum() >= 3);
        const auto* Data = Mesh->GetResourceForRendering();
        if (Data && !Data->LODRenderData.IsEmpty())
        {
            const auto& LOD = Data->LODRenderData[0];
            const auto& Colors = LOD.StaticVertexBuffers.ColorVertexBuffer;
            bool Owners[9] = {};
            int32 InvalidOwners = 0;
            int32 InvalidKinds = 0;
            for (uint32 Index = 0; Index < Colors.GetNumVertices(); ++Index)
            {
                const FColor Color = Colors.VertexColor(Index);
                const int32 Owner = FMath::RoundToInt(Color.R * 8.0f / 255);
                const int32 Kind = FMath::RoundToInt(Color.G * 3.0f / 255);
                InvalidOwners += FMath::Abs(Color.R - Owner * 255.0f / 8) >= 1.1f ? 1 : 0;
                InvalidKinds += FMath::Abs(Color.G - Kind * 85.0f) >= 1.1f ? 1 : 0;
                if (Owner >= 0 && Owner <= 8)
                {
                    Owners[Owner] = true;
                }
            }
            TestEqual(*(Name + TEXT(" 肉壁归属编码错误数")), InvalidOwners, 0);
            TestEqual(*(Name + TEXT(" 组织图集编码错误数")), InvalidKinds, 0);
            for (const bool bPresent : Owners)
            {
                TestTrue(TEXT("共享内部和八处肉壁均保留"), bPresent);
            }
            TestTrue(TEXT("参考空间组织尺寸为厘米"), Mesh->GetBounds().BoxExtent.GetMax() > 15
                && Mesh->GetBounds().BoxExtent.GetMax() < 100);
        }
        ++Checked;
        AddInfo(Name + TEXT(" 资源检查完成"));
    }
    TestEqual(TEXT("十种外观 两个基础模板及固定靶均已核验"), Checked, 13);
    return true;
}
#endif
