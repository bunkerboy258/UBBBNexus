#pragma once
struct FBBBGrenadeRuntimeData;
class UBBBGrenadeDefinition;
class ABBBCharacter;
/** 本次使用调度 */
class FBBBGrenadeUseSystem final
{
  public:
    /** @param InData 黑板 @param InDefinition 静态定义 @param InCharacter 使用角色 @return 无 */
    void Initialize(FBBBGrenadeRuntimeData &InData, const UBBBGrenadeDefinition &InDefinition,
                    ABBBCharacter &InCharacter);
    /** @return 无 依次调度使用与结束 */
    void Update();
    /** @return 无 解除依赖 */
    void Shutdown();

  private:
    FBBBGrenadeRuntimeData *Data = nullptr;
    const UBBBGrenadeDefinition *Definition = nullptr;
    ABBBCharacter *Character = nullptr;
};
