
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Core/Initialization/BBBCharacterInitializer.h"
#include "BBBWork/UBBBNexus/Character/Core/Shutdown/BBBCharacterShutdown.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Animation/BBBReloadDetachPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Animation/BBBReloadInterruptPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Animation/BBBReloadLoadPacket.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"

ABBBCharacter::ABBBCharacter()
{
    //启用帧更新
    PrimaryActorTick.bCanEverTick = true;

    //LateUpdate与主管线处于同一更新组并通过依赖关系固定顺序
    LateUpdateTick.bCanEverTick = true;
    LateUpdateTick.bStartWithTickEnabled = false;
    LateUpdateTick.TickGroup = TG_PrePhysics;
    //允许网络同步
    bReplicates = true;
    //由引擎同步角色根组件的位置与旋转
    SetReplicateMovement(true);
    //移动时由移动组件自动朝向速度方向
    UCharacterMovementComponent *Movement = GetCharacterMovement();

    Movement->bTickBeforeOwner = false;
    Movement->PrimaryComponentTick.AddPrerequisite(this, PrimaryActorTick);

    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->bUseControllerDesiredRotation = false;
    //限制自动转向速度
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
    //保留空中调整移动方向的能力
    GetCharacterMovement()->AirControl = 0.25f;
    //阻止控制器俯仰直接旋转角色胶囊体
    bUseControllerRotationPitch = false;
    //角色朝向统一交给移动与朝向系统处理
    bUseControllerRotationYaw = false;
    //阻止控制器横滚直接旋转角色胶囊体
    bUseControllerRotationRoll = false;

    //将骨骼网格下移到胶囊体底部
    GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
    //将骨骼网格前轴对齐角色前进方向
    GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    /*其实非必要 一般是蓝图里调整*/

    //创建负责收发角色状态的网络组件
    CharacterNetworkComponent = CreateDefaultSubobject<UBBBCharacterNetworkComponent>(TEXT("CharacterNetworkComponent"));
}

void ABBBCharacter::BeginPlay()
{
    Super::BeginPlay();
    FBBBCharacterInitializer::Initialize(*this);

    //全部运行依赖注入完成后才允许执行LateUpdate
    LateUpdateTick.SetTickFunctionEnable(true);
}

//------------------------------------------------------------------------------

void ABBBCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // 结束游戏时先停止延迟更新并收束装备实例
    FBBBCharacterShutdown::Shutdown(*this);

    Super::EndPlay(EndPlayReason);
}

void ABBBCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    // 主更新只在角色所属世界有效时推进运行时状态
    //读取角色所属世界
    const UWorld *World = GetWorld();

    if (!ensureMsgf(World, TEXT("[UBBBC]Character update aborted because world is null")))
    { return; }

    //更新当前帧世界时间快照
    RuntimeData.WorldData.Update(DeltaSeconds, World->GetTimeSeconds());

    CharacterUpdate.Update();
}

//------------------------------------------------------------------------------

void ABBBCharacter::RegisterActorTickFunctions(bool bRegister)
{
    Super::RegisterActorTickFunctions(bRegister);

    UCharacterMovementComponent *Movement = GetCharacterMovement();
    USkeletalMeshComponent *CharacterMesh = GetMesh();

    if (!ensureMsgf(
        Movement && CharacterMesh,
        TEXT("[UBBBC]Character tick registration failed because required components are null")))
    {
        return;
    }

    if (bRegister)
    {
        // 注册时建立移动延迟更新和动画更新的依赖顺序
        LateUpdateTick.Target = this;
        LateUpdateTick.SetTickFunctionEnable(HasActorBegunPlay());
        LateUpdateTick.AddPrerequisite(Movement, Movement->PrimaryComponentTick);
        LateUpdateTick.RegisterTickFunction(GetLevel());

        //骨骼网格必须等待LateUpdate提交最终动画事实后才能更新动画图
        CharacterMesh->PrimaryComponentTick.AddPrerequisite(this, LateUpdateTick);
        return;
    }

    // 注销时移除全部更新依赖避免引擎继续回调角色
    CharacterMesh->PrimaryComponentTick.RemovePrerequisite(this, LateUpdateTick);
    LateUpdateTick.RemovePrerequisite(Movement, Movement->PrimaryComponentTick);
    LateUpdateTick.UnRegisterTickFunction();
    LateUpdateTick.Target = nullptr;
}

//------------------------------------------------------------------------------

bool ABBBCharacter::ShouldReplicateAcceleration() const
{
    return true;
}

//------------------------------------------------------------------------------

void ABBBCharacter::LateUpdate()
{
    // 将移动完成后的更新转交角色更新管线
    CharacterUpdatePipeline.LateUpdate();
}

void ABBBCharacter::ReportReloadStartNotify(const int32 Sequence)
{
    FBBBReloadDetachPacket Packet;
    Packet.Sequence = Sequence;
    SubmitInput(Packet);
}

void ABBBCharacter::ReportReloadEndNotify(const int32 Sequence, const EBBBCharacterReloadEndReasonDefinition EndReason)
{
    if (EndReason == EBBBCharacterReloadEndReasonDefinition::PlaybackFailed)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UBBBC]Reload playback failed Sequence=%d"), Sequence);
    }

    // 装填完成与中断分走不同包 由解析状态机裁决序号
    if (EndReason == EBBBCharacterReloadEndReasonDefinition::Loaded)
    {
        FBBBReloadLoadPacket Packet;
        Packet.Sequence = Sequence;
        SubmitInput(Packet);
        return;
    }

    FBBBReloadInterruptPacket Packet;
    Packet.Sequence = Sequence;
    SubmitInput(Packet);
}
