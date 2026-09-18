#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterControlInput.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterEquipmentInput.h"
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
     * 提交持续控制状态
     * @param Packet	世界空间控制事实
     * @return 是否接受输入
     */
    bool Submit(const FBBBCharacterControlInput &Packet);
    /**
     * 提交离散装备请求
     * @param Packet	装备操作
     * @return 是否接受输入
     */
    bool Submit(const FBBBCharacterEquipInput &Packet);

    /** @return 是否接受开火请求 */
    bool Submit(const FBBBCharacterFireInput &Packet);

    /** @return 是否接受换弹请求 */
    bool Submit(const FBBBCharacterReloadInput &Packet);
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
