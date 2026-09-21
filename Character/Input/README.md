# 角色固定输入

所有角色输入只通过 `ABBBCharacter::SubmitInput(Packet)` 提交。每种包在 `FBBBCharacterInputState` 中拥有一个固定槽位，槽位由“激活标记 + 最新数据”组成，同类型输入默认覆盖旧值。

```text
Input/
├── BBBCharacterInputSubmit      游戏线程、自检和解析重入守卫
└── Packets/
    ├── Command/                 切枪、换弹、开火和跳跃意图
    ├── Event/
    │   └── Equipment/           已经发生的装备事实
    ├── Presentation/            五个独立蒙太奇槽位与相机表现
    ├── State/                   连续控制与网络传入的最终状态
    └── _Template/               新输入包模板

ParseSystem/DomainData/States/
├── BBBCharacterInputState       全部固定槽位与提交映射
├── BBBCharacterOperationState   解析期间的换弹、切枪和瞬时操作状态
├── BBBCharacterControlState     输入系统裁决后的连续控制状态
└── BBBCharacterCameraState      等待相机系统消费的表现输入

ParseSystem/DomainData/Context/
└── BBBCharacterInputContext     输入包共享的临时解析上下文
```

## 包语义

每个包只公开三项行为：

- `IsValid`：提交闸口的数据自检。
- `CanApply`：读取上下文并判断本次是否允许应用。
- `Apply`：产生该包负责的黑板效果。

处理器不访问变体、不调用虚函数。`FBBBCharacterInputProcessor::Update` 中的源码顺序就是应用顺序：还原、事实、连续控制、请求、五个蒙太奇槽位、相机。

切枪先于换弹，换弹先于开火。后续请求直接观察前序请求已经写入的 `FBBBCharacterOperationState`，不存在 Priority 或 ApprovedBit。

## 生命周期

槽位在被解析后失活。装备系统和动画通知在解析阶段之后提交的新输入会重新激活槽位，自然留到下一次解析，不需要队列。

解析期间提交会触发报警，防止正在读取的数据被重入覆盖。网络系统不接管原始控制输入，只观察输入应用后已经形成的角色状态与装备事实；接收端再把最终状态和事实重新投递为输入包。

## 新增输入包

1. 复制 `Packets/_Template/BBBTemplatePacket.h/.cpp` 并实现三项行为。
2. 在 `FBBBCharacterInputState` 增加一个明确命名的固定槽位，并在 `BBBCharacterInputSubmit.h` 注册编译期槽位映射。
3. 在 `FBBBCharacterInputProcessor::Update` 的预期位置增加一行 `Process`。
4. 如需联网，只允许网络系统观察应用后形成的最终状态或事实，并在接收端重新构造对应输入包。

## 蒙太奇

保留 `FullBody`、`UpperBody`、`FullBodyAdditivePreAim`、`UpperBodyAdditive`、`AdditiveHitReact` 五个 Slot。`BBBCharacterMontageInput::Submit` 读取武器配置蒙太奇的轨道并分别提交对应固定槽位包；多轨蒙太奇共享播放修订号。

换弹蒙太奇的通知属于具体武器领域；当前步枪使用 `UBBBRifleDetachMagazineAnimNotify`、`UBBBRifleLoadMagazineAnimNotify` 和 `UBBBRifleReloadLifecycleAnimNotifyState` 直接提交步枪输入。
