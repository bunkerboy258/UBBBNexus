#pragma once
class ABBBWorldDropItem;
/** 世界掉落物初始化 */
class FBBBWorldDropInitializer final
{
public:
    /** @param Drop\t唯一运行时根 @return 无 构造默认组件 */
    static void Construct(ABBBWorldDropItem &Drop);
    /** @param Drop\t唯一运行时根 @return 无 只准备编辑器静态显示 */
    static void Preview(ABBBWorldDropItem &Drop);
    /** @param Drop\t唯一运行时根 @return 无 */
    static void Initialize(ABBBWorldDropItem &Drop);
};
