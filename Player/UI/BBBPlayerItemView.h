#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BBBPlayerItemView.generated.h"

class ABBBPlayerController;
class APawn;
class UFontFace;
class UBBBPlayerItemPortrait;
class SImage;
class SScrollBox;
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
    bool MoveItem(int32 Source, int32 Target, const APawn *SourcePawn, FGuid InstanceId);
    /** @return 当前界面所属玩家控制器 */
    ABBBPlayerController *GetItemController() const;
    /** @return 无 在数字键输入后短暂展示三个快捷格 */
    void NotifyQuickSelection();
    /** @param bHeld TAB 是否按住 @return 无 */
    void SetQuickBarHeld(bool bHeld);
    /** @param Slot 点击查看的物品位置 @return 无 */
    void InspectSlot(int32 Slot);
    /** @return 当前查看的物品位置 */
    int32 GetInspectedSlot() const;
    /** @return 当前是否处于穿戴页面 */
    bool IsGearPage() const;
    /** @return 快捷切换提示当前不透明度 */
    float GetQuickSelectionOpacity() const;

  protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual void NativeTick(const FGeometry &Geometry, float DeltaTime) override;
    virtual FReply NativeOnPreviewKeyDown(const FGeometry &Geometry, const FKeyEvent &KeyEvent) override;

  private:
    /** @return 无 只刷新详情展示 保持当前鼠标拖拽控件身份 */
    void RefreshDetail();
    /** 重新读取唯一背包并更新界面 */
    UFUNCTION()
    void RefreshItems();
    /** @return 本地背包布局 */
    TSharedRef<SWidget> MakeBackpack();
    /** @return 当前角色战斗数据的全视口覆层 */
    TSharedRef<SWidget> MakeGameplayHud();
    /** @return 当前选择物品的详情画刷 */
    const FSlateBrush *GetDetailBrush() const;
    /** @return 角色实时预览画刷 */
    const FSlateBrush *GetCharacterBrush() const;
    /** @param bMisc 是否展示杂物 @return 已处理 */
    FReply ShowStorage(bool bMisc);
    /** @param bGear 是否展示穿戴 @return 已处理 */
    FReply ShowGear(bool bGear);

    /** 字体引用同时承担打包依赖 */
    UPROPERTY()
    TObjectPtr<UFontFace> InterfaceFont;

    /** 两处格子均只引用同一个背包索引 */
    TSharedPtr<SHorizontalBox> QuickBar;
    TSharedPtr<SVerticalBox> BackpackSlots;
    TSharedPtr<SVerticalBox> EquipmentSlots;
    TSharedPtr<SBox> DetailImage;
    FSlateBrush DetailBrush;
    FSlateBrush CharacterBrush;
    /** 只复制实际显示结果的本地预览 */
    UPROPERTY(Transient)
    TObjectPtr<UBBBPlayerItemPortrait> CharacterPreview;
    int32 InspectedSlot = INDEX_NONE;
    FGuid InspectedInstance;
    bool bMiscPage = false;
    bool bGearPage = false;
    bool bQuickBarHeld = false;
    TSharedPtr<STextBlock> StatusText;
    /** 上次收到的操作结果版本 */
    int32 ObservedOperationRevision = 0;
    /** 上次实际选中格子 用于显示切换反馈 */
    int32 ObservedSelectedSlot = INDEX_NONE;
    /** 当前展示的角色 */
    TWeakObjectPtr<APawn> ObservedPawn;
    /** 最近数字选择的单调时钟时间 */
    double QuickSelectionTime = -100.0;
    /** 玩家背包打开状态 */
    bool bBackpackOpen = false;
};
