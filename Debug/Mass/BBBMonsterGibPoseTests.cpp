#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS
#include "ReferenceSkeleton.h"
#include "Engine/SkeletalMesh.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterGoreDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterPresentationDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterGibPresentationSubsystem.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMonsterDetachedPoseOrderTest,
    "BBB.Monster.Gore.DetachedPoseBoneOrder", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBMonsterDetachedPoseOrderTest::RunTest(const FString&)
{
    FReferenceSkeleton Source;
    FReferenceSkeleton Part;
    {
        FReferenceSkeletonModifier SourceBuilder(Source, nullptr);
        FReferenceSkeletonModifier PartBuilder(Part, nullptr);
        SourceBuilder.Add(FMeshBoneInfo(TEXT("root"), TEXT("root"), INDEX_NONE), FTransform::Identity);
        SourceBuilder.Add(FMeshBoneInfo(TEXT("arm"), TEXT("arm"), 0), FTransform::Identity);
        SourceBuilder.Add(FMeshBoneInfo(TEXT("leg"), TEXT("leg"), 0), FTransform::Identity);
        PartBuilder.Add(FMeshBoneInfo(TEXT("root"), TEXT("root"), INDEX_NONE), FTransform::Identity);
        PartBuilder.Add(FMeshBoneInfo(TEXT("leg"), TEXT("leg"), 0), FTransform::Identity);
        PartBuilder.Add(FMeshBoneInfo(TEXT("arm"), TEXT("arm"), 0), FTransform::Identity);
    }
    TArray<FTransform> Captured = {
        FTransform(FVector(10, 20, 30)), FTransform(FVector(110, 20, 30)), FTransform(FVector(10, 220, 30))};
    TArray<FTransform> Local;
    TestTrue(TEXT("同数量不同顺序按骨骼名称捕获"),
        UBBBMonsterGibPresentationSubsystem::BuildDetachedPose(Source, Captured, Part, Local));
    if (Local.Num() == 3)
    {
        TestTrue(TEXT("腿保持腿的局部位置"), Local[1].GetTranslation().Equals(FVector(0, 200, 0)));
        TestTrue(TEXT("手臂保持手臂的局部位置"), Local[2].GetTranslation().Equals(FVector(100, 0, 0)));
    }
    TestEqual(TEXT("每根残肢骨骼均有输出"), Local.Num(), 3);
    Captured.Pop();
    TestFalse(TEXT("不完整身体姿态拒绝生成"),
        UBBBMonsterGibPresentationSubsystem::BuildDetachedPose(Source, Captured, Part, Local));
    TestTrue(TEXT("拒绝时不遗留部分姿态"), Local.IsEmpty());

    const TCHAR* Variants[] = {TEXT("Male/Connor"), TEXT("Male/Jiho"), TEXT("Male/Michael"), TEXT("Male/Morita"),
        TEXT("Male/Renzo"), TEXT("Male/Sakurada"), TEXT("Female/Alice"), TEXT("Female/Kiyo"),
        TEXT("Female/Setsuko"), TEXT("Female/Yuina")};
    int32 CheckedParts = 0;
    int32 ReorderedParts = 0;
    for (const TCHAR* Variant : Variants)
    {
        FString Sex;
        FString Name;
        FString(Variant).Split(TEXT("/"), &Sex, &Name);
        const FString Path = FString::Printf(TEXT("/Game/_Project/Mass/Monster/Zombie/%s/Variants/%s/DA_BBBZombie%sPresentation"),
            *Sex, *Name, *Name);
        const auto* Definition = LoadObject<UBBBMonsterPresentationDefinition>(nullptr, *Path);
        if (!TestNotNull(*Path, Definition) || !Definition->Gore || !Definition->Gore->Body)
        {
            continue;
        }
        const auto& BodySkeleton = Definition->Gore->Body->GetRefSkeleton();
        TArray<FTransform> Current;
        for (int32 Index = 0; Index < BodySkeleton.GetNum(); ++Index)
        {
            FTransform Bone = BodySkeleton.GetRefBonePose()[Index];
            Bone.SetRotation(FQuat(FVector::UpVector, 0.013 * Index) * Bone.GetRotation());
            const int32 Parent = BodySkeleton.GetParentIndex(Index);
            Current.Add(Parent == INDEX_NONE ? Bone : Bone * Current[Parent]);
        }
        for (const auto& Wound : Definition->Gore->Wounds)
        {
            if (!Wound.DetachedSkin)
            {
                continue;
            }
            const auto& Detached = Wound.DetachedSkin->GetRefSkeleton();
            if (!TestTrue(*Wound.DetachedSkin->GetName(),
                UBBBMonsterGibPresentationSubsystem::BuildDetachedPose(BodySkeleton, Current, Detached, Local)))
            {
                continue;
            }
            TArray<FTransform> Restored;
            bool bReordered = false;
            for (int32 Index = 0; Index < Detached.GetNum(); ++Index)
            {
                const int32 Parent = Detached.GetParentIndex(Index);
                Restored.Add(Parent == INDEX_NONE ? Local[Index] : Local[Index] * Restored[Parent]);
                const int32 SourceIndex = BodySkeleton.FindBoneIndex(Detached.GetBoneName(Index));
                bReordered |= SourceIndex != Index;
                TestTrue(*FString::Printf(TEXT("%s %s 捕获后骨骼位置与旋转一致"), *Name, *Detached.GetBoneName(Index).ToString()),
                    Restored[Index].Equals(Current[SourceIndex], 0.001));
            }
            ReorderedParts += bReordered ? 1 : 0;
            ++CheckedParts;
        }
    }
    TestEqual(TEXT("十种外观九类残肢均完成姿态还原"), CheckedParts, 90);
    TestTrue(TEXT("实际资源覆盖骨骼排列不同的回归条件"), ReorderedParts > 0);
    return true;
}
#endif
