#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "BBBWork/UBBBNexus/Appearance/Data/BBBAppearanceSelection.h"
#include "BBBClientSubsystem.generated.h"

class APawn;
class UBBBClientSaveGame;
class UBBBCharacterCustomizationController;

/** 本地玩家配置与换装入口 不依赖具体人物控制器类型 */
UCLASS(Config = Game)
class ABBB_EVAC_API UBBBClientSubsystem : public ULocalPlayerSubsystem
{
    GENERATED_BODY()

public:
    /**
     * 读取本地配置
     * @param Collection	子系统集合
     * @return 无
     */
    virtual void Initialize(FSubsystemCollectionBase &Collection) override;

    /** @return 无 */
    virtual void Deinitialize() override;

    /**
     * 重新绑定本地控制器的换人通知
     * @param NewPlayerController	新的本地控制器
     * @return 无
     */
    virtual void PlayerControllerChanged(APlayerController *NewPlayerController) override;

    /**
     * 保存已经应用的外观
     * @param Selection	完整外观组合
     * @return 是否写入成功
     */
    bool SaveAppearance(const FBBBAppearanceSelection &Selection);

    /**
     * 打开或关闭本地换装界面
     * @return 无
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|Client")
    void ToggleCustomization();

private:
    /**
     * 人物重建后恢复本地选择
     * @param OldPawn	旧人物
     * @param NewPawn	新人物
     * @return 无
     */
    UFUNCTION()
    void OnPawnChanged(APawn *OldPawn, APawn *NewPawn);

    /** 换装主控蓝图类 */
    UPROPERTY(Config)
    TSoftClassPtr<UBBBCharacterCustomizationController> CustomizationClass;

    /** 本地配置对象 */
    UPROPERTY(Transient)
    TObjectPtr<UBBBClientSaveGame> Settings;

    /** 本地玩家持有的换装主控 */
    UPROPERTY(Transient)
    TObjectPtr<UBBBCharacterCustomizationController> Customization;

    /** 不延长旧控制器的生命周期 */
    TWeakObjectPtr<APlayerController> Controller;
};
