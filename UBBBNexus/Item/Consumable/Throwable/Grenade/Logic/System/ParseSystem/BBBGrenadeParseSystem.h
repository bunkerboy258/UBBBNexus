#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Input/LocalControl/Use/FBBBGrenadePrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Input/LocalControl/Use/FBBBGrenadeFinishLocalControlPacket.h"
struct FBBBGrenadeRuntimeData;
/** 固定槽位解析调度 */
class FBBBGrenadeParseSystem final
{
  public:
    /** @param InData 唯一黑板 @return 无 */
    void Initialize(FBBBGrenadeRuntimeData &InData);
    /** @return 无 调度解析 */
    void Update();
    /** @return 无 解除依赖 */
    void Shutdown();
    /** @param Packet 动作请求 @return 无 */
    void Submit(FBBBGrenadePrimaryLocalControlPacket Packet);
    /** @param Packet 结束请求 @return 无 */
    void Submit(FBBBGrenadeFinishLocalControlPacket Packet);

  private:
    FBBBGrenadeRuntimeData *Data = nullptr;
};
