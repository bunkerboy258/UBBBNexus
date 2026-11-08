#pragma once

#include "Components/AudioComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehavior.h"
#include "BBBMonsterSoundPresentationComponent.generated.h"

class UBBBMonsterSoundPresentationDefinition;
struct FBBBMonsterHitReactionFragment;

/** 由 Mass 批量调用的单声道发声桥接 不维护第二套玩法状态 */
UCLASS(ClassGroup = "Monster", meta = (BlueprintSpawnableComponent))
class ABBB_EVAC_API UBBBMonsterSoundPresentationComponent final : public UAudioComponent
{
    GENERATED_BODY()

public:
    /** @return 创建不自动播放且不独立更新的声音组件 */
    UBBBMonsterSoundPresentationComponent();

    /**
     * 消费当前有效事实 不排队 不重播历史
     * @param Instance	当前实体的跨机出生身份
     * @param Settings	静态发声配置
     * @param State	当前行为事实
     * @param ActionId	当前动作编号
     * @param ActionProgress	当前动作归一化进度
     * @param Hit	最近成立命中
     * @param bCrawling	持续爬行事实
     * @param Speed	当前水平速度
     * @param Now	当前世界时间
     * @param bNewActor	表现角色是否刚交接
     * @return 无返回值
     */
    void ApplyFacts(const FGuid& Instance, UBBBMonsterSoundPresentationDefinition* Settings,
        EBBBMonsterBehavior State, uint32 ActionId, float ActionProgress,
        const FBBBMonsterHitReactionFragment& Hit, bool bCrawling, float Speed, float Now, bool bNewActor);

    /** @return 停止声音并清空上一代实体的全部播放状态 */
    void ResetPresentation();

    /**
     * @param Audio	独立接触声道
     * @param bGrounded	Mass 当前地面支撑结果
     * @param bCrawling	当前姿态
     * @param Speed	真实水平速度
     * @param Now	当前时间
     * @param bNewActor	是否刚交接
     * @return 无
     */
    void ApplyContactFacts(UAudioComponent& Audio, bool bGrounded, bool bCrawling, float Speed, float Now, bool bNewActor);

    /** @return 当前实体固定声线下标 */
    int32 GetVoiceIndex() const { return VoiceIndex; }

    /** @return 最近消费的命中编号 */
    uint32 GetObservedHitSerial() const { return ObservedHitSerial; }

    /** @return 去重后产生的声音请求数 包含听觉范围外丢弃项 */
    uint32 GetRequestedVoiceCount() const { return RequestedVoiceCount; }

    /** @return 音频组件实际进入播放的次数 不证明扬声器输出 */
    uint32 GetStartedVoiceCount() const { return StartedVoiceCount; }

private:
    /** 仅本地声音节奏的当前累计路程 */
    float ContactTravel = 0.0f;

    /** 当前接触采样时间 */
    float ContactTime = -1.0f;

    /** 上次支撑事实 用于单次落地声 */
    bool bContactGrounded = true;

    /** 当前角色租约对应的实体身份 */
    FGuid BoundInstance;

    /** 仅持有当前静态配置 不保存玩法状态 */
    UPROPERTY(Transient)
    TObjectPtr<UBBBMonsterSoundPresentationDefinition> BoundSettings;

    /** 当前固定声线 */
    UPROPERTY(Transient, VisibleInstanceOnly, Category = "小怪|声音", meta = (DisplayName = "声线下标"))
    int32 VoiceIndex = INDEX_NONE;

    /** 最新已经消费的命中编号 */
    UPROPERTY(Transient, VisibleInstanceOnly, Category = "小怪|声音", meta = (DisplayName = "已消费命中"))
    uint32 ObservedHitSerial = 0;

    /** 最新已经消费的动作编号 */
    UPROPERTY(Transient, VisibleInstanceOnly, Category = "小怪|声音", meta = (DisplayName = "已消费动作"))
    uint32 ObservedActionId = 0;

    /** 声音决策诊断 不保留声音请求历史 */
    UPROPERTY(Transient, VisibleInstanceOnly, Category = "小怪|声音", meta = (DisplayName = "声音请求数"))
    uint32 RequestedVoiceCount = 0;

    /** 音频设备已接受的播放次数 */
    UPROPERTY(Transient, VisibleInstanceOnly, Category = "小怪|声音", meta = (DisplayName = "已启动播放数"))
    uint32 StartedVoiceCount = 0;

    /** 最近声音事件的诊断名称 */
    UPROPERTY(Transient, VisibleInstanceOnly, Category = "小怪|声音", meta = (DisplayName = "最近声音事件"))
    FName LastVoiceEvent;

    /** 上一帧的行为快照 只用于表现去重 */
    EBBBMonsterBehavior ObservedState = EBBBMonsterBehavior::Idle;

    /** 当前租约已经消费死亡表现 */
    bool bDeathConsumed = false;

    /** 上一帧的爬行快照 */
    bool bObservedCrawling = false;

    /** 下一次允许间歇发声的时间 */
    float NextAmbientAt = 0.0f;

    /** 下一次允许受击发声的时间 */
    float NextHitAt = 0.0f;

    /** 当前实体固定音调 */
    float EntityPitch = 1.0f;

    /** 本地表现随机流 不影响玩法随机 */
    FRandomStream Random;

    /** 避免连续重复相同素材 */
    UPROPERTY(Transient)
    TObjectPtr<USoundBase> PreviousSound;

    /**
     * @param Pool	当前声音池
     * @param Event	诊断事件
     * @param bAction	是否使用优先动作预算
     * @return 无返回值
     */
    void RequestVoice(const TArray<TObjectPtr<USoundBase>>& Pool, FName Event, bool bAction);
};
