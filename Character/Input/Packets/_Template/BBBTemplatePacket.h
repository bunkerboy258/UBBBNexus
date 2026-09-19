#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterPacketContext.h"

// TODO 新包复制本对文件到所属带目录后 改名改带 并在 BBBCharacterPacketRegistry.h 登记
// 本模板不参与注册 仅作新包起点 仲裁规则见注册表文件头全景注释

/**
 * TODO 包名与一句话职责 注明所属优先级带与避让关系
 */
struct FBBBTemplatePacket
{
    // TODO 从 BBBCharacterPacketPriority 带区间选取 请求带内必须全局唯一
    static constexpr int32 Priority = 0;

    // TODO 全局唯一身份位 取未被占用的 1ull << N 撞位会被 static_assert 拦下
    static constexpr uint64 ApprovedBit = 0;

    // TODO 包数据字段 只放本包自有的数据

    /** @return 包内容是否合法 */
    bool IsValid() const;

    /**
     * 检查本帧是否允许执行 约定只读
     * 请求带铁律 只读解析状态与已批准集合 禁止读控制基座
     * @param Context	黑板上下文
     * @return 是否允许执行
     */
    bool CanExecute(const FBBBCharacterPacketContext &Context) const;

    /**
     * 应用黑板效果 只写本包负责的域 跨帧守卫走 ParseState 语义方法
     * @param Context	黑板上下文
     */
    void Execute(FBBBCharacterPacketContext &Context) const;
};
