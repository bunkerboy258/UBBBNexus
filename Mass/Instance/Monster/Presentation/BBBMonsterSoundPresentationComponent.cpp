#include "BBBMonsterSoundPresentationComponent.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSoundPresentationDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitReactionFragment.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

UBBBMonsterSoundPresentationComponent::UBBBMonsterSoundPresentationComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    bAutoActivate = false;
    bAutoDestroy = false;
    bStopWhenOwnerDestroyed = true;
    bCanPlayMultipleInstances = false;
    bAllowSpatialization = true;
    bOverridePriority = true;
}

void UBBBMonsterSoundPresentationComponent::ResetPresentation()
{
    Stop();
    SetSound(nullptr);
    BoundInstance.Invalidate();
    BoundSettings = nullptr;
    VoiceIndex = INDEX_NONE;
    ObservedHitSerial = 0;
    ObservedActionId = 0;
    RequestedVoiceCount = 0;
    StartedVoiceCount = 0;
    LastVoiceEvent = NAME_None;
    ObservedState = EBBBMonsterBehavior::Idle;
    bDeathConsumed = false;
    bObservedCrawling = false;
    Priority = 1.0f;
    NextAmbientAt = 0.0f;
    NextHitAt = 0.0f;
    PreviousSound = nullptr;
}

void UBBBMonsterSoundPresentationComponent::RequestVoice(const TArray<TObjectPtr<USoundBase>>& Pool, const FName Event, const bool bAction)
{
    if (!ensureMsgf(BoundSettings && Pool.Num() > 0, TEXT("[UBBBM][ZombieAudio]声音配置或声音池无效")))
    {
        return;
    }

    ++RequestedVoiceCount;
    LastVoiceEvent = Event;
    const float RequestedPriority = Event == TEXT("Death") ? 5.0f
        : Event == TEXT("Hit") ? 4.0f
        : Event == TEXT("Attack") ? 3.0f
        : Event == TEXT("Alert") ? 2.0f : 1.0f;
    if (IsPlaying() && Priority > RequestedPriority)
    {
        return;
    }

    int32 Index = Random.RandRange(0, Pool.Num() - 1);
    if (Pool.Num() > 1 && Pool[Index] == PreviousSound)
    {
        Index = (Index + 1) % Pool.Num();
    }

    if (GetWorld()->GetNetMode() == NM_DedicatedServer
        || !UGameplayStatics::AreAnyListenersWithinRange(this, GetComponentLocation(), BoundSettings->AudibleDistance))
    {
        return;
    }

    Stop();
    SetSound(Pool[Index]);
    PreviousSound = Pool[Index];
    SetAttenuationSettings(BoundSettings->Attenuation);
    ConcurrencySet.Reset();
    ConcurrencySet.Add(bAction ? BoundSettings->ActionConcurrency : BoundSettings->AmbientConcurrency);
    Priority = RequestedPriority;
    SetVolumeMultiplier(BoundSettings->Voices[VoiceIndex].Volume);
    SetPitchMultiplier(EntityPitch);
    Play();
    if (IsPlaying())
    {
        ++StartedVoiceCount;
    }

    UE_LOG(LogTemp, Verbose, TEXT("[UBBBM][ZombieAudio]实例=%s 事件=%s 声线=%d 请求=%u 播放=%u"),
        *BoundInstance.ToString(), *Event.ToString(), VoiceIndex, RequestedVoiceCount, StartedVoiceCount);
}

