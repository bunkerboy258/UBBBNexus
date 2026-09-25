# Actor

适用于承担独立玩法的 Actor，包括角色与具体装备。数据构造遵守 [数据宪法](../Data/Law.md)，网络行为遵守 [网络宪法](../Network/Law.md)。

## 物理文件构造

以下模板规定文件的职责与存放位置。Actor 根、Input/、Logic/Core/、Logic/RuntimeData/、Logic/System/ 及下文规定的必需系统是固定结构。Animation/、Config/、Network/ 仅在实际使用时创建；系统内的 Definitions/、Context/ 仅在需要对应的 O、D 时创建。禁止为对齐模板建立空目录。

参考模板如下：
```text
BBBActor实例目录/
    Actor根.h / .cpp                   唯一运行时根与对外入口
    Input/                             外部输入包与如何提交的定义
    Animation/                         动画实例基类
    Config/                            静态配置
    Network/                           与UE的网络系统桥接的组件
    Logic/
        Core/
            Initialization/            Initializer 初始化逻辑
            Shutdown/                  Shutdown 销毁或失活逻辑
            Update/                    UpdatePipeline 更新主管线
        RuntimeData/                   RuntimeData 运行时黑板 (C)
        System/
            XxxSystem/
                XxxSystem.h / .cpp     系统控制根
                DomainData/
                    XxxDomainState.h   B 
                    States/            A 
                    Definitions/       O (没有不创建此目录)
                    Context/           D 
                Processors/            具体逻辑处理器
```
- BBBActor必须实现一个SubmitInput(<输入数据包的类型模板形参> &&<输入包>(因为是临时对象所以使用右值引用))方法 
- 且此方法是外部影响该 Actor 的唯一入口
- BBBActor 必须拥有 ParseSystem、AnimationSystem、NetworkSystem 和至少一种其它类型的 System
- 可以根据实现的玩法不同 创建不同的System 但是都必须拥有一致的物理文件 内部架构一致的代码设计

## 生命周期管理

- 每个实例只有一个运行时根 
- 初始化逻辑必须全部下沉到Initializer处理
- Tick调度只能由UpdatePipeline发起 Actor的Tick只转发
- 关闭必须经过Shutdown 
- 持有方负责被持有Actor实例的创建 挂接 替换与销毁由“持有者”管理（比如角色是装备的持有者）
- 被持有方负责自己的初始化 玩法更新和内部清理 

## 设计哲学

- BBBActor在本架构的抽象模型是：Actor 的不同能力或内部行为由各个系统（System）负责
- 每个系统维护黑板上对应的领域状态，并根据该 Actor 的状态表现出相应行为
- 因此，Actor 的运行时黑板由各系统对应的领域状态组成
- 外部要影响 Actor 的行为，就通过输入包修改对应的状态数据
- SubmitInput(...)是外部提交输入包的入口
- 输入包在 Input/ 下静态定义如何影响 Actor 状态，以及在什么条件下允许应用
- 这样 外部系统通过组装‘输入包’ 并通过SubmitInput(...)方法推送它 
- ParseSystem 处理后，合法输入包的效果写入对应的状态数据
- 更进一步 无论是涉及网络同步的BBBActor网络消息 还是需要用‘动画通知’等第三方资产承载的逻辑
- 都可以通过本套设计完成与BBBActor的交互

## 表现层的工作

- 为了实现逻辑与表现的解耦，BBBActor的动画实例基类保存该 Actor 的基本状态快照，并承担与引擎动画侧的桥接功能
- 动画实例基类只能提供‘动画层需要用到的’‘基本事实’与‘状态’ 不能持有过度处理的二级事实
- 这样 在引擎内的具体动画蓝图的线程安全更新函数内 会在工作线程下 根据基类提供的‘基本数据’ 线程安全的生成驱动表现层需要的数据 
- ‘AnimationSystem’直接且独享对动画实例基类的维护权 同样为了解耦 它只能只读黑板上的数据

## 网络同步原理

- 我们先根据BBBActor在本项目P2P网络模型下 扮演的角色的不同网络权责 进行简单的分类:
- 规定:
- X：此 Actor 由本客户端控制；间接控制也算，例如装备通过持有角色受玩家客户端控制
- Y：此 Actor 不由本客户端控制
- M：本客户端是主机（权威客户端）
- N：本客户端是客机（非权威客户端）
- 排列组合一下 显而易见可以分为四类:    
-     甲   乙    丙   丁
-     XM   XN   YM   YN
- 那么 对应的网络职责就显而易见了:
- 甲：生成并维护权威事实，向客机侧的丁类 Actor 分发当前结果，并校验丁类上报的消息
- 乙：生成事实并投送给主机侧的丙类 Actor，接收并还原丙类发送的当前结果
- 丙：维护权威事实，向客机侧的乙类和丁类 Actor 分发当前结果，并校验它们上报的消息
- 丁：可以因本机观察到友伤等事件生成事实并投送给主机侧对应的甲类或丙类 Actor；同时接收并还原主机发送的当前结果
- 生成或观察事实：领域系统根据本地输入或观察将离散事实与连续状态变化写入该 Actor 的黑板；NetworkSystem 每帧只读黑板，将需要同步的事实交给 Network/ 下的网络组件传输
- 接收并校验：网络消息作为输入包进入该 Actor 的帧更新流程；校验仅检查包格式、目标实例、发送来源、字段范围与消息先后，不重新裁决对端已生成事实的玩法因果
- 还原：通过上述校验的还原包按携带的当前结果应用，不再检查本机玩法前置条件；还原过程不得再次产生发射、伤害等世界结果
