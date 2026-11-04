#pragma once
#include "CoreMinimal.h"
#include "BBBWorldDropPresentationState.generated.h"
class UMaterialInstanceDynamic;
class UBBBItemDefinition;
struct FStreamableHandle;
/** 本地掉落物显示资源的独立生命周期 */
USTRUCT()
struct FBBBWorldDropPresentationState final
{
    GENERATED_BODY()
    /** 所属显示配置的异步资源与引用 保持到对象关闭 */
    TSharedPtr<FStreamableHandle> ResourceLoad;
    /** 最近请求的显示资源路径 */
    TArray<FSoftObjectPath> ResourcePaths;
    /** 区分未请求与空资源配置 */
    bool bResourcesRequested = false;
    /** 当前已解析的物品显示来源 */
    UPROPERTY()
    TObjectPtr<UBBBItemDefinition> RenderedDefinition = nullptr;
    /** 当前白色图标的动态材质 */
    UPROPERTY(Transient)
    TObjectPtr<UMaterialInstanceDynamic> IconMaterial = nullptr;
    /** 本地光束的淡入材质 */
    UPROPERTY(Transient)
    TObjectPtr<UMaterialInstanceDynamic> BeamMaterial = nullptr;
    /** 本地落点的淡入材质 */
    UPROPERTY(Transient)
    TObjectPtr<UMaterialInstanceDynamic> GroundGlowMaterial = nullptr;
    /** 最近一次开始显示的本地时间 */
    double VisibleSince = 0;
    /** 上次更新是否已显示掉落标记 */
    bool bWasVisible = false;
};
