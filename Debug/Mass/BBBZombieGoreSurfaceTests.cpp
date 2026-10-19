#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "BBBZombieGoreSurfaceDefinition.h"
#include "BBBZombieSurfaceWoundComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Presentation/BBBProjectilePointDamageEvent.h"
#include "Engine/SkeletalMesh.h"
#include "Misc/AutomationTest.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "PhysicsEngine/PhysicsConstraintTemplate.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "StaticMeshAttributes.h"

/** 只读取资源与静态物理参数 不启动游戏或创建世界 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBZombieGoreSurfaceAssetTest, "UBBB.Debug.ZombieGoreSurfaceAssets",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBZombieGoreSurfaceAssetTest::RunTest(const FString&)
{
    const auto* Definition = LoadObject<UBBBZombieGoreSurfaceDefinition>(nullptr,
        TEXT("/Game/_Project/Debug/ZombieHitReaction/Gore/DA_TargetGoreSurface.DA_TargetGoreSurface"));
    if (!TestNotNull(TEXT("独立调试定义已保存"), Definition))
    {
        return false;
    }
    TestNotNull(TEXT("独立身体"), Definition->Body.Get());
    if (Definition->Body && Definition->Body->GetResourceForRendering())
    {
        TestEqual(TEXT("GPU 蒙皮仅使用四组纹理坐标"), Definition->Body->GetResourceForRendering()->LODRenderData[0].StaticVertexBuffers.StaticMeshVertexBuffer.GetNumTexCoords(), 4u);
    }
    TestNotNull(TEXT("立体重伤组织"), Definition->HeavyWound.Get());
    TestEqual(TEXT("固定图集上限"), Definition->AtlasSize, 2048);
    TestEqual(TEXT("头部一个与四肢各两个断点"), Definition->Cuts.Num(), 9);
    TSet<int32> Keys;
    for (const auto& Cut : Definition->Cuts)
    {
        const int32 Key = static_cast<int32>(Cut.Region) * 10 + Cut.Tier;
        TestFalse(TEXT("断点不能重复"), Keys.Contains(Key));
        Keys.Add(Key);
        if (!TestNotNull(TEXT("身体封口"), Cut.Cap.Get()) || !TestNotNull(TEXT("掉落部件"), Cut.Piece.Get()))
        {
            continue;
        }
        TestTrue(TEXT("断口几何仅在调试目录"), Cut.Cap->GetPathName().StartsWith(TEXT("/Game/_Project/Debug/")));
        TestFalse(TEXT("断口中心有限"), Cut.LocalCenter.ContainsNaN());
        const auto* CapDescription = Cut.Cap->GetMeshDescription(0);
        if (TestNotNull(TEXT("断面有实际拓扑"), CapDescription))
        {
            const FStaticMeshConstAttributes Attributes(*CapDescription);
            int32 BoneFaces = 0;
            for (const auto Triangle : CapDescription->Triangles().GetElementIDs())
            {
                bool bBone = true;
                for (const auto Instance : CapDescription->GetTriangleVertexInstances(Triangle))
                {
                    bBone &= Attributes.GetVertexInstanceColors()[Instance].X > .7f;
                }
                BoneFaces += bBone ? 1 : 0;
            }
            TestEqual(TEXT("实际断面已删除骨头几何标记"), BoneFaces, 0);
        }
        const auto* Physics = Cut.Piece->GetPhysicsAsset();
        if (!TestNotNull(TEXT("掉落部件有独立物理"), Physics))
        {
            continue;
        }
        const bool bLong = Cut.Tier == 1 && Cut.Region != EBBBMonsterHitRegion::Head;
        TestEqual(TEXT("长肢体两刚体 短肢体一刚体"), Physics->SkeletalBodySetups.Num(), bLong ? 2 : 1);
        TestEqual(TEXT("长肢体只有一个受限关节"), Physics->ConstraintSetup.Num(), bLong ? 1 : 0);
        for (const auto& Constraint : Physics->ConstraintSetup)
        {
            const auto& Joint = Constraint->DefaultInstance;
            TestEqual(TEXT("关节线性轴锁定"), Joint.GetLinearXMotion(), LCM_Locked);
            TestEqual(TEXT("关节线性第二轴锁定"), Joint.GetLinearYMotion(), LCM_Locked);
            TestEqual(TEXT("关节线性第三轴锁定"), Joint.GetLinearZMotion(), LCM_Locked);
            TestEqual(TEXT("摆动为受限运动"), Joint.GetAngularSwing1Motion(), ACM_Limited);
            TestEqual(TEXT("扭转为受限运动"), Joint.GetAngularTwistMotion(), ACM_Limited);
            TestTrue(TEXT("摆动限制不超过二十五度"), Joint.GetAngularSwing1Limit() <= 25.0f);
            TestTrue(TEXT("第二摆动限制不超过十五度"), Joint.GetAngularSwing2Limit() <= 15.0f);
            TestTrue(TEXT("扭转限制不超过十二度"), Joint.GetAngularTwistLimit() <= 12.0f);
            TestFalse(TEXT("摆动使用硬限制"), Joint.ProfileInstance.ConeLimit.bSoftConstraint);
            TestFalse(TEXT("扭转使用硬限制"), Joint.ProfileInstance.TwistLimit.bSoftConstraint);
            TestEqual(TEXT("默认保存配置同步"), Constraint->GetConstraintProfilePropertiesOrDefault(NAME_None).TwistLimit.TwistLimitDegrees, 12.0f);
        }
    }
    return true;
}

/** 公开点命中保留标准引擎类型且表现强度独立 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBZombieWoundEventTest, "UBBB.Debug.ZombieWoundEvent",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBBBZombieWoundEventTest::RunTest(const FString&)
{
    FBBBProjectilePointDamageEvent Event;
    Event.Damage = 20;
    Event.WoundStrength = 3;
    TestTrue(TEXT("仍属于标准点伤害"), Event.IsOfType(FPointDamageEvent::ClassID));
    TestTrue(TEXT("可识别额外表现参数"), Event.IsOfType(FBBBProjectilePointDamageEvent::ClassID));
    TestEqual(TEXT("表现强度不修改伤害"), Event.Damage, 20.0f);
    const auto* Surface = GetDefault<UBBBZombieSurfaceWoundComponent>();
    TestFalse(TEXT("不为调试外观增加独立逐帧调度"), Surface->PrimaryComponentTick.bCanEverTick);
    TestEqual(TEXT("当前断点固定六个槽"), Surface->SelectedTiers.Num(), 6);
    TestEqual(TEXT("默认使用明显抛飞"), Surface->LaunchSpeed, 650.0f);
    return true;
}

#endif
