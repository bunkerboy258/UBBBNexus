# Equipment Base Law

- 本目录只定义装备的公共内容 禁止持有具体装备业务

## 物理目录

物理目录固定为以下结构

```text
Base/
    Law.md
    BBBEquipment.h / .cpp                定义装备公共基类 公共组件与固定入口
    Config/                              定义公共静态配置
    Animation/                           定义公共动画实例基类与基本事实
    Input/                               声明实际共用的固定输入
    Logic/                               定义公共生命周期骨架
        Core/
            Initialization/
                BBBEquipmentInitializer.h / .cpp    定义公共初始化基类
            Update/
                BBBEquipmentUpdatePipeline.h / .cpp 定义公共更新管线基类
            Shutdown/
                BBBEquipmentShutdown.h / .cpp       定义公共关闭基类
```

## 具体装备实现要求

每种具体装备都必须在 Instance/<类型>/ 的对应位置提供以下六种实现 不能只使用 Base 代替自己的实现

- 装备 Actor 类 继承公共装备基类
- 配置类 放在 Config/ 下 继承公共配置基类
- 动画实例类 放在 Animation/ 下 继承公共动画实例基类
- 初始化类 放在 Logic/Core/Initialization/ 下 继承公共初始化基类
- 更新管线类 放在 Logic/Core/Update/ 下 继承公共更新管线基类
- 关闭类 放在 Logic/Core/Shutdown/ 下 继承公共关闭基类

以上六种具体 C++ 类都必须标记 final 表示继承到这一层就结束 禁止再从它们派生新的 C++ 类 Base 中对应的公共基类不能标记 final

公共输入包直接使用 按 Actor 宪法声明 final 禁止继承 专属输入包放在具体装备的 Input/ 下

公共枚举与基本事实数据直接使用 不要求继承 专属事实数据放在具体装备的 Animation/ 下
