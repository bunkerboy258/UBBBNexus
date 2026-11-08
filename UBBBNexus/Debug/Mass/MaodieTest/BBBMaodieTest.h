#pragma once
#include "GameFramework/Character.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Config/BBBMaodieTestConfig.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/RuntimeData/BBBMaodieTestRuntimeData.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/ParseSystem/BBBMaodieTestParseSystem.h"
#include "BBBMaodieTest.generated.h"
class UBBBMaodieTestNetworkComponent;
/** 可直接摆放的游荡哈气掉物测试角色 */
UCLASS(Blueprintable)
class ABBB_EVAC_API ABBBMaodieTest : public ACharacter
{
    GENERATED_BODY()
public:
    ABBBMaodieTest();
    /** @param Packet 最新镜像事实 @return 无 */
    template<typename TPacket>
    void SubmitInput(TPacket &&Packet)
    {
        FBBBMaodieTestParseSystem::Submit(RuntimeData, Forward<TPacket>(Packet));
    }
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    /** 唯一运行时黑板 */
    UPROPERTY(Transient)
    FBBBMaodieTestRuntimeData RuntimeData;
    /** @return 静态行为配置 */
    const FBBBMaodieTestConfig &GetTestConfig() const
    {
        return Config;
    }
protected:
    /** 可编辑行为资源与参数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "耄耋", meta = (DisplayName = "耄耋测试配置"))
    FBBBMaodieTestConfig Config;
private:
    friend class FBBBMaodieTestInitializer;
    friend class FBBBMaodieTestNetworkProcessor;
    /** 当前表现事实的网络桥接 */
    UPROPERTY()
    TObjectPtr<UBBBMaodieTestNetworkComponent> Network;
};
