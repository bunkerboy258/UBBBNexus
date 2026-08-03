
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Context/BBBNetworkRuntimeData.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"

UBBBCharacterNetworkComponent::UBBBCharacterNetworkComponent()
{

    //关闭Tick 由事件驱动
    PrimaryComponentTick.bCanEverTick = false;

    //启用网络复制
    SetIsReplicatedByDefault(true);
}

void UBBBCharacterNetworkComponent::Initialize(FBBBNetworkRuntimeData &InNetworkData)
{
    NetworkData = &InNetworkData;
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

    if (!ensureMsgf(NetworkData, TEXT("[UBBBC]Network component method aborted: network data is null")))
    { return; }
    NetworkData->SetPendingRestoreAimState(AimState);
}

void UBBBCharacterNetworkComponent::SendEquipmentPacket(FBBBEquipmentNetworkPacket Packet)
{

    //拥有者具备服务器权威时直接处理请求
    if (GetOwner() && GetOwner()->HasAuthority())
    {
        if (!ensureMsgf(NetworkData, TEXT("[UBBBC]Equipment packet send failed because network data is null")))
        { return; }
        //把装备请求交给服务器校验链
        NetworkData->EnqueueValidationEquipmentPacket(MoveTemp(Packet));
        //结束当前把装备请求交给服务器校验链流程
        return;
    }

    //将装备请求发送到服务器
    ServerUploadEquipmentPacket(Packet);
}

//根据本机权限上传或广播射击事件
void UBBBCharacterNetworkComponent::SendFirePacket(FBBBFireNetworkPacket Packet)
{

    //拥有者具备服务器权威时直接处理请求
    if (GetOwner() && GetOwner()->HasAuthority())
    {
        //将确认后的射击事件广播给所有客户端
        MulticastFirePacket(Packet);
        //结束当前将确认后的射击事件广播给所有客户端流程
        return;
    }

    //将射击事件发送到服务器
    ServerUploadFirePacket(Packet);
}

//根据本机权限上传或广播换弹事件
void UBBBCharacterNetworkComponent::SendReloadPacket(FBBBReloadNetworkPacket Packet)
{

    //拥有者具备服务器权威时直接处理请求
    if (GetOwner() && GetOwner()->HasAuthority())
    {
        //将确认后的换弹事件广播给所有客户端
        MulticastReloadPacket(Packet);
        //结束当前将确认后的换弹事件广播给所有客户端流程
        return;
    }

    //将换弹事件发送到服务器
    ServerUploadReloadPacket(Packet);
}

//根据本机权限提交瞄准同步状态
void UBBBCharacterNetworkComponent::SendAimState(FBBBAimNetworkState AimState)
{

    //拥有者具备服务器权威时直接处理请求
    if (GetOwner() && GetOwner()->HasAuthority())
    {
        //写入权威瞄准状态并驱动服务器本地还原
        SetReplicatedAimState(AimState);
        //结束当前写入权威瞄准状态并驱动服务器本地还原流程
        return;
    }

    //将瞄准状态发送到服务器
    ServerSubmitAimState(AimState);
}

//在服务器执行Upload装备数据包
void UBBBCharacterNetworkComponent::ServerUploadEquipmentPacket_Implementation(FBBBEquipmentNetworkPacket Packet)
{
    //缺少网络系统时记录错误并停止当前流程
    if (!ensureMsgf(NetworkData, TEXT("[UBBBC]Network component method aborted: network data is null")))
    {
        //结束当前在服务器执行Upload装备数据包流程
        return;
    }

    //把装备请求交给服务器校验链
    NetworkData->EnqueueValidationEquipmentPacket(MoveTemp(Packet));
}

//在服务器执行Upload射击数据包
void UBBBCharacterNetworkComponent::ServerUploadFirePacket_Implementation(FBBBFireNetworkPacket Packet)
{

    //将确认后的射击事件广播给所有客户端
    MulticastFirePacket(Packet);
}

//在服务器执行Upload换弹数据包
void UBBBCharacterNetworkComponent::ServerUploadReloadPacket_Implementation(FBBBReloadNetworkPacket Packet)
{

    //将确认后的换弹事件广播给所有客户端
    MulticastReloadPacket(Packet);
}

//在各客户端接收并处理装备数据包
void UBBBCharacterNetworkComponent::MulticastEquipmentPacket_Implementation(FBBBEquipmentNetworkPacket Packet)
{
    if (!ensureMsgf(NetworkData, TEXT("[UBBBC]Network component method aborted: network data is null")))
    { return; }

    APawn *OwnerPawn = GetOwnerPawn();
    //跳过本地控制角色
    if (!OwnerPawn || OwnerPawn->IsLocallyControlled())
    { return; }

    //提交还原数据包
    NetworkData->EnqueueRestoreEquipmentPacket(MoveTemp(Packet));
}

//在各客户端接收并处理射击数据包
void UBBBCharacterNetworkComponent::MulticastFirePacket_Implementation(FBBBFireNetworkPacket Packet)
{
    //读取拥有者Pawn
    APawn *OwnerPawn = GetOwnerPawn();

    //跳过本地自主角色避免重复应用自身预测结果
    if (!OwnerPawn || OwnerPawn->IsLocallyControlled())
    {
        //结束当前读取拥有者Pawn流程
        return;
    }
    //缺少网络系统时记录错误并停止当前流程
    if (!ensureMsgf(NetworkData, TEXT("[UBBBC]Network component method aborted: network data is null")))
    {
        //结束当前读取拥有者Pawn流程
        return;
    }

    //提交还原数据包
    NetworkData->EnqueueRestoreFirePacket(MoveTemp(Packet));
}

//在各客户端接收并处理换弹数据包
void UBBBCharacterNetworkComponent::MulticastReloadPacket_Implementation(FBBBReloadNetworkPacket Packet)
{
    //读取拥有者Pawn
    APawn *OwnerPawn = GetOwnerPawn();

    //跳过本地自主角色避免重复应用自身预测结果
    if (!OwnerPawn || OwnerPawn->IsLocallyControlled())
    {
        //结束当前读取拥有者Pawn流程
        return;
    }
    //缺少网络系统时记录错误并停止当前流程
    if (!ensureMsgf(NetworkData, TEXT("[UBBBC]Network component method aborted: network data is null")))
    {
        //结束当前读取拥有者Pawn流程
        return;
    }

    //提交还原数据包
    NetworkData->EnqueueRestoreReloadPacket(MoveTemp(Packet));
}

//在服务器执行Submit瞄准状态
void UBBBCharacterNetworkComponent::ServerSubmitAimState_Implementation(FBBBAimNetworkState AimState)
{

    //写入权威瞄准状态并驱动服务器本地还原
    SetReplicatedAimState(AimState);
}

//在瞄准状态复制到达后驱动模拟端还原
void UBBBCharacterNetworkComponent::OnRep_ReplicatedAimState()
{
    //缺少网络系统时记录错误并停止当前流程
    if (!ensureMsgf(NetworkData, TEXT("[UBBBC]Network component method aborted: network data is null")))
    {
        //结束当前在瞄准状态复制到达后驱动模拟端还原流程
        return;
    }

    //把权威瞄准状态交给还原链
    NetworkData->SetPendingRestoreAimState(ReplicatedAimState);
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
