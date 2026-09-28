#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Styling/SlateBrush.h"
#include "BBBCharacterCustomizationView.generated.h"

class UBBBCharacterCustomizationSession;
class UTextureRenderTarget2D;
class UTexture2D;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class SBox;

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
    /** 构建指定一侧的穿戴与款式面板 */
    TSharedRef<SWidget> MakeSidePanel(bool bLeft);

    /** 按部位顺序构建当前穿戴图卡 */
    TSharedRef<SWidget> MakeSlotGrid(bool bLeft);

    /** 构建当前展开侧的部位切换条 */
    TSharedRef<SWidget> MakeSlotStrip(bool bLeft);

    /** 构建单个当前穿戴部位图卡 */
    TSharedRef<SWidget> MakeSlotCard(FName PartSlot);

    /** 构建指定部位的款式缩略图网格 */
    TSharedRef<SWidget> MakeItemGrid(FName PartSlot);

    /** 构建可直接选择的单个款式图卡 */
    TSharedRef<SWidget> MakeItemCard(FName PartSlot, FName ItemId);

    /** 创建身体或背心的徽章选择行 */
    TSharedRef<SWidget> MakePatchRow(FName PartSlot);

    /** 构建身体或背心的八乘八徽章网格 */
    TSharedRef<SWidget> MakePatchGrid(FName PartSlot);

    /** 构建可折叠的污渍与磨损控制 */
    TSharedRef<SWidget> MakeSurfaceControls();

    /** 展开指定部位的款式网格 */
    void OpenSlot(FName PartSlot);

    /** 展开指定部位的徽章网格 */
    void ShowPatchGrid(FName PartSlot);

    /** 收起当前部位选择网格 */
    void CloseSlot();

    /** 判断部位图卡属于左侧还是右侧 */
    bool IsLeftSideSlot(FName PartSlot) const;

    /** 读取面向玩家的款式显示名 */
    FText GetItemDisplayName(FName ItemId) const;

    /** 读取当前徽章索引 */
    int32 GetSelectedPatchIndex(FName PartSlot) const;

    /** 读取当前草稿中的款式行名 */
    FName GetSelectedItemId(FName PartSlot) const;

    /** 读取由界面持有的款式缩略图画刷 */
    const FSlateBrush *GetItemBrush(FName ItemId);

    /** 刷新所有当前穿戴部位的图像 */
    void RefreshSlotCardThumbnails();

    /** 将当前草稿的徽章坐标写入对应缩略图材质 */
    void UpdatePatchPreview(FName PartSlot);

    FText GetSelectedItem(FName PartSlot) const;

    UPROPERTY(Transient)
    TObjectPtr<UBBBCharacterCustomizationSession> Session;

    UPROPERTY(Transient)
    TObjectPtr<UTextureRenderTarget2D> PreviewTexture;

    FSlateBrush PreviewBrush;

    /** 保持徽章图集在 Slate 使用期间有效 */
    UPROPERTY(Transient)
    TObjectPtr<UTexture2D> PatchAtlas;

    /** 保持当前界面已读取的款式图像有效 */
    UPROPERTY(Transient)
    TMap<FName, TObjectPtr<UTexture2D>> ItemThumbnails;

    /** 由部位名称索引的当前穿戴缩略图控件 */
    TMap<FName, TSharedPtr<SBox>> SlotThumbnailBoxes;

    /** 维持 Slate 图像引用的画刷地址稳定 */
    TMap<FName, TSharedPtr<FSlateBrush>> ItemThumbnailBrushes;

    /** 由固定大小图集构建的徽章区域画刷 */
    TArray<FSlateBrush> PatchBrushes;

    /** 左侧当前款式面板 */
    TSharedPtr<SBox> LeftItemGrid;

    /** 右侧当前款式面板 */
    TSharedPtr<SBox> RightItemGrid;

    /** 左侧徽章图案容器 */
    TSharedPtr<SBox> LeftPatchGrid;

    /** 右侧徽章图案容器 */
    TSharedPtr<SBox> RightPatchGrid;

    /** 当前展开的部位名称 */
    FName ExpandedSlot;

    /** 鼠标悬停的部位 */
    FName HoveredPartSlot;

    /** 鼠标悬停的款式 */
    FName HoveredItemId;

    /** 当前是否显示徽章图案 */
    bool bShowingPatches = false;

    /** 当前是否展开表面控制 */
    bool bSurfaceExpanded = false;

    /** 最近一次操作的界面结果 */
    FText StatusMessage;

    /** 当前预览机位 */
    FName CurrentView = TEXT("Full");

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
