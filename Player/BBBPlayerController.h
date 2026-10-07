
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BBBPlayerController.generated.h"

class ABBBCharacter;
class UBBBEquipmentDefinition;
struct FBBBPlayerItemDisplayData;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBBBPlayerItemsChanged);
//封装InputMappingContext的数据与行为
class UInputMappingContext;
//封装InputAction的数据与行为
class UInputAction;
class UBBBPlayerInputSystem;
class UBBBPlayerItemView;

//将下方类型注册为受虚幻对象系统管理的类
UCLASS()
//封装ABBB玩家控制器的数据与行为
class ABBB_EVAC_API ABBBPlayerController : public APlayerController
{
    GENERATED_BODY()
public:

    /**
     * 创建并配置玩家控制器 加载默认输入映射并创建鼠标切换动作
     */
    ABBBPlayerController();

    /**
     * 进入游戏时为本地玩家注册默认与鼠标切换输入映射
     */
    virtual void BeginPlay() override;

    /**
     * 绑定鼠标切换输入动作到控制器回调
     */
    virtual void SetupInputComponent() override;

    /**
     * 切换鼠标指针与游戏输入模式
     */
    //让下方函数按照所列规则参与反射调用或远程调用
    UFUNCTION(BlueprintCallable, Category = "BBB|输入")
    void ToggleMouseCursor();

    /**
     * 转交本地换装界面的打开请求
     * @return 无
     */
    UFUNCTION(BlueprintCallable, Category = "BBB|客户端")
    void ToggleCustomization();

    /** @return 无 打开或关闭背包并维护输入模式 */
    UFUNCTION(BlueprintCallable, Category = "BBB|物品")
    void ToggleBackpack();

    /** @return 背包面板是否打开 */
    UFUNCTION(BlueprintPure, Category = "BBB|物品")
    bool IsBackpackOpen() const;

    /** @return 任一同级玩家页面是否打开 供游玩 HUD 控制可见性 */
    bool IsPlayerMenuOpen() const;

    /** @return 当前实际手持的物品 挂接失败或空手时返回空引用 */
    UFUNCTION(BlueprintPure, Category = "BBB|物品")
    AActor *GetActiveItem() const;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    /** @param DeltaTime	本帧时长 @return 无 观察物品结果变化并通知玩家 UI */
    virtual void PlayerTick(float DeltaTime) override;

    /** @return 当前玩家是否拥有可读取的真实背包 */
    UFUNCTION(BlueprintPure, Category = "BBB|物品")
    bool HasItemInventory() const;

    /** @return 背包槽位内容 包含空格 前序槽位与普通格子来自同一数组 */
    UFUNCTION(BlueprintPure, Category = "BBB|物品")
    TArray<AActor *> GetBackpackItems() const;

    /** @param Slot	背包索引 @return 物品的公共显示配置 空格返回空引用 */
    UFUNCTION(BlueprintPure, Category = "BBB|物品")
    UBBBEquipmentDefinition *GetItemDefinition(int32 Slot) const;

    /** @param Slot\t背包索引 @return 该格子的只读展示数据 */
    FBBBPlayerItemDisplayData GetItemDisplayData(int32 Slot) const;

    /** @return 实际手持装备的显示名称 空手时为空文本 */
    FText GetActiveItemName() const;

    /** @return 前序快捷槽位数量 */
    UFUNCTION(BlueprintPure, Category = "BBB|物品")
    int32 GetQuickAccessSlotCount() const;

    /** @return 当前选中的快捷槽位 未选择时返回 INDEX_NONE */
    UFUNCTION(BlueprintPure, Category = "BBB|物品")
    int32 GetSelectedItemSlot() const;

    /** @param EquipmentId	待获得装备定义 @return 输入是否接受 最终结果由操作完成状态提供 */
    UFUNCTION(BlueprintCallable, Category = "BBB|物品")
    bool SubmitItemAdd(FName EquipmentId);

    /** @param Source	起始槽位 @param Target	目标槽位 @return 输入是否接受 */
    UFUNCTION(BlueprintCallable, Category = "BBB|物品")
    bool SubmitItemMove(int32 Source, int32 Target);

    /** @param Slot	快捷索引 INDEX_NONE 表示取消选择 @return 输入是否接受 */
    UFUNCTION(BlueprintCallable, Category = "BBB|物品")
    bool SubmitItemSelect(int32 Slot);

    /**
     * 查询最近一次处理批次的结果
     * @param Revision	完成操作版本
     * @param SucceededCount	该批次成功操作数
     * @param RejectedCount	该批次失败操作数
     * @return 无
     */
    UFUNCTION(BlueprintPure, Category = "BBB|物品")
    void GetItemOperationResult(int32 &Revision, int32 &SucceededCount, int32 &RejectedCount) const;

    /** 背包内容 快捷选择 实际装备或操作完成结果改变时通知 */
    UPROPERTY(BlueprintAssignable, Category = "BBB|物品")
    FBBBPlayerItemsChanged OnItemsChanged;

protected:

    //让下方成员按照所列规则参与编辑序列化或网络复制
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|输入", meta = (DisplayName = "默认输入映射上下文"))
    //保存默认MappingContext供所属对象后续流程使用
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    //让下方成员按照所列规则参与编辑序列化或网络复制
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|输入", meta = (DisplayName = "映射上下文优先级"))
    //更新int32MappingContextPriority供后续步骤读取
    int32 MappingContextPriority = 0;
private:
    /** 本地玩家唯一物品界面 */
    UPROPERTY(Transient)
    TObjectPtr<UBBBPlayerItemView> ItemView;

    /** 背包关闭时恢复打开前的输入状态 */
    bool bBackpackPreviousCursor = false;
    bool bBackpackPreviousGameplayInput = true;

    /** @return 本机控制且已经初始化真实背包的角色 */
    ABBBCharacter *GetItemCharacter() const;

    /** 上次观察到的玩家角色 */
    TWeakObjectPtr<ABBBCharacter> ObservedItemCharacter;

    /** 上次观察到的实际主手物品 */
    TWeakObjectPtr<AActor> ObservedActiveItem;

    /** 上次通知 UI 的背包版本 */
    int32 ObservedInventoryRevision = INDEX_NONE;

    /** 上次通知 UI 的快捷选择版本 */
    int32 ObservedItemBarRevision = INDEX_NONE;

    /** 上次通知 UI 的操作完成版本 */
    int32 ObservedItemOperationRevision = INDEX_NONE;

    UPROPERTY(VisibleAnywhere, Category = "BBB|输入", meta = (DisplayName = "玩家输入系统"))
    TObjectPtr<UBBBPlayerInputSystem> PlayerInputSystem;


    //让下方成员按照所列规则参与编辑序列化或网络复制
    UPROPERTY(Transient)
    //保存ToggleMouseAction供所属对象后续流程使用
    TObjectPtr<UInputAction> ToggleMouseAction;

    //让下方成员按照所列规则参与编辑序列化或网络复制
    UPROPERTY(Transient)
    //保存ToggleMouseIMC供所属对象后续流程使用
    TObjectPtr<UInputMappingContext> ToggleMouseIMC;

    /**
     * 切换鼠标菜单输入模式
     * @param bEnabled	是否启用菜单模式
     */
    void SetMouseMenuMode(bool bEnabled);
};
