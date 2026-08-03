
#pragma once
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/Processors/BBBInputProcessor.h"
class FBBBCharacterInitializer;
struct FBBBCharacterWorldRuntimeData;
struct FBBBInputRawData;
struct FBBBInputRuntimeData;
struct FBBBInputPipelineConfig;

//连接原始输入缓冲处理器与输入运行时数据
class FBBBInputPipeline final
{
public:

    /**
     * 生成当前帧供意图系统读取的稳定输入
     */
    void Update() const;
private:
    //初始化器负责一次性注入输入依赖
    friend class FBBBCharacterInitializer;

    /**
     * 绑定输入缓冲运行时数据与处理配置
     * @param InInputData	输入运行时数据
     * @param InWorldData	角色所属世界运行时数据
     * @param InInputRawData	原始输入缓冲
     * @param InConfig	输入管线配置
     */
    void Initialize(
        FBBBInputRuntimeData &InInputData,
        const FBBBCharacterWorldRuntimeData &InWorldData,
        const FBBBInputRawData &InInputRawData,
        const FBBBInputPipelineConfig &InConfig);

    /** 角色所属世界与帧时间来源 */
    const FBBBCharacterWorldRuntimeData *WorldData = nullptr;

    //由增强输入回调持续写入的原始输入
    const FBBBInputRawData *InputRawData = nullptr;

    //保存本帧规范化输入与历史缓冲状态
    FBBBInputRuntimeData *InputData = nullptr;

    //定义死区与输入容错时间窗口
    const FBBBInputPipelineConfig *Config = nullptr;

    //执行无状态的输入规范化计算
    FBBBInputProcessor InputProcessor;
};
