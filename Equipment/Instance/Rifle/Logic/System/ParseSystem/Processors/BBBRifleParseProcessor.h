#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/BBBRifleInputSubmit.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/RuntimeData/BBBRifleRuntimeData.h"

struct FBBBRifleUpdateContext;
struct FBBBEquipmentStateAuthorityFactPacket;

/** 固定输入存储与来源检查 */
class FBBBRifleParseProcessor final
{
public:
    /** @param Context	本次更新上下文 @return 无 */
    static void Update(FBBBRifleUpdateContext &Context);

    /** @param Data	步枪运行时数据 @return 无 */
    static void Clear(FBBBRifleRuntimeData &Data);

    /**
     * 接收建立持有关系后的表现请求
     * @param Data		步枪运行时数据
     * @param bEquipped	是否为当前装备
     * @param Packet		待提交请求
     * @return 是否接受
     */
    static bool SubmitEquip(
        FBBBRifleRuntimeData &Data,
        bool bEquipped,
        bool bMirror,
        const FBBBEquipmentEquipLocalControlPacket &Packet);

    /**
     * 接收镜像装备表现请求
     * @param Data		步枪运行时数据
     * @param bEquipped	是否为当前装备
     * @param bMirror	是否只执行镜像还原
     * @param Packet		待提交请求
     * @return 是否接受
     */
    static bool SubmitEquip(
        FBBBRifleRuntimeData &Data,
        bool bEquipped,
        bool bMirror,
        const FBBBEquipmentEquipAuthorityFactPacket &Packet);

    /**
     * 接收本机控制端输入
     * @param Data		步枪运行时数据
     * @param bEquipped	是否为当前装备
     * @param bMirror	是否只执行镜像还原
     * @param Packet		待提交请求
     * @return 是否接受
     */
    template<typename TPacket>
    static bool SubmitLocal(
        FBBBRifleRuntimeData &Data,
        const bool bEquipped,
        const bool bMirror,
        const TPacket &Packet)
    {
        if (!bEquipped || bMirror)
        {
            return false;
        }

        BBBRifleInput::Submit(Data.Parse.InputState, Packet);
        return true;
    }

    /**
     * 校验并提交镜像当前结果
     * @param Data		步枪运行时数据
     * @param bEquipped	是否为当前装备
     * @param bMirror	是否只执行镜像还原
     * @param Payload		收到的网络数据
     * @return 是否接受
     */
    static bool SubmitMirror(
        FBBBRifleRuntimeData &Data,
        bool bEquipped,
        bool bMirror,
        const FBBBEquipmentStateAuthorityFactPacket &Payload);
};
