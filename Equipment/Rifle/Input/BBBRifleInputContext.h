#pragma once

#include "CoreMinimal.h"

class ABBBCharacter;
class ABBBRifleEquipment;
class UAnimMontage;
class UBBBRifleDefinition;
class USkeletalMeshComponent;
class UWorld;
struct FBBBRifleRuntimeData;

/** 步枪输入解析期间使用的单帧上下文 */
struct FBBBRifleInputContext final
{
    /** 当前步枪演员 */
    ABBBRifleEquipment &Equipment;

    /** 当前装备持有角色 */
    ABBBCharacter &Character;

    /** 当前步枪骨骼网格 */
    USkeletalMeshComponent &WeaponMesh;

    /** 当前步枪静态配置 */
    const UBBBRifleDefinition &Definition;

    /** 当前步枪运行时事实 */
    FBBBRifleRuntimeData &RuntimeData;

    /** 当前帧间隔 */
    float DeltaSeconds = 0.0f;

    /** @return 当前世界 */
    UWorld *GetWorld() const;

    /**
     * 向角色蒙太奇槽提交表现输入
     * @param Montage       待播放蒙太奇
     * @param Sequence      动作序号
     * @param bReload       是否属于换弹流程
     * @return 无
     */
    void SubmitMontage(UAnimMontage *Montage, int32 Sequence, bool bReload) const;

    /**
     * 播放不产生玩法结果的开火表现
     * @param Sequence      开火序号
     * @return 无
     */
    void PlayFirePresentation(int32 Sequence) const;
};
