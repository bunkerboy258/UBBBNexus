#pragma once
#include "CoreMinimal.h"
#include "BBBMonsterDirectorPhase.generated.h"

/** 副本导演当前运行阶段 */
UENUM(BlueprintType)
enum class EBBBMonsterDirectorPhase : uint8
{
    /** 尚未开始 */
    Inactive UMETA(DisplayName = "尚未开始"),
    /** 等待阶段启动条件 */
    Waiting UMETA(DisplayName = "等待触发"),
    /** 生成额度尚未用完 */
    Spawning UMETA(DisplayName = "生成小怪"),
    /** 额度已用完 等待存活与预留清零 */
    Clearing UMETA(DisplayName = "等待清理"),
    /** 保留当前阶段暂停新增 */
    Paused UMETA(DisplayName = "已暂停"),
    /** 已停止新增 */
    Stopped UMETA(DisplayName = "已停止"),
    /** 全部阶段完成 */
    Completed UMETA(DisplayName = "副本完成")
};
