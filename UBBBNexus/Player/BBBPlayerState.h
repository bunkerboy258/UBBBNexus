
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BBBPlayerKillCounts.h"
#include "BBBPlayerState.generated.h"

class USoundBase;

//将下方类型注册为受虚幻对象系统管理的类
UCLASS()
//封装ABBB玩家状态的数据与行为
class ABBB_EVAC_API ABBBPlayerState : public APlayerState
{
    GENERATED_BODY()
public:

    /**
     * 创建并配置玩家状态 设置网络更新频率
     */
    ABBBPlayerState();

    /**
     * 进入游戏时建立运行依赖
     */
    virtual void BeginPlay() override;

    /** @return 当前玩家本局的击杀数量 */
    UFUNCTION(BlueprintPure, Category = "Combat")
    FBBBPlayerKillCounts GetKillCounts() const { return KillCounts; }

    /** @return 当前玩家阵营标识 PLA 或 UA_BBC */
    UFUNCTION(BlueprintPure, Category = "Profile")
    FName GetFactionName() const { return FactionName; }

    /**
     * 设置玩家阵营标识 仅权威端有效
     * @param InFactionName	阵营标识
     * @return 无
     */
    void SetFactionName(FName InFactionName);

    /**
     * @param ConfirmedKills	本轮新成立的两类击杀数量
     * @return 无
     */
    void AddConfirmedKills(const FBBBPlayerKillCounts& ConfirmedKills);

    //~ Begin AActor Interface
    virtual void PostNetInit() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    //~ End AActor Interface

protected:
    //~ Begin APlayerState Interface
    virtual void CopyProperties(APlayerState* PlayerState) override;
    //~ End APlayerState Interface

private:
    /** 还原拥有者的最新计数并反馈新击杀 */
    UFUNCTION()
    void OnRep_KillCounts(const FBBBPlayerKillCounts& Previous);

    /** 向本地玩家播放确认音 */
    void PlayKillConfirmation() const;

    /** 房主维护并同步给本人的最新计数 */
    UPROPERTY(ReplicatedUsing = OnRep_KillCounts)
    FBBBPlayerKillCounts KillCounts;

    /** 房主维护并同步给全队的玩家阵营标识 */
    UPROPERTY(Replicated)
    FName FactionName = TEXT("PLA");

    /** 击杀确认使用的单次提示音 */
    UPROPERTY()
    TObjectPtr<USoundBase> KillConfirmationSound;

    /** 初始网络快照已建立 */
    bool bKillCountsInitialized = false;
};
