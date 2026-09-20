#pragma once

#include "CoreMinimal.h"

class ABBBCharacter;
class ABBBEquipment;
class USkeletalMeshComponent;

/**
 * 装备签名解析期间可以读取的单帧外部上下文
 *
 * 本结构不保存网络身份和跨帧数据。输入自身携带执行模式，防止不同来源的输入
 * 被装备演员上的全局标记错误合并。
 */
struct FBBBEquipmentRuntimeContext
{
    /** 当前装备演员 */
    ABBBEquipment &Equipment;

    /** 拥有当前装备的角色 */
    ABBBCharacter &Character;

    /** 当前装备的骨骼网格 */
    USkeletalMeshComponent &WeaponMesh;

    /** 当前帧间隔 */
    float DeltaSeconds = 0.0f;
};
