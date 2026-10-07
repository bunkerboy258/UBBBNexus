# Equipment Base Law

- 本目录只定义装备的公共内容 禁止持有具体装备业务

## 物理目录

```text
Base/
    Law.md
    BBBEquipment.h / .cpp
    Config/                              公共静态配置
    Animation/                           公共动画实例基类与基本事实
    Input/                               公共固定输入声明
        LocalControl/
            <领域>/
                <输入包>.h
        RemoteMessage/
            <领域>/
                <输入包>.h
        AuthorityFact/
            <领域>/
                <输入包>.h
    Logic/
        Core/
            Initialization/
                BBBEquipmentInitializer.h / .cpp
```

- 物理目录固定为上述结构 各级可以按需设置 Law.md 没有实际内容的目录不创建
- BBBEquipment.h / .cpp 只定义装备公共基类 公共组件与固定入口
- Config/ 只定义公共静态配置 禁止持有单件装备的可变状态
- Animation/ 只定义公共动画实例基类与基本事实 禁止持有具体装备行为规则
- Input/ 只声明实际共用的固定输入 禁止依赖具体装备黑板或承载具体装备的序列化协议
- 具体装备专属配置 动画事实与输入包放在 Instance/<类型>/ 对应目录下
- Logic/ 只允许公共组件初始化 禁止设置业务黑板 更新管线或业务系统
- Base/ 禁止设置 Network/ 禁止定义共用装备网络组件或网络协议基类

## 职责边界

- 角色负责装备持有关系的创建 挂接 替换 销毁与同步 相关输入包定义在角色侧
- 具体装备负责自己的初始化 玩法更新 内部清理与行为同步
- 具体装备的 UE 网络桥接组件放在 Instance/<类型>/Network/
- 具体装备的网络业务处理放在自身 Logic/System/NetworkSystem/ 具体逻辑下沉到 Processors/
- 具体装备自行定义实际需要的消息与状态格式 禁止强制不同装备共用完整状态包
- 允许借用持有角色的网络连接 禁止让角色解析或维护具体装备业务
- 网络身份只读持有角色黑板 禁止复制镜像标记
- 网络接收通过输入入口进入具体实例 禁止直接写业务黑板
- 网络原则与输入入口遵守 Actor 和 Network 宪法 禁止为形式统一增加无实际需要的继承层或通用框架
