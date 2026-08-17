
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkSystem.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"

UBBBCharacterNetworkComponent::UBBBCharacterNetworkComponent()
{

    //关闭Tick 由事件驱动
    PrimaryComponentTick.bCanEverTick = false;

    //启用网络复制
    SetIsReplicatedByDefault(true);
}

void UBBBCharacterNetworkComponent::Initialize(FBBBCharacterNetworkSystem &InNetworkSystem)
{
    NetworkSystem = &InNetworkSystem;
}

void UBBBCharacterNetworkComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    //同步瞄准状态给所有客户端 除了拥有这个 Actor 的那个客户端
    DOREPLIFETIME_CONDITION(UBBBCharacterNetworkComponent, ReplicatedAimState, COND_SkipOwner);
}

void UBBBCharacterNetworkComponent::SetReplicatedAimState(const FBBBAimNetworkState &AimState)
{
    ReplicatedAimState = AimState;
}

//在服务器执行Upload装备数据包
void UBBBCharacterNetworkComponent::ServerUploadEquipmentPacket_Implementation(FBBBEquipmentNetworkPacket Packet)
{
    if (!ensureMsgf(NetworkSystem, TEXT("[UBBBC]Equipment packet receive failed because network system is null")))
    {
        return;
    }

    NetworkSystem->ReceiveEquipmentForValidation(MoveTemp(Packet));
}

//在服务器执行Upload射击数据包
void UBBBCharacterNetworkComponent::ServerUploadFirePacket_Implementation(FBBBFireNetworkPacket Packet)
{

    if (!ensureMsgf(NetworkSystem, TEXT("[UBBBC]Fire packet receive failed because network system is null")))
    {
        return;
    }

    NetworkSystem->ReceiveFireForDistribution(MoveTemp(Packet));
}

//在服务器执行Upload换弹数据包
void UBBBCharacterNetworkComponent::ServerUploadReloadPacket_Implementation(FBBBReloadNetworkPacket Packet)
{

    if (!ensureMsgf(NetworkSystem, TEXT("[UBBBC]Reload packet receive failed because network system is null")))
    {
        return;
    }

    NetworkSystem->ReceiveReloadForDistribution(MoveTemp(Packet));
}

//在各客户端接收并处理装备数据包
void UBBBCharacterNetworkComponent::MulticastEquipmentPacket_Implementation(FBBBEquipmentNetworkPacket Packet)
{
    if (!ensureMsgf(NetworkSystem, TEXT("[UBBBC]Equipment multicast receive failed because network system is null")))
    {
        return;
    }

    NetworkSystem->ReceiveEquipmentForRestore(MoveTemp(Packet));
}

//在各客户端接收并处理射击数据包
void UBBBCharacterNetworkComponent::MulticastFirePacket_Implementation(FBBBFireNetworkPacket Packet)
{
    if (!ensureMsgf(NetworkSystem, TEXT("[UBBBC]Fire multicast receive failed because network system is null")))
    {
        return;
    }

    NetworkSystem->ReceiveFireForRestore(MoveTemp(Packet));
}

//在各客户端接收并处理换弹数据包
void UBBBCharacterNetworkComponent::MulticastReloadPacket_Implementation(FBBBReloadNetworkPacket Packet)
{
    if (!ensureMsgf(NetworkSystem, TEXT("[UBBBC]Reload multicast receive failed because network system is null")))
    {
        return;
    }

    NetworkSystem->ReceiveReloadForRestore(MoveTemp(Packet));
}

//在服务器执行Submit瞄准状态
void UBBBCharacterNetworkComponent::ServerSubmitAimState_Implementation(FBBBAimNetworkState AimState)
{
    if (!ensureMsgf(NetworkSystem, TEXT("[UBBBC]Aim state receive failed because network system is null")))
    {
        return;
    }

    NetworkSystem->ReceiveSubmittedAimState(AimState);
}

//在瞄准状态复制到达后驱动模拟端还原
void UBBBCharacterNetworkComponent::OnRep_ReplicatedAimState()
{
    if (!ensureMsgf(NetworkSystem, TEXT("[UBBBC]Replicated aim state receive failed because network system is null")))
    {
        return;
    }

    NetworkSystem->ReceiveReplicatedAimState(ReplicatedAimState);
}

bool UBBBCharacterNetworkComponent::IsOwnerLocallyControlled() const
{
    APawn *OwnerPawn = GetOwnerPawn();
    return OwnerPawn && OwnerPawn->IsLocallyControlled();
}

bool UBBBCharacterNetworkComponent::IsOwnerAuthority() const
{
    APawn *OwnerPawn = GetOwnerPawn();
    return OwnerPawn && OwnerPawn->HasAuthority();
}

//读取拥有者Pawn
APawn *UBBBCharacterNetworkComponent::GetOwnerPawn() const
{
    //更新Pawn拥有者Pawn供后续步骤读取
    APawn *OwnerPawn = Cast<APawn>(GetOwner());

    //校验关键对象并在失效时记录可定位的错误
    ensureMsgf(OwnerPawn, TEXT("[UBBBC]Network component owner is not APawn"));
    //返回拥有者Pawn作为本次查询结果
    return OwnerPawn;
}
