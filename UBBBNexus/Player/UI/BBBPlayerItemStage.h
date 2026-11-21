#pragma once

#include "GameFramework/Actor.h"
#include "BBBPlayerItemStage.generated.h"

class ASpotLight;
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

    UPROPERTY(EditInstanceOnly, Category="雾束", meta=(DisplayName="雾束密度", ClampMin="0"))
    float MistDensity = 0.0f;
    UPROPERTY(EditInstanceOnly, Category="雾束", meta=(DisplayName="外缘散射", ClampMin="0"))
    float MistHalo = 0.0f;
    UPROPERTY(EditInstanceOnly, Category="雾束", meta=(DisplayName="雾束内锥角", ClampMin="0", ClampMax="88"))
    float MistInnerAngle = 0.0f;
    UPROPERTY(EditInstanceOnly, Category="雾束", meta=(DisplayName="雾束外锥角", ClampMin="1", ClampMax="89"))
    float MistOuterAngle = 1.0f;
};
