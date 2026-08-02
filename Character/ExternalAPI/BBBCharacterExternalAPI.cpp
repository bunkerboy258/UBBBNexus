
#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBCharacterAnimationCommands.h"
#include "BBBWork/UBBBNexus/Character/System/CameraSystem/Definition/BBBCameraRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemEvents.h"

void FBBBCharacterExternalAPI::Initialize(
    FBBBCharacterAnimationCommands &InAnimationCommands,
    FBBBCameraCommands &InCameraCommands,
    FBBBCharacterItemEvents &InItemEvents)
{
    AnimationCommands = &InAnimationCommands;
    CameraCommands = &InCameraCommands;
    ItemEvents = &InItemEvents;
}

//压入动画蒙太奇请求
void FBBBCharacterExternalAPI::QueueMontage(
    const FBBBCharacterAnimationRequest &Request)
{
    //缺少动画Commands时记录错误并停止当前流程
    if (!ensureMsgf(AnimationCommands, TEXT("[UBBBC]Character external animation target is null")))
    {
        //结束当前加入队列物品蒙太奇流程
        return;
    }

    //加入队列蒙太奇
    AnimationCommands->QueueMontage(Request);
}

//提交后坐力
void FBBBCharacterExternalAPI::SubmitCameraRecoil(
    const FVector2D &Impulse,
    float RecoverySpeed)
{
    if (!ensureMsgf(CameraCommands, TEXT("[UBBBC]Character external camera target is null")))
    {
        return;
    }

    CameraCommands->SubmitRecoilRequest(Impulse, RecoverySpeed);
}

//通知射击事件
void FBBBCharacterExternalAPI::PublishItemFireEvent()
{
    if (!ensureMsgf(ItemEvents, TEXT("[UBBBC]Character external item target is null")))
    {
        return;
    }

    ItemEvents->PublishFire();
}

//通知换弹事件
void FBBBCharacterExternalAPI::PublishReloadEvent()
{
    if (!ensureMsgf(ItemEvents, TEXT("[UBBBC]Character external item target is null")))
    {
        return;
    }

    ItemEvents->PublishReload();
}

//压入锁定物品ik请求
void FBBBCharacterExternalAPI::SubmitItemIKBlockRequest(bool bBlocked)
{
    if (!ensureMsgf(AnimationCommands, TEXT("[UBBBC]Character external animation target is null")))
    {
        return;
    }

    AnimationCommands->SubmitItemIKBlockRequest(bBlocked);
}
