#pragma once

#include "GameFramework/Actor.h"
#include "BBBPlayerItemStage.generated.h"

class ASpotLight;
class UAnimSequence;
class USceneCaptureComponent2D;
class USpotLightComponent;
class UStaticMeshComponent;

/** 整备间关卡摆件 持有可编辑的布景 灯光 雾束与机位 */
UCLASS(NotBlueprintable)
class ABBB_EVAC_API ABBBPlayerItemStage final : public AActor
{
    GENERATED_BODY()

public:
    ABBBPlayerItemStage();
    virtual void OnConstruction(const FTransform& Transform) override;
    virtual void PostRegisterAllComponents() override;

    /** @return 人物脚下的预览锚点 */
    const USceneComponent& GetCharacterAnchor() const { return *CharacterAnchor; }
    /** @return 鼠标视差的机位环绕锚点 */
    const USceneComponent& GetOrbitAnchor() const { return *OrbitAnchor; }
    /** @return 关卡内配置好的背包捕获机位 */
    USceneCaptureComponent2D* GetCapture() const { return Capture; }
    /** @return 决定暖雾方向和亮度的主灯 */
    USpotLightComponent* GetKey() const;
    /** @return 当前的暖雾散射体积 */
    UStaticMeshComponent* GetMist() const { return Mist; }
    /** @return 持枪预览播放的角色动作 */
    UAnimSequence* GetArmedIdle() const { return ArmedIdle; }
    /** @return 空手预览播放的站立动作 */
    UAnimSequence* GetUnarmedIdle() const { return UnarmedIdle; }
    /** @return 顶栏相对静止镜头的深度与位置微调 */
    FVector GetNavigationOffset() const { return NavigationOffset; }
    /** @return 左右面板视差的倍率 */
    float GetPanelParallaxScale() const { return PanelParallaxScale; }
    /** @return 格框白光最内层的不透明度 */
    float GetSlotGlowOpacity() const { return SlotGlowOpacity; }
    /** @return 格子悬停亮起所用时间 */
    float GetSlotHoverSeconds() const { return SlotHoverSeconds; }
    /** 同步体积雾材质的灯位和散射参数 @return 无 */
    void UpdateMist() const;

private:
    /** 刷新材质参数并将当前机位写入编辑器预览目标 */
    UFUNCTION(CallInEditor, Category="Inventory Studio")
    void RefreshEditorPreview();

    UPROPERTY(VisibleAnywhere, Category="Inventory Studio")
    TObjectPtr<UStaticMeshComponent> Floor;
    UPROPERTY(EditInstanceOnly, Category="雾束", meta=(DisplayName="暖主光引用", ToolTip="选择决定暖雾方向与亮度的关卡聚光灯"))
    TObjectPtr<ASpotLight> MistKeyLight;
    UPROPERTY(VisibleAnywhere, Category="Inventory Studio")
    TObjectPtr<UStaticMeshComponent> Mist;
    UPROPERTY(VisibleAnywhere, Category="Inventory Studio")
    TObjectPtr<USceneCaptureComponent2D> Capture;
    UPROPERTY(VisibleAnywhere, Category="Inventory Studio")
    TObjectPtr<USceneComponent> CharacterAnchor;
    UPROPERTY(VisibleAnywhere, Category="Inventory Studio")
    TObjectPtr<USceneComponent> OrbitAnchor;
    UPROPERTY(EditInstanceOnly, Category="背包人物", meta=(DisplayName="持枪待机动作"))
    TObjectPtr<UAnimSequence> ArmedIdle;
    UPROPERTY(EditInstanceOnly, Category="背包人物", meta=(DisplayName="空手待机动作"))
    TObjectPtr<UAnimSequence> UnarmedIdle;

    UPROPERTY(EditInstanceOnly, Category="雾束", meta=(DisplayName="雾束密度", ClampMin="0"))
    float MistDensity = 0.0f;
    UPROPERTY(EditInstanceOnly, Category="雾束", meta=(DisplayName="外缘散射", ClampMin="0"))
    float MistHalo = 0.0f;
    UPROPERTY(EditInstanceOnly, Category="雾束", meta=(DisplayName="雾束内锥角", ClampMin="0", ClampMax="88"))
    float MistInnerAngle = 0.0f;
    UPROPERTY(EditInstanceOnly, Category="雾束", meta=(DisplayName="雾束外锥角", ClampMin="1", ClampMax="89"))
    float MistOuterAngle = 1.0f;

    UPROPERTY(EditInstanceOnly, Category="背包界面", meta=(DisplayName="面板鼠标视差", ClampMin="0", ClampMax="2"))
    float PanelParallaxScale = 0.8f;
    UPROPERTY(EditInstanceOnly, Category="背包界面", meta=(DisplayName="顶部页签偏移",
        ToolTip="单位厘米 X 沿静止镜头向前 Z 向上 Y 随人物中心自动对齐"))
    FVector NavigationOffset = FVector::ZeroVector;
    UPROPERTY(EditInstanceOnly, Category="背包界面", meta=(DisplayName="格框白光强度", ClampMin="0", ClampMax="0.25"))
    float SlotGlowOpacity = 0.065f;
    UPROPERTY(EditInstanceOnly, Category="背包界面", meta=(DisplayName="格框悬停时长", ClampMin="0.02", ClampMax="0.5"))
    float SlotHoverSeconds = 0.075f;
};
