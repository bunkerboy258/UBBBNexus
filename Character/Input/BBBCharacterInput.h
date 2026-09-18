#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterControlInput.h"
#include "BBBWork/UBBBNexus/Character/Input/Behaviors/BBBEquipBehavior.h"
#include "BBBWork/UBBBNexus/Character/Input/Behaviors/BBBFireBehavior.h"
#include "BBBWork/UBBBNexus/Character/Input/Behaviors/BBBReloadBehavior.h"
#include "BBBWork/UBBBNexus/Character/Input/Events/BBBEquipmentActionEvent.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterAnimationInput.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterMontagePacket.h"
#include "BBBWork/UBBBNexus/PlayerCamera/Input/BBBPlayerCameraInput.h"
struct FBBBInputRuntimeData;
struct FBBBCharacterRestoreInput;
class FBBBCharacterInitializer;
class FBBBCharacterNetworkSystem;

/** 角色固定类型输入入口只负责校验与入队 */
class ABBB_EVAC_API FBBBCharacterInput final
{
public:
    /**
     * 提交世界空间移动
     * @param Packet	移动事实
     * @return 是否接受输入
     */
    bool Submit(const FBBBCharacterControlInput &Packet);
    /**
     * 提交离散装备请求
     * @param Packet	装备操作
     * @return 是否接受输入
     */
    bool Submit(const FBBBEquipInput &Packet);

    /**
     * 提交本帧开火请求
     * @param Packet	开火输入
     * @return 是否接受输入
     */
    bool Submit(const FBBBFireInput &Packet);

    /**
     * 提交换弹请求
     * @param Packet	换弹输入
     * @return 是否接受输入
     */
    bool Submit(const FBBBReloadInput &Packet);
    /**
     * 提交已完成的装备结果
     * @param Packet	装备事实
     * @return 是否接受输入
     */
    bool Submit(const FBBBEquipmentActionEvent &Packet);
    /**
     * 提交动画阶段
     * @param Packet	带有操作序号的通知
     * @return 是否接受输入
     */
    bool Submit(const FBBBCharacterReloadAnimationInput &Packet);
    /**
     * 提交蒙太奇贡献
     * @param Packet	已获准操作的表现
     * @return 是否接受输入
     */
    bool Submit(const FBBBCharacterMontagePacket &Packet);
    /**
     * 提交相机贡献
     * @param Packet	相机冲量
     * @return 是否接受输入
     */
    bool Submit(const FBBBPlayerCameraInput &Packet);

private:
    friend class FBBBCharacterInitializer;
    friend class FBBBCharacterNetworkSystem;

    void Initialize(FBBBInputRuntimeData &Data);
    void SubmitRestore(const FBBBCharacterRestoreInput &Packet);
    FBBBInputRuntimeData *Data = nullptr;
};
