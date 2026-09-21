
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Logic/Core/Initialization/BBBCharacterInitializer.h"
#include "BBBWork/UBBBNexus/Character/Logic/Core/Shutdown/BBBCharacterShutdown.h"
#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"

ABBBCharacter::ABBBCharacter()
{
    //启用帧更新
    PrimaryActorTick.bCanEverTick = true;

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
    CharacterUpdatePipeline.Update(DeltaSeconds);
}

//------------------------------------------------------------------------------

void ABBBCharacter::RegisterActorTickFunctions(bool bRegister)
{
    Super::RegisterActorTickFunctions(bRegister);

    CharacterUpdatePipeline.RegisterTickFunctions(*this, bRegister);
}

//------------------------------------------------------------------------------

bool ABBBCharacter::ShouldReplicateAcceleration() const
{
    return true;
}

//------------------------------------------------------------------------------
