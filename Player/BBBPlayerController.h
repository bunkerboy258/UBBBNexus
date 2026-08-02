
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BBBPlayerController.generated.h"
//封装InputMappingContext的数据与行为
class UInputMappingContext;
//封装InputAction的数据与行为
class UInputAction;

//将下方类型注册为受虚幻对象系统管理的类
UCLASS()
//封装ABBB玩家控制器的数据与行为
class ABBB_EVAC_API ABBBPlayerController : public APlayerController
{
    GENERATED_BODY()
public:

    //创建并配置ABBB玩家控制器
    ABBBPlayerController();

    //进入游戏时建立运行依赖
    virtual void BeginPlay() override;

    //绑定玩家输入动作
    virtual void SetupInputComponent() override;

    //让下方函数按照所列规则参与反射调用或远程调用
    UFUNCTION(BlueprintCallable, Category = "BBB|Input")
    //切换鼠标指针与游戏输入模式
    void ToggleMouseCursor();
protected:

    //让下方成员按照所列规则参与编辑序列化或网络复制
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Input")
    //保存默认MappingContext供所属对象后续流程使用
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    //让下方成员按照所列规则参与编辑序列化或网络复制
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Input")
    //更新int32MappingContextPriority供后续步骤读取
    int32 MappingContextPriority = 0;
private:

    //让下方成员按照所列规则参与编辑序列化或网络复制
    UPROPERTY()
    //保存ToggleMouseAction供所属对象后续流程使用
    TObjectPtr<UInputAction> ToggleMouseAction;

    //让下方成员按照所列规则参与编辑序列化或网络复制
    UPROPERTY()
    //保存ToggleMouseIMC供所属对象后续流程使用
    TObjectPtr<UInputMappingContext> ToggleMouseIMC;

    //切换鼠标菜单输入模式
    void SetMouseMenuMode(bool bEnabled);
};