void UBBBMonsterSoundPresentationComponent::ApplyFacts(const FGuid& Instance, UBBBMonsterSoundPresentationDefinition* Settings,
    const EBBBMonsterBehavior State, const uint32 ActionId, const float ActionProgress,
    const FBBBMonsterHitReactionFragment& Hit, const bool bCrawling, const float Speed, const float Now, const bool bNewActor)
{
    if (!Settings || !GetOwner() || GetOwner()->IsHidden())
    {
        ResetPresentation();
        return;
    }

    if (!ensureMsgf(Instance.IsValid() && FMath::IsFinite(Now) && FMath::IsFinite(Speed)
        && FMath::IsFinite(ActionProgress) && FMath::IsFinite(Hit.Age), TEXT("[UBBBM][ZombieAudio]身份或声音表现时间无效")))
    {
        ResetPresentation();
        return;
    }

    if (bNewActor || Instance != BoundInstance || Settings != BoundSettings)
    {
        ResetPresentation();
        if (!ensureMsgf(Settings->IsValid(), TEXT("[UBBBM][ZombieAudio]声音配置不完整或包含循环素材")))
        {
            return;
        }

        BoundInstance = Instance;
        BoundSettings = Settings;
        Random.Initialize(static_cast<int32>(GetTypeHash(Instance)));
        VoiceIndex = static_cast<int32>(GetTypeHash(Instance) % Settings->Voices.Num());
        EntityPitch = Random.FRandRange(Settings->PitchMin, Settings->PitchMax);
        ObservedState = State;
        ObservedActionId = ActionId;
        ObservedHitSerial = Hit.Serial;
        bObservedCrawling = bCrawling;
        bDeathConsumed = State == EBBBMonsterBehavior::Dead;
        const float InitialDelayMax = bCrawling ? Settings->CrawlIntervalMax
            : State == EBBBMonsterBehavior::Chase ? Settings->ChaseIntervalMax : Settings->IdleIntervalMax;
        NextAmbientAt = Now + Random.FRandRange(FMath::Min(0.5f, InitialDelayMax), InitialDelayMax);
        return;
    }

    const bool bNewAction = ActionId != ObservedActionId;
    const bool bNewHit = Hit.Serial != ObservedHitSerial;
    const bool bNewAlert = State == EBBBMonsterBehavior::Alert && ObservedState != EBBBMonsterBehavior::Alert;
    const bool bNewCrawl = bCrawling && !bObservedCrawling;
    const bool bNewChase = State == EBBBMonsterBehavior::Chase && ObservedState != EBBBMonsterBehavior::Chase;
    const FBBBMonsterSoundVoice& Voice = Settings->Voices[VoiceIndex];
    ObservedState = State;
    ObservedActionId = ActionId;
    ObservedHitSerial = Hit.Serial;
    bObservedCrawling = bCrawling;
    if (bNewChase && !bCrawling)
    {
        NextAmbientAt = FMath::Min(NextAmbientAt, Now + Random.FRandRange(0.2f, 0.8f));
    }

    if (State == EBBBMonsterBehavior::Dead)
    {
        if (!bDeathConsumed)
        {
            Stop();
            bDeathConsumed = true;
            RequestVoice(Voice.Death, TEXT("Death"), true);
        }

        return;
    }

    if (bDeathConsumed)
    {
        return;
    }

    if (bNewHit && Hit.Serial != 0 && Hit.Age >= 0.0f && Hit.Age <= Settings->HitMaxAge && Now >= NextHitAt)
    {
        NextHitAt = Now + Settings->HitInterval;
        NextAmbientAt = Now + Settings->CrawlIntervalMin;
        RequestVoice(Voice.Hit, TEXT("Hit"), true);
        return;
    }

    if (bNewAction && State == EBBBMonsterBehavior::Attack && ActionProgress >= 0.0f && ActionProgress <= 0.4f)
    {
        NextAmbientAt = Now + Settings->ChaseIntervalMin;
        RequestVoice(Voice.Attack, TEXT("Attack"), true);
        return;
    }

    if (bNewAlert)
    {
        NextAmbientAt = Now + Settings->ChaseIntervalMin;
        RequestVoice(Voice.Alert, TEXT("Alert"), true);
        return;
    }

    if (bNewCrawl)
    {
        NextAmbientAt = Now + Random.FRandRange(Settings->CrawlIntervalMin, Settings->CrawlIntervalMax);
        RequestVoice(Voice.Crawl, TEXT("Crawl"), false);
        return;
    }

    if (State == EBBBMonsterBehavior::Attack || State == EBBBMonsterBehavior::Alert || Now < NextAmbientAt || IsPlaying())
    {
        return;
    }

    if (bCrawling)
    {
        NextAmbientAt = Now + Random.FRandRange(Settings->CrawlIntervalMin, Settings->CrawlIntervalMax);
        RequestVoice(Voice.Crawl, TEXT("Crawl"), false);
        return;
    }

    if (State == EBBBMonsterBehavior::Chase)
    {
        const float Cadence = FMath::Clamp(300.0f / FMath::Max(Speed, 100.0f), 0.7f, 1.5f);
        NextAmbientAt = Now + Random.FRandRange(Settings->ChaseIntervalMin, Settings->ChaseIntervalMax) * Cadence;
        RequestVoice(Voice.Chase, TEXT("Chase"), false);
        return;
    }

    NextAmbientAt = Now + Random.FRandRange(Settings->IdleIntervalMin, Settings->IdleIntervalMax);
    RequestVoice(Voice.Idle, TEXT("Idle"), false);
}
