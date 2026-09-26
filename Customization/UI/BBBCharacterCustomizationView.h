#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Styling/SlateBrush.h"
#include "BBBCharacterCustomizationView.generated.h"

class UBBBCharacterCustomizationSession;
class UTextureRenderTarget2D;
class UMaterialInterface;
class UMaterialInstanceDynamic;

/**
 * 本地换装界面
 * 只显示独立世界的画面并转交操作
 */
UCLASS(Config = Game)
class ABBB_EVAC_API UBBBCharacterCustomizationView : public UUserWidget
{
    GENERATED_BODY()

public:
    /** 创建可接收取消键的界面 */
    UBBBCharacterCustomizationView(const FObjectInitializer &ObjectInitializer);

    /**
     * 绑定本地换装会话
     * @param InSession	当前会话
     * @return 无
     */
    void SetSession(UBBBCharacterCustomizationSession *InSession);

    /**
     * 显示独立世界的捕获画面
     * @param InTexture	预览渲染目标
     * @return 无
     */
    void SetPreviewTexture(UTextureRenderTarget2D *InTexture);

protected:
    /** @return 本地界面的 Slate 内容 */
    virtual TSharedRef<SWidget> RebuildWidget() override;

    /**
     * 处理换装界面的取消键
     * @param Geometry	界面布局
     * @param KeyEvent	按键事件
     * @return 是否消费输入
     */
    virtual FReply NativeOnKeyDown(const FGeometry &Geometry, const FKeyEvent &KeyEvent) override;

private:
    TSharedRef<SWidget> MakeSelectionRow(FName PartSlot);

    /** 创建身体或背心的徽章选择行 */
    TSharedRef<SWidget> MakePatchRow(FName PartSlot);

    /** 将当前草稿的徽章坐标写入对应缩略图材质 */
    void UpdatePatchPreview(FName PartSlot);

    FText GetSelectedItem(FName PartSlot) const;

    UPROPERTY(Transient)
    TObjectPtr<UBBBCharacterCustomizationSession> Session;

    UPROPERTY(Transient)
    TObjectPtr<UTextureRenderTarget2D> PreviewTexture;

    FSlateBrush PreviewBrush;

    /** 徽章图集的界面材质 */
    UPROPERTY(Config)
    TSoftObjectPtr<UMaterialInterface> PatchPreviewMaterial;

    UPROPERTY(Transient)
    TObjectPtr<UMaterialInstanceDynamic> BodyPatchMaterial;

    UPROPERTY(Transient)
    TObjectPtr<UMaterialInstanceDynamic> VestPatchMaterial;

    FSlateBrush BodyPatchBrush;

    FSlateBrush VestPatchBrush;
};
