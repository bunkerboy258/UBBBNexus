# Actor 宪法

适用于承担独立玩法的 Actor 角色与具体装备必须遵守 数据构造遵守 [数据宪法](../Data/Law.md) 网络行为遵守 [网络宪法](../Network/Law.md)

## 物理文件构造

每个具体 Actor 必须按以下职责落文件 有实际职责才建目录 禁止空目录占位

```text
Actor实例目录/
    Actor根.h / .cpp                    唯一运行时根与对外入口
    Law.md                             本实例的系统清单与特殊约束
    Input/                             外部输入包与提交定义
    Animation/                         动画实例与动画图读取接口
    Config/ 或 Definition/             静态配置与资源定义 沿用实例已有命名
    Network/                           UE复制组件与传输边界 可复用已有公共载体
    Logic/
        Core/
            Initialization/            Initializer 依赖装配与初值设置
            Shutdown/                  Shutdown 关闭与资源收束
            Update/                    UpdatePipeline 更新主管线与本次调用Context
        RuntimeData/                   C RuntimeData 聚合黑板
        System/
            XxxSystem/
                XxxSystem.h / .cpp     系统控制根
                Law.md                 本领域规则
                DomainData/
                    XxxDomainState.h   B 领域状态持有者
                    States/            A 持久状态 类型以State结尾
                    Definitions/       O 领域自定义数据结构 每个类型独占头文件
                    Context/           D 本领域本次调用的临时数据
                Processors/            具体逻辑处理器
```

- 系统目录直属文件只允许系统根的头文件 实现文件与 Law 数据不得散放在系统根或 Processors 中
- B A O 各自独占头文件 文件只定义对应类型 禁止总State包装层 O必须放在所属系统的DomainData/Definitions中
- 系统目录只使用上述职责分层 禁止继续套子系统或分类层 次级Law必须明确本层允许的目录与文件

## 各部分职责

- Actor根负责UE组件装配 对外入口和生命周期转发 禁止在根里实现具体玩法规则或复制系统调度顺序
- Core只负责装配 关闭与更新编排 禁止承接移动 开火 换弹等玩法判断
- System根只组织依赖和Context 按源码固定顺序调用处理器 禁止把具体规则堆进System的Update
- Processor负责实际计算 状态转换和执行效果 一个处理器只负责一个任务 同一规则只保留一份实现
- RuntimeData与DomainData只存数据 Config或Definition只存静态参数与资源引用 禁止在数据容器中实现玩法或清理逻辑
- Animation目录只承接动画实例与快照 Network目录只承接UE传输能力 具体表现规则和同步规则分别归AnimationSystem与NetworkSystem

## 生命周期管理

- 每个实例只有一个运行时根 只公开持有一个RuntimeData 生命周期与实例一致 系统不得各自另存一套玩法黑板
- 初始化必须由Initializer统一装配依赖 校验配置并调用各领域初始化入口 必要依赖未就绪不得开始玩法更新
- 逐帧系统调度只能由UpdatePipeline发起 Actor的Tick只转发 系统与处理器不得自建Tick绕过主管线
- Tick前置依赖与分阶段更新归Core管理 必须明确哪些阶段在移动前 移动后或骨骼更新后执行
- 关闭必须经过Shutdown 先阻止后续更新 再解除依赖 停止表现并释放所持对象 可能产生回调的停止操作完成后再清空残留输入
- 持有方负责被持有实例的创建 挂接 替换与销毁 被持有方负责自己的初始化 玩法更新和内部清理 禁止由持有方代管其玩法状态

## 内部玩法实现形式

- 逐帧调用链固定为 Actor的Tick到Core主管线 到System根 到Processor 禁止通过散落的回调或动态注册表暗中改变更新顺序
- 玩法使用显式状态与处理器实现 状态保存在A 规则写在Processor 通过清楚的if或switch完成条件判断与状态转换
- 处理器的逐帧入口统一为Update 一个处理器文件只保留一个Update 禁止互相调用Update或调度别的系统
- 输入接收与玩法执行必须分开 外部请求先进入固定输入槽 同类覆盖最新值 由固定阶段消费 禁止历史请求队列
- 输入包只声明输入及其校验和应用效果 不承担网络收发 不擅自推进其他领域的玩法
- C直接持有B B直接私有持有A A持有O 写权限必须显式授予维护者 领域外只通过只读入口取结果 交接请求走约定入口
- Context只由对应调度层在栈上构造 用完销毁 系统和处理器不得跨帧保存Context或其中的临时引用
- 系统按实际职责设置 角色可以有移动 瞄准与装备持有系统 武器可以有动作系统 禁止为了数量一致制造空系统 基类或扩展点

## 跨实例 表现与网络

- 跨Actor只通过对方公开的通用入口协作 禁止直接改对方黑板 角色不得理解具体武器的开火与换弹实现
- 玩法结果由负责该玩法的领域生成 动画系统发布表现快照 动画线程只读 通知需要影响玩法时必须重新提交输入
- 网络身份使用统一来源 具体同步逻辑只放NetworkSystem 公共传输层不得解释具体玩法
- 本机执行玩法 镜像应用当前结果 镜像不得重演历史动作或重复产生发射等世界结果
- 配置缺失 非法输入 身份错误与容量越界必须通过日志或ensure报警 并停止本次无效操作 禁止带错继续执行
