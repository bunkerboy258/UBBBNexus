
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Camera/CameraComponent.h"
#include "BBBWork/UBBBNexus/Character/Instance/Core/Initialization/BBBCharacterInitializer.h"
#include "BBBWork/UBBBNexus/Character/Instance/Core/Shutdown/BBBCharacterShutdown.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
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
    bUseControllerRotationYaw = true;
    //阻止控制器横滚直接旋转角色胶囊体
    bUseControllerRotationRoll = false;

    //将骨骼网格下移到胶囊体底部
    GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
    //将骨骼网格前轴对齐角色前进方向
    GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    /*其实非必要 一般是蓝图里调整*/

    //创建第三人称相机弹簧臂
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    //将弹簧臂绑定到角色根组件
    CameraBoom->SetupAttachment(GetRootComponent());
    //创建跟随相机
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    //将相机绑定到弹簧臂末端插槽
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
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
    FBBBCharacterShutdown::Shutdown(*this);

    Super::EndPlay(EndPlayReason);
}

void ABBBCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    //读取角色所属世界
    const UWorld *World = GetWorld();

    if (!ensureMsgf(World, TEXT("[UBBBC]Character update aborted because world is null")))
    { return; }

    //更新当前帧世界时间快照
    RuntimeData.WorldData.Update(DeltaSeconds, World->GetTimeSeconds());

    CharacterUpdatePipeline.Update();
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
        LateUpdateTick.Target = this;
        LateUpdateTick.SetTickFunctionEnable(HasActorBegunPlay());
        LateUpdateTick.AddPrerequisite(Movement, Movement->PrimaryComponentTick);
        LateUpdateTick.RegisterTickFunction(GetLevel());

        //骨骼网格必须等待LateUpdate提交最终动画事实后才能更新动画图
        CharacterMesh->PrimaryComponentTick.AddPrerequisite(this, LateUpdateTick);
        return;
    }

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
    CharacterUpdatePipeline.LateUpdate();
}

void ABBBCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    FBBBCharacterInitializer::BindInput(*this, PlayerInputComponent);
}

void ABBBCharacter::ReportReloadStartNotify(const int32 Sequence)
{
    ExternalAPI.SubmitReloadStartNotify(Sequence);
}

void ABBBCharacter::ReportReloadEndNotify(const int32 Sequence, const EBBBCharacterReloadEndReason EndReason)
{
    ExternalAPI.SubmitReloadEndNotify(Sequence, EndReason);
}
