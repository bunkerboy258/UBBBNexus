# Mass

## 适用范围

- 全项目 Mass 代码实现必须位于 `UBBBNexus/Mass/`
- 遵守网络宪法 玩法宪法
- 本目录不适用 Actor 的 ABCO 持有关系与逐实例系统构造

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

- 当前实例类型使用 `Projectile/` 与 `Monster/`
- 同一领域在 `Input/` `Fragments/` `Processors/` 下必须使用相同目录名
- 无实际实现的目录不创建
- 禁止在实例目录中增加 `Base/` `Template/` `Logic/System/` 等中间包装层
- 公共实现只允许放入 `Mass/Core/` 与 `Mass/Network/`
- 具体实例之间只能通过公开输入与查询交互 禁止直接依赖对方的 Fragment 与 Processor

## 命名规范

| 对象         | 类型命名                               | 文件命名                                    |
|------------|------------------------------------|-----------------------------------------|
| 静态配置       | `UBBB<实例>Definition`               | `BBB<实例>Definition.h / .cpp`            |
| 输入包        | `FBBB<实例><作用><来源>Packet`           | 与结构体同名的 `.h`                            |
| 实例数据       | `FBBB<实例><数据语义>Fragment`           | `BBB<实例><数据语义>Fragment.h`               |
| 分类标记       | `FBBB<实例><标记语义>Tag`                | `BBB<实例><标记语义>Tag.h`                    |
| 模板装配       | `UBBB<实例>Trait`                    | `BBB<实例>Trait.h / .cpp`                 |
| 批量处理器      | `UBBB<实例><处理语义>Processor`          | `BBB<实例><处理语义>Processor.h / .cpp`       |
| 表现 Actor   | `ABBB<实例>PresentationActor`        | `BBB<实例>PresentationActor.h / .cpp`     |
| 表现组件       | `UBBB<实例>PresentationComponent`    | `BBB<实例>PresentationComponent.h / .cpp` |

- 一个文件只定义一个公开业务类型
- 类型名必须说明具体职责 禁止使用 `Manager` `Helper` `Utils` 掩盖不同职责

## 文件职责

| 位置                             | 必须归入的内容                            |
|--------------------------------|------------------------------------|
| `Core/BBBMassSubsystem`        | 世界级依赖装配 实例创建与回收 输入投递入口             |
| `Core/BBBMassProcessingGroups` | 更新阶段名称与统一顺序约定                      |
| `Config/`                      | 静态参数与资产引用                          |
| `Input/`                       | 输入数据与 `IsValid` `CanApply` `Apply` |
| `Fragments/`                   | 实例持久状态与必要的共享数据定义                   |
| `Tags/`                        | 无数据的分类标记                           |
| `Traits/`                      | 实体模板所需 Fragment Tag 与配置的装配         |
| `Processors/`                  | 输入解析 移动 碰撞 生命 寻路 网络观察等具体更新逻辑       |
| `Network/`                     | 传输数据 RPC 属性复制与接收投递                 |
| `Presentation/`                | 动画 Niagara 与表现对象的 UE 桥接            |

- Core 禁止承载具体实例逻辑
- Trait 禁止承载逐帧玩法逻辑
- Network 禁止生成玩法事实或重新裁决已成立结果
- Presentation 禁止维护第二套玩法状态
- Processor 直接承担批量更新 

## 数据与访问

- Fragment 直接存字段 不要再包一层.
- 按共同读写范围 更新频率 生命周期拆分 Fragment.
- 常一起读 由同一职责维护的数据 合并.
- 写入责任或更新需求不同的数据 拆开.
- 静态配置可共享;可变状态按实例独立存.
- 同一状态只能有一个明确维护来源.
- 优先用语义匹配的 UE 标准 Fragment 不要另建同义状态.
- Processor 必须用 Query 声明实际需要的只读 读写权限.
- 不要跨帧保存 Query 取得的 Fragment 地址和引用.

## 输入与调度

- 外部行为输入必须经 `SubmitInput` 投到目标 Entity.
- 输入包必须是 final 结构体;一包一头文件 不写 cpp.
- 输入包只读允许数据 只写对应状态;禁调具体行为与 UE 执行接口.
- 每目标 每种输入只有一个槽位;槽位只含 `bActive` 和输入包.
- 同种输入后到覆盖先到;禁止改成事件追加队列.
- 输入由解析 Processor 在固定阶段消费.
- 后续 Processor 按状态更新对应行为.
- 每个 Processor 必须声明阶段和必要执行依赖.
- 实际更新交给 Mass 调度;禁止逐实例创建 Tick 和系统对象.
- 涉及 UE 接口的处理必须遵守线程要求.
- 实体结构变化必须用 Mass 允许的安全时机.

## 网络权责

需要同步的实例必须按下表划分职责

| 分类 | 条件 | 必须执行的职责 |
|---|---|---|
| 甲 | 本机控制且本机为房主 | 生成并维护事实;接收丁的消息;向丁分发结果 |
| 乙 | 本机控制且本机为客机 | 本机操作立即成立;向丙上报结果;接收丙分发的事实 |
| 丙 | 非本机控制且本机为房主 | 接收乙与丁的结果;维护当前事实;向乙与丁分发;禁止重演乙的因果 |
| 丁 | 非本机控制且本机为客机 | 接收并还原甲或丙的事实;将本机合法交互产生的结果上报甲或丙 |

- 镜像可接受本机其他实例造成的合法影响 但不得自行决定原控制端负责的行为.
- 网络接收必须转成目标实例的输入.
- 跨机实例身份不得用本地 Entity 下标.
- 重复消息不得重复生效;旧实例消息不得作用于新实例.
- 禁止预测 回滚与历史因果重演.

## 子弹与小怪

- 子弹不做独立网络同步 开火由武器同步.
- 各端从本机枪口生成子弹 方向取本机枪口正前方.
- 子弹能否造成伤害 由发射来源的本机控制权决定.
- 子弹表现与逻辑碰撞分离 表现用批量光效完成.

- 小怪目标选择 寻路与行动决策由房主执行.
- 客机只还原行动结果;本机有效命中立即影响血量.
- 本地致死立即生效 并上报房主.
- 同一代小怪死亡后 禁止被旧存活状态恢复.
- 小怪逻辑碰撞不得依赖表现 Actor 是否存在.