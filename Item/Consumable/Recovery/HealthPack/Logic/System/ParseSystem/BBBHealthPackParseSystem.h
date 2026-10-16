#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Input/LocalControl/Use/FBBBHealthPackPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Input/LocalControl/Use/FBBBHealthPackFinishLocalControlPacket.h"
struct FBBBHealthPackRuntimeData;
/** 固定槽位解析调度 */
class FBBBHealthPackParseSystem final
{
  public:
    /** @param InData 唯一黑板 @return 无 */
    void Initialize(FBBBHealthPackRuntimeData &InData);
    /** @return 无 调度解析 */
    void Update();
    /** @return 无 解除依赖 */
    void Shutdown();
    /** @param Packet 动作请求 @return 无 */
    void Submit(FBBBHealthPackPrimaryLocalControlPacket Packet);
    /** @param Packet 结束请求 @return 无 */
    void Submit(FBBBHealthPackFinishLocalControlPacket Packet);

  private:
    FBBBHealthPackRuntimeData *Data = nullptr;
};
