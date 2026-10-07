#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/BBBMeleeEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Config/BBBMeleeDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/ActionSystem/DomainData/Context/BBBMeleeUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/ParseSystem/Processors/BBBMeleeParseProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/ActionSystem/Processors/BBBMeleeActionProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/ActionSystem/Processors/BBBMeleeContactProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/NetworkSystem/Processors/BBBMeleeNetworkProcessor.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/BBBCharacterParseSystem.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/Processors/BBBCharacterEquipmentAnimationInputProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/Context/BBBCharacterEquipmentUpdateContext.h"
#include "Components/BoxComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "Misc/ScopeExit.h"

/** 隔离物理世界验证通知窗口与攻击生命周期 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBBBMeleeWindowTest, "BBB.Equipment.Melee.WindowLifecycle",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FBBBMeleeWindowTest::RunTest(const FString &Parameters)
{
    const auto Initialization = UWorld::InitializationValues().AllowAudioPlayback(false)
        .CreatePhysicsScene(true).CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(false);
    UWorld *World = UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr,
        true, ERHIFeatureLevel::Num, &Initialization);
    if (!TestNotNull(TEXT("隔离世界"), World))
    {
        return false;
    }
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World);
        World->DestroyWorld(false);
    };
    auto *Character = World->SpawnActor<ABBBCharacter>();
    const auto *Definition = LoadObject<UBBBMeleeDefinition>(nullptr,
        TEXT("/Game/_Project/Characters/BBBC_UA/Equipment/Melee/Bat_01/DA_Bat_01.DA_Bat_01"));
    UClass *EquipmentClass = LoadClass<ABBBMeleeEquipment>(nullptr,
        TEXT("/Game/_Project/Characters/BBBC_UA/Equipment/Melee/Bat_01/BP_Bat_01.BP_Bat_01_C"));
    if (!TestNotNull(TEXT("首个近战装备配置"), Definition) || !TestNotNull(TEXT("实际近战装备蓝图"), EquipmentClass))
    {
        return false;
    }
    auto *Equipment = World->SpawnActor<ABBBMeleeEquipment>(EquipmentClass);
    auto *Target = World->SpawnActor<AActor>();
    auto *TargetCollision = NewObject<UBoxComponent>(Target);
    TargetCollision->SetBoxExtent(FVector(10.0f));
    TargetCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TargetCollision->SetCollisionResponseToAllChannels(ECR_Block);
    Target->SetRootComponent(TargetCollision);
    Target->SetActorLocation(FVector(0.0f, 0.0f, 50.0f));
    Target->SetCanBeDamaged(true);
    TargetCollision->RegisterComponent();
    World->Tick(LEVELTICK_All, 1.0f / 60.0f);
    Equipment->SetOwner(Character);
    Character->RuntimeData.External.NetworkIdentityState.bIsMirror = false;
    Equipment->DispatchBeginPlay();
    TestTrue(TEXT("真实资产通过装备初始化"), Equipment->IsInitialized());
    TestTrue(TEXT("近战配置具有明确类型"), Definition->EquipmentType == EBBBEquipmentType::Melee);
    TestTrue(TEXT("真实网格具有扫掠起点"), Equipment->GetEquipmentSkeletalMesh()->DoesSocketExist(Definition->TraceStartSocket));
    TestTrue(TEXT("真实网格具有扫掠终点"), Equipment->GetEquipmentSkeletalMesh()->DoesSocketExist(Definition->TraceEndSocket));
    UE_LOG(LogTemp, Log, TEXT("[BBBMelee] Test geometry Base=%s Tip=%s Target=%s"),
        *Equipment->GetEquipmentSkeletalMesh()->GetSocketLocation(Definition->TraceStartSocket).ToString(),
        *Equipment->GetEquipmentSkeletalMesh()->GetSocketLocation(Definition->TraceEndSocket).ToString(),
        *Target->GetActorLocation().ToString());
    FBBBMeleeUpdateContext Context{*Equipment, *Character, *Equipment->GetEquipmentSkeletalMesh(),
        *Definition, Equipment->RuntimeData, *World};
    auto &Data = Equipment->RuntimeData;
    FBBBMeleeContactProcessor::Update(Context);
    TestEqual(TEXT("没有窗口不结算任何命中"), Equipment->GetHitCount(), 0);
    FBBBMeleeParseProcessor::Submit(Data, FBBBMeleeAttackLocalControlPacket{});
    FBBBMeleeParseProcessor::Update(Context);
    FBBBMeleeActionProcessor::Update(Context);
    TestEqual(TEXT("主行为创建首轮攻击"), Equipment->GetAttackSequence(), 1);
    TestFalse(TEXT("攻击开始不能提前造成伤害"), Equipment->IsContactOpen());
    FBBBMeleeParseProcessor::Submit(Data, FBBBMeleeBeginActionLocalControlPacket{{10}});
    FBBBMeleeParseProcessor::Submit(Data, FBBBMeleeBeginActionLocalControlPacket{{11}});
    FBBBMeleeParseProcessor::Submit(Data, FBBBMeleeBeginContactLocalControlPacket{{20}});
    TestEqual(TEXT("同帧通知标识累计"), Data.Parse.ReadMeleeInputState().BeginAction.Packet.Tokens.Num(), 2);
    FBBBMeleeParseProcessor::Update(Context);
    FBBBMeleeActionProcessor::Update(Context);
    TestTrue(TEXT("收到开窗通知才启用伤害"), Equipment->IsContactOpen());
    TestEqual(TEXT("首个生命周期标识绑定当前攻击"), Data.Action.ReadMeleeActionState().ActionToken, 10);
    FBBBMeleeContactProcessor::Update(Context);
    const int32 FirstHits = Equipment->GetHitCount();
    TestTrue(TEXT("窗口内真实物理扫掠命中靶标"), FirstHits > 0);
    FBBBMeleeContactProcessor::Update(Context);
    TestEqual(TEXT("连续多帧同一目标只结算一次"), Equipment->GetHitCount(), FirstHits);
    FBBBMeleeParseProcessor::Submit(Data, FBBBMeleeEndContactLocalControlPacket{{99}});
    FBBBMeleeParseProcessor::Update(Context);
    FBBBMeleeActionProcessor::Update(Context);
    TestTrue(TEXT("错误窗口的结束通知不会关闭当前窗口"), Equipment->IsContactOpen());
    FBBBMeleeParseProcessor::Submit(Data, FBBBMeleeEndContactLocalControlPacket{{20}});
    FBBBMeleeParseProcessor::Submit(Data, FBBBMeleeAttackLocalControlPacket{});
    FBBBMeleeParseProcessor::Update(Context);
    FBBBMeleeActionProcessor::Update(Context);
    TestFalse(TEXT("正确关窗通知停止伤害"), Equipment->IsContactOpen());
    TestEqual(TEXT("关窗之后收手期间不允许下一次攻击"), Equipment->GetAttackSequence(), 1);
    FBBBMeleeParseProcessor::Submit(Data, FBBBMeleeEndActionLocalControlPacket{{99}});
    FBBBMeleeParseProcessor::Update(Context);
    FBBBMeleeActionProcessor::Update(Context);
    TestTrue(TEXT("迟到的其它生命周期结束不影响当前攻击"), Data.Action.ReadMeleeActionState().bAttacking);
    FBBBMeleeParseProcessor::Submit(Data, FBBBMeleeEndActionLocalControlPacket{{10}});
    FBBBMeleeParseProcessor::Update(Context);
    FBBBMeleeActionProcessor::Update(Context);
    TestFalse(TEXT("动作结束才恢复可攻击状态"), Data.Action.ReadMeleeActionState().bAttacking);
    FBBBMeleeActionProcessor::Stop(Data);
    TestTrue(TEXT("失活清除本轮命中集合"), Data.Action.ReadMeleeActionState().HitActors.IsEmpty());
    Character->RuntimeData.External.NetworkIdentityState.bIsMirror = true;
    Context.bCausal = false;
    FBBBMeleeAttackStartAuthorityFactPacket Mirror{{3}, {1}};
    FBBBMeleeParseProcessor::Submit(Data, Mirror);
    FBBBMeleeParseProcessor::Update(Context);
    FBBBMeleeActionProcessor::Update(Context);
    FBBBMeleeContactProcessor::Update(Context);
    TestEqual(TEXT("镜像采用当前序号"), Equipment->GetAttackSequence(), 3);
    TestFalse(TEXT("镜像绝不开启伤害窗口"), Equipment->IsContactOpen());
    TestEqual(TEXT("镜像绝不重复结算世界伤害"), Equipment->GetHitCount(), FirstHits);
    TestTrue(TEXT("动作结果输入数据有效"), Mirror.IsValid());
    FBBBMeleeAttackStartAuthorityFactPacket InvalidResult{{-1}, {2}};
    TestFalse(TEXT("拒绝无效动作序号"), InvalidResult.IsValid());
    return true;
}
#endif
