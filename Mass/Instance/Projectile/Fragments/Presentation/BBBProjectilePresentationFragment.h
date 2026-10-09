#pragma once

#include "MassEntityTypes.h"
#include "BBBProjectilePresentationFragment.generated.h"

class UNiagaraDataChannelAsset;
class UNiagaraSystem;
class UStaticMesh;

/** 子弹批量表现通道 */
USTRUCT()
struct ABBB_EVAC_API FBBBProjectilePresentationFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 批量显示的弹体网格 */
    UPROPERTY()
    TWeakObjectPtr<UStaticMesh> Mesh;

    /** 弹体网格的局部变换 */
    FTransform MeshRelativeTransform = FTransform::Identity;

    /** 批量光效通道 */
    UPROPERTY()
    TWeakObjectPtr<UNiagaraDataChannelAsset> Channel;

    /** 当前实体使用的共享光效系统 */
    UPROPERTY()
    TWeakObjectPtr<UNiagaraSystem> System;

    /** 仅提交本次碰撞表面事实的批量通道 */
    UPROPERTY()
    TWeakObjectPtr<UNiagaraDataChannelAsset> ImpactChannel;

    /** 当前实体的飞行光段长度 */
    float LengthCm = 0.0f;

    /** 当前实体的飞行光段宽度 */
    float WidthCm = 0.0f;

    /** 当前实体的飞行光段颜色 */
    FLinearColor Color = FLinearColor::White;

    /** 共享 Niagara 数据中的表现槽位 */
    int32 Slot = INDEX_NONE;

    /** 新槽位仅在首帧请求生成光段 */
    bool bSpawnPending = false;

    /** 仅供光效读取的当前存活标记 */
    bool bVisualAlive = false;

};
