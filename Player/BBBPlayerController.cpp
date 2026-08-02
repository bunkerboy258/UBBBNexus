
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "UObject/ConstructorHelpers.h"
//定义当前编译单元使用的日志分类
DEFINE_LOG_CATEGORY_STATIC(LogBBBPlayerController, Log, All);

//创建并配置ABBB玩家控制器
ABBBPlayerController::ABBBPlayerController()
{
    //允许控制器参与服务器到客户端的属性复制
    bReplicates = true;
    //在类默认对象构造期间加载所需资源
    static ConstructorHelpers::FObjectFinder<UInputMappingContext> DefaultMappingContextAsset(TEXT("/Game/Input/IMC_Default.IMC_Default"));
    //资源加载成功后缓存角色使用的默认输入映射
    if (DefaultMappingContextAsset.Succeeded())
    {
        //保存默认映射供本地玩家进入游戏时注册
        DefaultMappingContext = DefaultMappingContextAsset.Object;
    }
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

//绑定玩家输入动作
void ABBBPlayerController::SetupInputComponent()
{
    //先执行父类的绑定玩家输入动作
    Super::SetupInputComponent();
    //将控制器输入组件转换为增强输入组件
    UEnhancedInputComponent *EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
    //EnhancedInput与ToggleMouseAction不满足时停止当前绑定玩家输入动作流程
    if (!EnhancedInput || !ToggleMouseAction)
    {
        //结束当前绑定玩家输入动作流程
        return;
    }
    //把输入动作绑定到对应的控制器回调
    EnhancedInput->BindAction(ToggleMouseAction, ETriggerEvent::Started, this, &ABBBPlayerController::ToggleMouseCursor);
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
    //切换鼠标菜单输入模式
    SetMouseMenuMode(!bShowMouseCursor);
}

//切换鼠标菜单输入模式
void ABBBPlayerController::SetMouseMenuMode(bool bEnabled)
{
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
