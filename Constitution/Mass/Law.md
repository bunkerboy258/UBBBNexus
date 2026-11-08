# Mass Law

本项目的 Mass 设计追求: 实体存自己的数据 处理器批量做事 表现层还原当前结果

## 物理目录

```text
Mass/
    Core/
        BBBMassSubsystem.h / .cpp
        BBBMassProcessingGroups.h

    Network/
        公共网络载体.h / .cpp

    Instance/
        <实例类型>/
            Config/
                BBB<实例>Definition.h / .cpp

            Input/
                LocalControl/
                    <领域>/
                        FBBB<实例><作用>LocalControlPacket.h
                RemoteMessage/
                    <领域>/
                        FBBB<实例><作用>RemoteMessagePacket.h
                AuthorityFact/
                    <领域>/
                        FBBB<实例><作用>AuthorityFactPacket.h

            Fragments/
                <领域>/
                    BBB<实例><数据语义>Fragment.h

            Tags/
                BBB<实例><标记语义>Tag.h

            Traits/
                BBB<实例>Trait.h / .cpp

            Processors/
                <领域>/
                    BBB<实例><处理语义>Processor.h / .cpp

            Network/
                该实例的网络数据与桥接实现

            Presentation/
                该实例的表现桥接实现
```

- 具体实例直属目录固定为上图结构 按实际实现建立对应目录
- 类名与文件名按实例和职责命名 输入包采用 FBBB<实例><作用><来源>Packet
- Trait Processor Actor 和表现组件分别采用 UBBB 或 ABBB 前缀 Fragment 与 Tag 采用 FBBB 前缀

## 谁负责什么

| 位置                         | 职责                   |
|----------------------------|----------------------|
| Core                       | 世界级装配 创建回收 输入投递与公开查询 |
| Network                    | 公共网络载体               |
| Instance/<实例>/Config       | 静态规则 数值与引用           |
| Instance/<实例>/Input        | 输入包定义                |
| Instance/<实例>/Fragments    | 实体当前的数据              |
| Instance/<实例>/Tags         | 实体分类标记               |
| Instance/<实例>/Traits       | 出生时装配数据与配置           |
| Instance/<实例>/Processors   | 批量执行逻辑业务             |
| Instance/<实例>/Network      | 实体网络业务               |
| Instance/<实例>/Presentation | 模型 动画 声音和特效的引擎桥接层    |

- Mass 实现统一位于 UBBBNexus/Mass 并遵循网络 玩法 数据与注释宪法
- 类型与文件采用 BBB<实例><职责> 命名 

## 数据

- Entity 是一个实例的身份 
- Fragment 是这个实例的数据 
- Processor 是处理一批实例的代码
- 静态配置共享 可变数据按实例保存
- 数据按读写责任 更新频率和生命周期拆分 常一起使用的数据放一起
- Fragment 直接保存字段 优先采用 UE 标准 Fragment
- Processor 通过 Query 声明实际读写范围 
- Network 负责传递结果 
- Presentation 负责还原结果的表现内容 

## 外界怎么影响实体

- 外界通过 SubmitInput 向目标实体递输入包 输入解析处理器检查后写入对应状态
- 输入包构造与ACTOR宪法描述一致

