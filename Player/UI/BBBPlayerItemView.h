#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Styling/SlateBrush.h"
#include "BBBPlayerItemView.generated.h"

class ABBBPlayerController;
class APawn;
class UFontFace;
class UTexture2D;
class SBox;
class SHorizontalBox;
class SVerticalBox;
class STextBlock;

/** 读取玩家物品结果并转交界面输入 */
UCLASS()
class ABBB_EVAC_API UBBBPlayerItemView final : public UUserWidget
{
    GENERATED_BODY()

public:
    UBBBPlayerItemView(const FObjectInitializer &ObjectInitializer);

    /** @return 当前背包面板是否打开 */
    bool IsBackpackOpen() const;

    /** @param bOpen	是否显示背包面板 @return 无 */
    void SetBackpackOpen(bool bOpen);

    /** @param Slot	待查看的背包索引 @return 无 */
    void InspectSlot(int32 Slot);

    /** @param Slot	快捷索引 INDEX_NONE 表示空手 @return 输入是否接受 */
    bool SelectSlot(int32 Slot);

    /**
     * 校验拖动开始时的身份并提交移动
     * @param Source	起始槽位
     * @param Target	目标槽位
     * @param SourcePawn	拖动开始时的玩家角色
     * @param SourceItem	拖动开始时的物品实例
     * @return 输入是否接受
     */
    bool MoveItem(int32 Source, int32 Target, const APawn *SourcePawn, const AActor *SourceItem);

    /** @return 当前界面所属玩家控制器 */
    ABBBPlayerController *GetItemController() const;

    /** @return 与换装页面共用的卡面纹理 */
    const FSlateBrush *GetCardSurfaceBrush() const { return &CardSurfaceBrush; }

    /** @return 与换装页面共用的卡框纹理 */
    const FSlateBrush *GetCardFrameBrush() const { return &CardFrameBrush; }

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual FReply NativeOnPreviewKeyDown(const FGeometry &Geometry, const FKeyEvent &KeyEvent) override;

private:
    /** 重新读取唯一背包并更新界面 */
    UFUNCTION()
    void RefreshItems();

    /** @return 本地背包布局 */
    TSharedRef<SWidget> MakeBackpack();

    /** @return 游玩界面的装备区 后续真实状态沿纵向布局追加 */
    TSharedRef<SWidget> MakeGameplayHud();

    /** @return 当前查看格子的物品详情布局 */
    TSharedRef<SWidget> MakeDetails();

    /** @return 实际手持装备的显示文本 */
    FText GetActiveItemText() const;

    /** @return 已占用槽位和总容量文本 */
    FText GetCapacityText() const;

    /** 字体引用同时承担打包依赖 */
    UPROPERTY()
    TObjectPtr<UFontFace> InterfaceFont;

    UPROPERTY()
    TObjectPtr<UTexture2D> CardSurfaceTexture;

    UPROPERTY()
    TObjectPtr<UTexture2D> CardFrameTexture;

    UPROPERTY(Transient)
    TObjectPtr<UTexture2D> DetailTexture;

    FSlateBrush CardSurfaceBrush;
    FSlateBrush CardFrameBrush;
    FSlateBrush DetailBrush;

    TSharedPtr<SHorizontalBox> QuickBar;
    TSharedPtr<SVerticalBox> BackpackSlots;
    TSharedPtr<STextBlock> DetailName;
    TSharedPtr<STextBlock> DetailDescription;
    TSharedPtr<STextBlock> StatusText;
    TSharedPtr<SBox> DetailArtwork;

    /** 仅保存界面查看位置 不作为快捷选择状态 */
    int32 InspectedSlot = INDEX_NONE;

    /** 避免重复显示已经处理的操作反馈 */
    int32 ObservedOperationRevision = 0;

    /** 用于换人后清理界面查看位置 */
    TWeakObjectPtr<APawn> ObservedPawn;

    bool bBackpackOpen = false;
};
