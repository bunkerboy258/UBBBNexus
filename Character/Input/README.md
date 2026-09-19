# 角色固定输入帧

所有角色输入只通过 `ABBBCharacter::SubmitInput(Packet)` 提交。每种包在 `FBBBCharacterInputFrame` 中拥有一个固定槽位，槽位由“激活标记 + 最新数据”组成，同类型输入默认覆盖旧值。

```text
Input/
├── BBBCharacterInputFrame       全部固定槽位与提交映射
├── BBBCharacterInputSubmit      游戏线程、自检和解析重入守卫
└── Packets/
    ├── BBBCharacterPacketContext
    ├── Base/                    移动与瞄准控制
    ├── Request/                 切枪、换弹、开火和跳跃请求
    ├── ReloadPhase/             换弹动画通知
    ├── Fact/                    已形成的装备事实
    ├── Restore/                 网络还原状态
    └── Presentation/            五个独立蒙太奇槽位与相机输入
```

## 包语义

每个包只公开三项行为：

- `IsValid`：提交闸口的数据自检。
- `CanApply`：读取上下文并判断本次是否允许应用。
- `Apply`：产生该包负责的黑板效果。

处理器不排序、不访问变体、不调用虚函数。`FBBBCharacterInputProcessor::Update` 中的源码顺序就是应用顺序：还原、事实、连续控制、请求、换弹通知、五个蒙太奇槽位、相机。

切枪先于换弹，换弹先于开火。后续请求直接观察前序请求已经写入的 `FBBBCharacterParseState`，不存在 Priority 或 ApprovedBit。

## 生命周期

槽位在被解析后失活。装备系统和动画回调在解析阶段之后提交的新输入会重新激活槽位，自然留到下一次解析，不需要队列。

本机非权威角色的移动、瞄准、切枪、换弹和开火槽位会暂时保留激活标记，由 `FBBBCharacterNetworkCommandProcessor` 上传权威端后失活。解析期间提交会触发报警，防止正在读取的数据被重入覆盖。

## 新增输入包

1. 复制 `Packets/_Template/BBBTemplatePacket.h/.cpp` 并实现三项行为。
2. 在 `FBBBCharacterInputFrame` 增加一个明确命名的固定槽位和一个 `Submit` 重载。
3. 在 `FBBBCharacterInputProcessor::Update` 的预期位置增加一行 `Process`。
4. 如需联网，明确它属于客户端命令、权威事实还是远端还原输入，并在对应网络边界增加传输结构。

## 蒙太奇

保留 `FullBody`、`UpperBody`、`FullBodyAdditivePreAim`、`UpperBodyAdditive`、`AdditiveHitReact` 五个 Slot。`BBBCharacterMontageInput::Submit` 读取武器配置蒙太奇的轨道并分别提交对应固定槽位包；多轨蒙太奇共享播放修订号。

换弹通知继续使用 `BBB.Reload.Start` 和 `BBB.Reload.End`，本轮不新增或自动修改动画资产通知。
