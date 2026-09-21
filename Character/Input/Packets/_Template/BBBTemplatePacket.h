#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

// TODO: 复制本对文件到对应语义目录并替换类型名
// TODO: 在FBBBCharacterInputState中增加槽位和Submit重载
// TODO: 在FBBBCharacterInputProcessor::Update中明确插入应用顺序

/** TODO: 说明输入语义、数据来源和负责修改的黑板状态 */
struct FBBBTemplatePacket final
{
    // TODO: 只保存该输入自己的完整数据，累计工作由提交方完成

    /** @return 包内容是否合法 */
    bool IsValid() const;

    /**
     * 判断当前角色状态是否允许应用
     * @param Context   角色输入上下文
     * @return 是否允许应用
     */
    bool CanApply(const FBBBCharacterInputContext &Context) const;

    /**
     * 应用该输入负责的黑板效果
     * @param Context   角色输入上下文
     */
    void Apply(FBBBCharacterInputContext &Context) const;
};
