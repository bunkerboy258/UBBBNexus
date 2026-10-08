
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerItemDisplayData.h"
#include "BBBWork/UBBBNexus/Client/BBBClientSubsystem.h"
#include "BBBWork/UBBBNexus/PlayerInput/BBBPlayerInputSystem.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemView.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Item/FBBBItemAddLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Item/FBBBItemMoveLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Item/FBBBItemSelectLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Config/BBBEquipmentDefinition.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "UObject/ConstructorHelpers.h"
//定义当前编译单元使用的日志分类
// 声明控制器专用日志分类
DEFINE_LOG_CATEGORY_STATIC(LogBBBPlayerController, Log, All);

//创建并配置ABBB玩家控制器
ABBBPlayerController::ABBBPlayerController()
{
    //允许控制器参与服务器到客户端的属性复制
    bReplicates = true;
    PlayerInputSystem = CreateDefaultSubobject<UBBBPlayerInputSystem>(TEXT("PlayerInputSystem"));
    //在类默认对象构造期间加载所需资源
    static ConstructorHelpers::FObjectFinder<UInputMappingContext> DefaultMappingContextAsset(TEXT("/Game/_Project/Input/IMC_Default.IMC_Default"));
    //资源加载成功后缓存角色使用的默认输入映射
    if (DefaultMappingContextAsset.Succeeded())
    {
        //保存默认映射供本地玩家进入游戏时注册
        DefaultMappingContext = DefaultMappingContextAsset.Object;
    }
}

//绑定玩家输入动作
void ABBBPlayerController::SetupInputComponent()
{
    //先执行父类的绑定玩家输入动作
    Super::SetupInputComponent();

    InputComponent->BindKey(EKeys::F6, IE_Pressed, this, &ABBBPlayerController::ToggleCustomization);
    InputComponent->BindKey(EKeys::Tab, IE_Pressed, this, &ABBBPlayerController::ToggleBackpack);

    // 每个运行时控制器独立创建菜单动作 避免构造阶段对象被蓝图默认值覆盖
    //创建不依赖资产文件的鼠标模式切换动作
    ToggleMouseAction = NewObject<UInputAction>(this, TEXT("IA_ToggleMouse"));
    //动作创建成功后声明其输入值为布尔触发
    if (ToggleMouseAction)
    {
        //让动作只表达按下或未按下两种状态
        ToggleMouseAction->ValueType = EInputActionValueType::Boolean;
    }
    //创建专用于鼠标模式切换的运行时映射上下文
    ToggleMouseIMC = NewObject<UInputMappingContext>(this, TEXT("IMC_ToggleMouse"));
    //映射与动作均有效时将 Escape 键绑定到切换动作
    if (ToggleMouseIMC && ToggleMouseAction)
    {
        //把鼠标切换动作映射到指定按键
        ToggleMouseIMC->MapKey(ToggleMouseAction, EKeys::Escape);
    }

    //将控制器输入组件转换为增强输入组件
    UEnhancedInputComponent *EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
    //EnhancedInput与ToggleMouseAction不满足时停止当前绑定玩家输入动作流程
    if (!ensureMsgf(EnhancedInput && PlayerInputSystem,
        TEXT("[BBBInput]增强输入组件或玩家输入组件缺失")))
    {
        //结束当前绑定玩家输入动作流程
        return;
    }
    PlayerInputSystem->Bind(*EnhancedInput);
    UE_LOG(LogBBBPlayerController, Log,
        TEXT("[BBBInput]输入绑定完成 Controller=%s Bindings=%d ToggleMouseAction=%s"),
        *GetName(), EnhancedInput->GetActionEventBindings().Num(), *GetNameSafe(ToggleMouseAction));

    // 鼠标菜单动作缺失不能阻断角色移动与视角输入
    if (!ensureMsgf(ToggleMouseAction && ToggleMouseIMC, TEXT("[BBBInput]鼠标切换动作或映射缺失")))
    {
        return;
    }

    //把输入动作绑定到对应的控制器回调
    EnhancedInput->BindAction(ToggleMouseAction, ETriggerEvent::Started, this, &ABBBPlayerController::ToggleMouseCursor);
}

