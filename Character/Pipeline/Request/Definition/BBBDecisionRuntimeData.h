
#pragma once
#include "CoreMinimal.h"
#include "BBBDecisionRuntimeData.generated.h"
class FBBBCharacterActionDomainArbitrator;
class FBBBCharacterActionPriorityArbitrator;
class FBBBCharacterActionPlanner;
class FBBBCharacterEquipmentStateArbitrator;
class FBBBEquipmentSelectionExecutor;
class FBBBEquipmentActionExecutor;
struct FBBBCharacterRuntimeData;

UENUM(BlueprintType)
//离散动作枚举大全
enum class EBBBCharacterActionType : uint8
{
    None,
    Equip,
    Fire,
    Reload
};

UENUM(BlueprintType)
//动作占用的角色表现范围
enum class EBBBCharacterActionDomain : uint8
{
    None,
    UpperBody,
    FullBody
};

UENUM(BlueprintType)
//动作请求经过仲裁后的明确结果
enum class EBBBArbitrationResult : uint8
{
    //还没出结果
    Pending,

    //批准
    Approved,

    //因同域存在更高优先级动作而失败
    RejectedByPriority,

    //因特殊状态或动作域冲突而失败
    RejectedByDomain
};

USTRUCT(BlueprintType)
//单个动作封装
struct FBBBCharacterActionRequest
{
    GENERATED_BODY()

    /**
     * 默认构造空动作请求
     */
    FBBBCharacterActionRequest() = default;

    /**
     * 构造完整动作请求
     * @param InType	动作类型
     * @param InDomain	动作占用域
     * @param InPriority	仲裁优先级
     * @param InEquipSlot	目标装备槽位
     */
    FBBBCharacterActionRequest(
        EBBBCharacterActionType InType,
        EBBBCharacterActionDomain InDomain,
        int32 InPriority,
        int32 InEquipSlot = INDEX_NONE)
        : Type(InType)
        , Domain(InDomain)
        , Priority(InPriority)
        , EquipSlot(InEquipSlot)
    {
    }

    /**
     * 读取动作类型
     * @return 动作类型
     */
    EBBBCharacterActionType GetType() const
    {
        return Type;
    }

    /**
     * 读取动作占用域
     * @return 动作占用域
     */
    EBBBCharacterActionDomain GetDomain() const
    {
        return Domain;
    }

    /**
     * 读取仲裁优先级
     * @return 仲裁优先级
     */
    int32 GetPriority() const
    {
        return Priority;
    }

    /**
     * 读取目标装备槽位
     * @return 装备槽位下标
     */
    int32 GetEquipSlot() const
    {
        return EquipSlot;
    }

    /**
     * 读取仲裁结果
     * @return 仲裁结果
     */
    EBBBArbitrationResult GetArbitrationResult() const
    {
        return ArbitrationResult;
    }

    /**
     * 判断请求是否已被执行器消费
     * @return 是否已消费
     */
    bool IsConsumed() const
    {
        return bConsumed;
    }
    
private:
    
    friend class FBBBCharacterActionDomainArbitrator;
    friend class FBBBCharacterActionPriorityArbitrator;
    friend class FBBBCharacterEquipmentStateArbitrator;
    friend class FBBBEquipmentSelectionExecutor;
    friend class FBBBEquipmentActionExecutor;

    /**
     * 标记请求为批准
     */
    void Approve()
    {
        ArbitrationResult = EBBBArbitrationResult::Approved;
    }

    /**
     * 标记请求因优先级被驳回
     */
    void RejectByPriority()
    {
        ArbitrationResult = EBBBArbitrationResult::RejectedByPriority;
    }

    /**
     * 标记请求因动作域冲突被驳回
     */
    void RejectByDomain()
    {
        ArbitrationResult = EBBBArbitrationResult::RejectedByDomain;
    }

    /**
     * 标记请求已被执行器消费
     */
    void MarkConsumed()
    {
        bConsumed = true;
    }

private:

    UPROPERTY()
    EBBBCharacterActionType Type = EBBBCharacterActionType::None;

    UPROPERTY()
    EBBBCharacterActionDomain Domain = EBBBCharacterActionDomain::None;

    UPROPERTY()
    int32 Priority = 0;

    UPROPERTY()
    int32 EquipSlot = INDEX_NONE;

    UPROPERTY()
    EBBBArbitrationResult ArbitrationResult = EBBBArbitrationResult::Pending;

    bool bConsumed = false;
};

USTRUCT(BlueprintType)
//当前帧全部动作请求的容器
struct FBBBDecisionRuntimeData
{
    GENERATED_BODY()
private:
    friend class FBBBCharacterActionPlanner;

    /**
     * 追加一个动作请求
     * @param Request	待加入的动作请求
     */
    void AddRequest(FBBBCharacterActionRequest Request)
    {
        Requests.Add(MoveTemp(Request));
        /*MoveTemp的作用是如果请求内有引用类型数据，避免深拷贝*/
    }
    
public:

    /**
     * 读取当前请求数量
     * @return 请求数量
     */
    int32 GetRequestCount() const
    {
        return Requests.Num();
    }

    /**
     * 读取指定下标的动作请求
     * @param Index	请求下标
     * @return 动作请求
     */
    const FBBBCharacterActionRequest &GetRequest(int32 Index) const
    {
        return Requests[Index];
    }
    
private:
    
    friend struct FBBBCharacterRuntimeData;

    /**
     * 校验仲裁与执行完整性后清空本帧请求
     */
    void CleanFrame()
    {
        for (const FBBBCharacterActionRequest &Request : Requests)
        {
            ensureMsgf(
                Request.GetArbitrationResult() != EBBBArbitrationResult::Pending,
                //报告没有完成仲裁的请求信息
                TEXT("[UBBBC]Action request reached cleanup without arbitration Type %d Domain %d Priority %d Result %d"),
                static_cast<int32>(Request.GetType()),
                static_cast<int32>(Request.GetDomain()),
                Request.GetPriority(),
                static_cast<int32>(Request.GetArbitrationResult()));

            ensureMsgf(
                //报告批准但没有被执行的请求信息
                Request.GetArbitrationResult() != EBBBArbitrationResult::Approved
                    || Request.IsConsumed(),
                TEXT("[UBBBC]Approved action request reached cleanup without executor Type %d Domain %d Priority %d Result %d"),
                static_cast<int32>(Request.GetType()),
                static_cast<int32>(Request.GetDomain()),
                Request.GetPriority(),
                static_cast<int32>(Request.GetArbitrationResult()));
        }

        Requests.Reset();
    }
    
    friend class FBBBCharacterActionDomainArbitrator;
    friend class FBBBCharacterActionPriorityArbitrator;
    friend class FBBBCharacterEquipmentStateArbitrator;
    friend class FBBBEquipmentSelectionExecutor;
    friend class FBBBEquipmentActionExecutor;

    /**
     * 为仲裁器提供可写请求访问
     * @param Index	请求下标
     * @return 可写的动作请求
     */
    FBBBCharacterActionRequest &AccessRequestForArbitration(int32 Index)
    {
        return Requests[Index];
    }

    /**
     * 为执行器提供可写请求访问
     * @param Index	请求下标
     * @return 可写的动作请求
     */
    FBBBCharacterActionRequest &AccessRequestForExecution(int32 Index)
    {
        return Requests[Index];
    }

    UPROPERTY()
    TArray<FBBBCharacterActionRequest> Requests;
};
