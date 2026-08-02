
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Aim/BBBAimTypes.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Facing/BBBFacingTypes.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Input/BBBInputTypes.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Item/BBBCharacterItemConfig.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Locomotion/BBBLocomotionTypes.h"
#include "BBBCharacterConfig.generated.h"
class UInputAction;

USTRUCT(BlueprintType)
//聚合输入处理参数与所有角色输入动作
struct FBBBCharacterInputConfig
{
    GENERATED_BODY()

    //定义死区与输入容错窗口
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FBBBInputPipelineConfig Pipeline;

    //绑定二维角色移动输入
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> MoveAction;

    //绑定二维视角旋转输入
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> LookAction;

    //绑定武器持续开火输入
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> FireAction;

    //绑定精确瞄准保持输入
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> PrecisionAimAction;

    //绑定角色冲刺保持输入
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> SprintAction;

    //绑定武器一次性换弹输入
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> ReloadAction;

    //绑定一号快捷栏选择输入
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> EquipSlot1Action;

    //绑定二号快捷栏选择输入
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> EquipSlot2Action;
};

USTRUCT(BlueprintType)
//定义第三人称相机基础与瞄准状态参数
struct FBBBCharacterCameraConfig
{
    GENERATED_BODY()

    //缩放设备视角输入的基础旋转速度
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float BaseTurnRate = 1.0f;

    //设置非瞄准状态弹簧臂长度
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float CameraBoomLength = 400.0f;

    //设置相机在弹簧臂末端的局部偏移
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVector CameraBoomSocketOffset = FVector::ZeroVector;

    //设置弹簧臂根部相对角色的目标偏移
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVector CameraBoomTargetOffset = FVector::ZeroVector;

    //设置相机组件相对插槽的局部位置
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVector CameraRelativeLocation = FVector::ZeroVector;

    //决定弹簧臂是否启用位置滞后
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bCameraLag = false;

    //控制弹簧臂滞后追赶速度
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float CameraLagSpeed = 10.0f;

    //设置瞄准状态下缩短后的弹簧臂长度
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float AimBoomLength = 200.0f;

    //控制弹簧臂在瞄准长度间的插值速度
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float AimBoomInterpSpeed = 12.0f;
};

USTRUCT(BlueprintType)
//定义武器挂接与瞄准骨骼名称
struct FBBBCharacterEquipmentConfig
{
    GENERATED_BODY()

    //指定主手武器挂接的右手插槽
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName RightHandWeaponSocketName = TEXT("WeaponGrip_R");

    //指定计算武器瞄准方向的源骨骼
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName AimSourceBoneName = TEXT("hand_r");

    //指定左手武器握持逆向动力学骨骼
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName LeftHandIKBoneName = TEXT("hand_l");
};

USTRUCT(BlueprintType)
//聚合角色全部可编辑运行配置
struct FBBBCharacterConfig
{
    GENERATED_BODY()

    //保存输入动作与输入处理参数
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FBBBCharacterInputConfig Input;

    //保存移动速度加速度与平滑参数
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FBBBCharacterLocomotionConfig Locomotion;

    //保存角色身体朝向阈值与速度
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FBBBCharacterFacingConfig Facing;

    //保存瞄准射线距离与起点骨骼
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FBBBAimConfig Aim;

    //保存瞄准动画补偿与逆向动力学参数
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FBBBAimAnimationConfig AimAnimation;

    //保存第三人称相机基础与瞄准参数
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FBBBCharacterCameraConfig Camera;

    //保存库存容量默认物品与自动装备策略
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FBBBCharacterItemConfig Item;

    //保存装备挂接与瞄准骨骼名称
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FBBBCharacterEquipmentConfig Equipment;
};