//进入游戏时建立运行依赖
void ABBBPlayerController::BeginPlay()
{
    //先执行父类的进入游戏时建立运行依赖
    Super::BeginPlay();
    //前置条件不满足时停止当前进入游戏时建立运行依赖流程
    if (!IsLocalController())
    {
        //结束当前进入游戏时建立运行依赖流程
        return;
    }
    //获取本控制器对应的本地玩家对象
    ULocalPlayer *LocalPlayer = GetLocalPlayer();
    //缺少本地玩家时无法访问增强输入子系统
    if (!LocalPlayer)
    {
        //结束当前读取Local玩家流程
        return;
    }
    ItemView = CreateWidget<UBBBPlayerItemView>(this, UBBBPlayerItemView::StaticClass());
    if (ensureMsgf(ItemView, TEXT("[BBBItems]玩家物品界面创建失败")))
    {
        ItemView->AddToPlayerScreen();
    }
    //获取负责管理本地输入映射上下文的增强输入子系统
    UEnhancedInputLocalPlayerSubsystem *Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    //增强输入子系统未建立时停止注册映射
    if (!Subsystem)
    {
        //结束当前读取Local玩家流程
        return;
    }
    //默认映射存在时注册角色基础输入
    if (DefaultMappingContext)
    {
        //按配置优先级启用默认输入映射
        Subsystem->AddMappingContext(DefaultMappingContext, MappingContextPriority);
    }
    //鼠标切换映射存在时以更高优先级注册
    if (ToggleMouseIMC)
    {
        //确保 Escape 切换不会被默认映射覆盖
        Subsystem->AddMappingContext(ToggleMouseIMC, MappingContextPriority + 1);
    }
}

//切换鼠标指针与游戏输入模式
void ABBBPlayerController::ToggleMouseCursor()
{
    //前置条件不满足时停止当前切换鼠标指针与游戏输入模式流程
    if (!IsLocalController())
    {
        //结束当前切换鼠标指针与游戏输入模式流程
        return;
    }
    if (IsBackpackOpen())
    {
        ToggleBackpack();
        return;
    }
    //切换鼠标菜单输入模式
    SetMouseMenuMode(!bShowMouseCursor);
}

void ABBBPlayerController::ToggleCustomization()
{
    if (IsBackpackOpen())
    {
        ToggleBackpack();
    }
    if (ULocalPlayer *LocalPlayer = GetLocalPlayer())
    {
        LocalPlayer->GetSubsystem<UBBBClientSubsystem>()->ToggleCustomization();
    }
}

bool ABBBPlayerController::IsBackpackOpen() const
{
    return ItemView && ItemView->IsBackpackOpen();
}

bool ABBBPlayerController::IsPlayerMenuOpen() const
{
    const ULocalPlayer *LocalPlayer = GetLocalPlayer();
    const UBBBClientSubsystem *Client = LocalPlayer ? LocalPlayer->GetSubsystem<UBBBClientSubsystem>() : nullptr;
    return IsBackpackOpen() || (Client && Client->IsCustomizationOpen());
}

void ABBBPlayerController::ToggleBackpack()
{
    if (!IsLocalController() || !ItemView)
    {
        return;
    }
    if (IsBackpackOpen())
    {
        ItemView->SetBackpackOpen(false);
        SetMouseMenuMode(bBackpackPreviousCursor);
        PlayerInputSystem->SetInputEnabled(bBackpackPreviousGameplayInput);
        return;
    }
    if (!HasItemInventory())
    {
        UE_LOG(LogBBBPlayerController, Warning, TEXT("[BBBItems]当前无法打开背包 Controller=%s"), *GetName());
        return;
    }
    UBBBClientSubsystem *Client = GetLocalPlayer()->GetSubsystem<UBBBClientSubsystem>();
    if (Client->IsCustomizationOpen())
    {
        Client->ToggleCustomization();
    }
    bBackpackPreviousCursor = bShowMouseCursor;
    bBackpackPreviousGameplayInput = PlayerInputSystem->IsInputEnabled();
    SetMouseMenuMode(true);
    ItemView->SetBackpackOpen(true);
    FInputModeUIOnly InputMode;
    InputMode.SetWidgetToFocus(ItemView->TakeWidget());
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(InputMode);
}

void ABBBPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (ItemView)
    {
        ItemView->RemoveFromParent();
        ItemView = nullptr;
    }
    Super::EndPlay(EndPlayReason);
}

AActor *ABBBPlayerController::GetActiveItem() const
{
    const ABBBCharacter *ItemCharacter = GetItemCharacter();
    return ItemCharacter ? ItemCharacter->GetActiveEquipment() : nullptr;
}

//切换鼠标菜单输入模式
void ABBBPlayerController::SetMouseMenuMode(bool bEnabled)
{
    PlayerInputSystem->SetInputEnabled(!bEnabled);
    //启用菜单模式时同时保留界面与游戏输入
    if (bEnabled)
    {
        //创建游戏与界面共用的输入模式
        FInputModeGameAndUI InputMode;
        //鼠标被视口捕获时仍保持指针可见
        InputMode.SetHideCursorDuringCapture(false);
        //应用界面可交互的输入模式
        SetInputMode(InputMode);
        //显示鼠标指针供菜单操作
        bShowMouseCursor = true;
        //结束当前设置InputMode流程
        return;
    }
    //关闭菜单后恢复纯游戏输入
    SetInputMode(FInputModeGameOnly());
    //隐藏鼠标指针避免影响瞄准
    bShowMouseCursor = false;
}

ABBBCharacter *ABBBPlayerController::GetItemCharacter() const
{
    ABBBCharacter *ItemCharacter = Cast<ABBBCharacter>(GetPawn());
    if (!IsLocalController() || !IsValid(ItemCharacter) || !ItemCharacter->IsLocallyControlled())
    {
        return nullptr;
    }
    return ItemCharacter;
}

bool ABBBPlayerController::HasItemInventory() const
{
    const ABBBCharacter *ItemCharacter = GetItemCharacter();
    return ItemCharacter && !ItemCharacter->RuntimeData.Item.ReadItemInventoryState().BackpackSlots.IsEmpty();
}

TArray<AActor *> ABBBPlayerController::GetBackpackItems() const
{
    TArray<AActor *> Items;
    if (const ABBBCharacter *ItemCharacter = GetItemCharacter())
    {
        for (const auto &Item : ItemCharacter->RuntimeData.Item.ReadItemInventoryState().BackpackSlots)
        {
            Items.Add(IsValid(Item.ItemActor.Get()) ? Item.ItemActor.Get() : nullptr);
        }
    }
    return Items;
}

UBBBEquipmentDefinition *ABBBPlayerController::GetItemDefinition(const int32 Slot) const
{
    const ABBBCharacter *ItemCharacter = GetItemCharacter();
    if (!ItemCharacter)
    {
        return nullptr;
    }
    const auto &Slots = ItemCharacter->RuntimeData.Item.ReadItemInventoryState().BackpackSlots;
    if (!Slots.IsValidIndex(Slot))
    {
        return nullptr;
    }
    const ABBBEquipment *Equipment = Cast<ABBBEquipment>(Slots[Slot].ItemActor.Get());
    return IsValid(Equipment) ? Equipment->GetDefinition() : nullptr;
}

int32 ABBBPlayerController::GetQuickAccessSlotCount() const
{
    const ABBBCharacter *ItemCharacter = GetItemCharacter();
    return ItemCharacter ? ItemCharacter->RuntimeData.Item.ReadItemBarState().QuickAccessSlotCount : 0;
}

FBBBPlayerItemDisplayData ABBBPlayerController::GetItemDisplayData(const int32 Slot) const
{
    FBBBPlayerItemDisplayData Data;
    const TArray<AActor *> Items = GetBackpackItems();
    Data.bOccupied = Items.IsValidIndex(Slot) && IsValid(Items[Slot]);
    Data.bQuick = Slot >= 0 && Slot < GetQuickAccessSlotCount();
    Data.bSelected = Slot >= 0 && Slot == GetSelectedItemSlot();
    Data.bActive = Data.bOccupied && Items[Slot] == GetActiveItem();
    if (const UBBBEquipmentDefinition *Definition = GetItemDefinition(Slot))
    {
        Data.Name = Definition->DisplayName.IsEmpty()
            ? FText::FromName(Definition->EquipmentId) : Definition->DisplayName;
        Data.Description = Definition->Description;
        Data.Icon = Definition->Icon;
    }
    return Data;
}

