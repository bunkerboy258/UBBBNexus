#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/Processors/BBBCharacterLifeObservationProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/Context/BBBCharacterNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"

void FBBBCharacterLifeObservationProcessor::Update(FBBBCharacterNetworkUpdateContext &Context) const
{
    auto &Observation = Context.Data.Network.LifeObservationState;
    const auto &Life = Context.Data.Life.ReadLifeState();
    const auto &Hit = Context.Data.Life.ReadHitState();
    if (!Life.bInitialized || Life.Revision <= Observation.Revision)
    {
        return;
    }

    const double Now = Context.Data.External.ReadWorldState().WorldTimeSeconds;
    const bool bImmediateEvent = Observation.Revision == 0
        || Observation.LastSentPhase != Life.Phase
        || !FMath::IsNearlyEqual(Observation.LastSentHealth, Life.Health, 0.01f)
        || !FMath::IsNearlyEqual(Observation.LastSentSpeedScale, Life.MovementSpeedScale, 0.001f)
        || Observation.LastSentHitSerial != Hit.Serial
        || Observation.LastSentDownedRevision != Life.DownedRevision
        || Observation.bLastSentRecoveryCrouched != Life.bRecoveryCrouched
        || (Life.Armor <= 0.0f && Observation.LastSentArmor > 0.0f);

    // 纯护甲缓慢自然回复期间按 0.15s 或 6 点变化节流 避免每帧触发可靠 RPC
    const bool bArmorThrottleReady = (Now - Observation.LastSentTimeSeconds) >= 0.15
        || FMath::Abs(Life.Armor - Observation.LastSentArmor) >= 6.0f;
    if (!bImmediateEvent && !bArmorThrottleReady)
    {
        return;
    }

    const auto CommitSent = [&]()
    {
        Observation.Revision = Life.Revision;
        Observation.LastSentPhase = Life.Phase;
        Observation.LastSentHealth = Life.Health;
        Observation.LastSentArmor = Life.Armor;
        Observation.LastSentSpeedScale = Life.MovementSpeedScale;
        Observation.LastSentHitSerial = Hit.Serial;
        Observation.LastSentDownedRevision = Life.DownedRevision;
        Observation.bLastSentRecoveryCrouched = Life.bRecoveryCrouched;
        Observation.LastSentTimeSeconds = Now;
    };

    if (Context.NetworkIdentityState.bHasAuthority)
    {
        Context.NetworkComponent.ReplicateLife(Life.Phase, Life.Health, Life.Armor, Life.Revision, Hit.Serial, Hit.Bone,
                                               Hit.Position, Hit.Direction, Life.DownedRevision, Life.bRecoveryCrouched, Life.MovementSpeedScale);
        CommitSent();
    }
    if (!Context.NetworkIdentityState.bHasAuthority && Context.NetworkIdentityState.bLocallyControlled)
    {
        Context.NetworkComponent.ServerSubmitLife(Life.Phase, Life.Health, Life.Armor, Life.Revision, Hit.Serial, Hit.Bone,
                                                  Hit.Position, Hit.Direction, Life.DownedRevision, Life.bRecoveryCrouched, Life.MovementSpeedScale);
        CommitSent();
    }
}
