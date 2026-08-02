
#pragma once
#include "CoreMinimal.h"
#include "BBBDecisionTypes.generated.h"
class FBBBCharacterActionDomainArbitrator;
class FBBBCharacterActionPriorityArbitrator;
class FBBBCharacterActionPlanner;
class FBBBCharacterEquipmentStateArbitrator;
class FBBBEquipmentExecutor;
class FBBBItemActionExecutor;
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

    FBBBCharacterActionRequest() = default;

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

    EBBBCharacterActionType GetType() const
    {
        return Type;
    }

    EBBBCharacterActionDomain GetDomain() const
    {
        return Domain;
    }

    int32 GetPriority() const
    {
        return Priority;
    }

    int32 GetEquipSlot() const
    {
        return EquipSlot;
    }

    EBBBArbitrationResult GetArbitrationResult() const
    {
        return ArbitrationResult;
    }

    bool IsConsumed() const
    {
        return bConsumed;
    }
    
private:
    
    friend class FBBBCharacterActionDomainArbitrator;
    friend class FBBBCharacterActionPriorityArbitrator;
    friend class FBBBCharacterEquipmentStateArbitrator;
    friend class FBBBEquipmentExecutor;
    friend class FBBBItemActionExecutor;

    void Approve()
    {
        ArbitrationResult = EBBBArbitrationResult::Approved;
    }

    void RejectByPriority()
    {
        ArbitrationResult = EBBBArbitrationResult::RejectedByPriority;
    }

    void RejectByDomain()
    {
        ArbitrationResult = EBBBArbitrationResult::RejectedByDomain;
    }

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

    void AddRequest(FBBBCharacterActionRequest Request)
    {
        Requests.Add(MoveTemp(Request));
        /*MoveTemp的作用是如果请求内有引用类型数据，避免深拷贝*/
    }
    
public:

    int32 GetRequestCount() const
    {
        return Requests.Num();
    }

    const FBBBCharacterActionRequest &GetRequest(int32 Index) const
    {
        return Requests[Index];
    }
    
private:
    
    friend struct FBBBCharacterRuntimeData;

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
    friend class FBBBEquipmentExecutor;
    friend class FBBBItemActionExecutor;

    FBBBCharacterActionRequest &AccessRequestForArbitration(int32 Index)
    {
        return Requests[Index];
    }

    FBBBCharacterActionRequest &AccessRequestForExecution(int32 Index)
    {
        return Requests[Index];
    }

    UPROPERTY()
    TArray<FBBBCharacterActionRequest> Requests;
};