FText ABBBPlayerController::GetActiveItemName() const
{
    const ABBBEquipment *Equipment = Cast<ABBBEquipment>(GetActiveItem());
    const UBBBEquipmentDefinition *Definition = IsValid(Equipment) ? Equipment->GetDefinition() : nullptr;
    return Definition ? (Definition->DisplayName.IsEmpty()
        ? FText::FromName(Definition->EquipmentId) : Definition->DisplayName) : FText::GetEmpty();
}

int32 ABBBPlayerController::GetSelectedItemSlot() const
{
    const ABBBCharacter *ItemCharacter = GetItemCharacter();
    return ItemCharacter ? ItemCharacter->RuntimeData.Item.ReadItemBarState().SelectedSlot : INDEX_NONE;
}

bool ABBBPlayerController::SubmitItemAdd(const FName EquipmentId)
{
    ABBBCharacter *ItemCharacter = GetItemCharacter();
    return ItemCharacter && !EquipmentId.IsNone()
        && ItemCharacter->SubmitInput(FBBBItemAddLocalControlPacket{{EquipmentId}});
}

bool ABBBPlayerController::SubmitItemMove(const int32 Source, const int32 Target)
{
    ABBBCharacter *ItemCharacter = GetItemCharacter();
    return ItemCharacter && Source >= 0 && Target >= 0
        && ItemCharacter->SubmitInput(FBBBItemMoveLocalControlPacket{{Source}, {Target}});
}

bool ABBBPlayerController::SubmitItemSelect(const int32 Slot)
{
    ABBBCharacter *ItemCharacter = GetItemCharacter();
    const bool bAccepted = ItemCharacter && Slot >= INDEX_NONE
        && ItemCharacter->SubmitInput(FBBBItemSelectLocalControlPacket{{Slot}});
    if (bAccepted && ItemView)
    {
        ItemView->NotifyQuickSelection();
    }
    return bAccepted;
}

void ABBBPlayerController::GetItemOperationResult(int32 &Revision, int32 &SucceededCount, int32 &RejectedCount) const
{
    Revision = 0;
    SucceededCount = 0;
    RejectedCount = 0;
    if (const ABBBCharacter *ItemCharacter = GetItemCharacter())
    {
        const auto &Result = ItemCharacter->RuntimeData.Item.ReadItemOperationState();
        Revision = Result.Revision;
        SucceededCount = Result.SucceededCount;
        RejectedCount = Result.RejectedCount;
    }
}

void ABBBPlayerController::PlayerTick(const float DeltaTime)
{
    Super::PlayerTick(DeltaTime);
    if (!IsLocalController())
    {
        return;
    }
    ABBBCharacter *ItemCharacter = GetItemCharacter();
    const int32 InventoryRevision = ItemCharacter ? ItemCharacter->RuntimeData.Item.ReadItemInventoryState().Revision : 0;
    const int32 BarRevision = ItemCharacter ? ItemCharacter->RuntimeData.Item.ReadItemBarState().Revision : 0;
    const int32 OperationRevision = ItemCharacter ? ItemCharacter->RuntimeData.Item.ReadItemOperationState().Revision : 0;
    AActor *Active = ItemCharacter ? ItemCharacter->GetActiveEquipment() : nullptr;
    if (ObservedItemCharacter.Get() != ItemCharacter && IsBackpackOpen())
    {
        ToggleBackpack();
    }
    if (ObservedItemCharacter.Get() != ItemCharacter || ObservedActiveItem.Get() != Active
        || ObservedInventoryRevision != InventoryRevision || ObservedItemBarRevision != BarRevision
        || ObservedItemOperationRevision != OperationRevision)
    {
        ObservedItemCharacter = ItemCharacter;
        ObservedActiveItem = Active;
        ObservedInventoryRevision = InventoryRevision;
        ObservedItemBarRevision = BarRevision;
        ObservedItemOperationRevision = OperationRevision;
        OnItemsChanged.Broadcast();
    }
}
