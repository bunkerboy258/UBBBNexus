#include "BBBMonsterSoundPresentationDefinition.h"

#include "Sound/SoundAttenuation.h"
#include "Sound/SoundConcurrency.h"
#include "Sound/SoundBase.h"

bool UBBBMonsterSoundPresentationDefinition::IsValid() const
{
    if (Voices.IsEmpty() || !Attenuation || !AmbientConcurrency || !ActionConcurrency || !ContactConcurrency
        || !FMath::IsFinite(AudibleDistance) || AudibleDistance <= 0.0f
        || !FMath::IsFinite(IdleIntervalMin) || IdleIntervalMin <= 0.0f
        || !FMath::IsFinite(IdleIntervalMax) || IdleIntervalMax < IdleIntervalMin
        || !FMath::IsFinite(ChaseIntervalMin) || ChaseIntervalMin <= 0.0f
        || !FMath::IsFinite(ChaseIntervalMax) || ChaseIntervalMax < ChaseIntervalMin
        || !FMath::IsFinite(CrawlIntervalMin) || CrawlIntervalMin <= 0.0f
        || !FMath::IsFinite(CrawlIntervalMax) || CrawlIntervalMax < CrawlIntervalMin
        || !FMath::IsFinite(HitInterval) || HitInterval <= 0.0f
        || !FMath::IsFinite(HitMaxAge) || HitMaxAge <= 0.0f
        || !FMath::IsFinite(PitchMin) || PitchMin < 0.5f
        || !FMath::IsFinite(PitchMax) || PitchMax < PitchMin || PitchMax > 2.0f)
    {
        return false;
    }

    TSet<FName> Names;
    for (const auto* Pool : {&Footsteps, &CrawlFriction, &Landings})
    {
        if (Pool->IsEmpty())
        {
            return false;
        }
        for (const USoundBase* Sound : *Pool)
        {
            if (!::IsValid(Sound) || Sound->IsLooping() || !FMath::IsFinite(Sound->GetDuration()) || Sound->GetDuration() <= 0.0f)
            {
                return false;
            }
        }
    }
    for (const FBBBMonsterSoundVoice& Voice : Voices)
    {
        if (Voice.Name.IsNone() || Names.Contains(Voice.Name) || !FMath::IsFinite(Voice.Volume) || Voice.Volume <= 0.0f || Voice.Volume > 4.0f)
        {
            return false;
        }

        Names.Add(Voice.Name);
        for (const auto* Pool : { &Voice.Idle, &Voice.Alert, &Voice.Chase, &Voice.Attack, &Voice.Hit, &Voice.Death, &Voice.Crawl })
        {
            if (Pool->IsEmpty())
            {
                return false;
            }

            for (const USoundBase* Sound : *Pool)
            {
                if (!::IsValid(Sound) || Sound->IsLooping() || !FMath::IsFinite(Sound->GetDuration()) || Sound->GetDuration() <= 0.0f)
                {
                    return false;
                }
            }
        }
    }

    return Attenuation->Attenuation.bAttenuate && Attenuation->Attenuation.bSpatialize
        && AmbientConcurrency->Concurrency.MaxCount > 0 && !AmbientConcurrency->Concurrency.bLimitToOwner
        && AmbientConcurrency->Concurrency.GetMaxCount() == AmbientConcurrency->Concurrency.MaxCount
        && ActionConcurrency->Concurrency.MaxCount > 0 && !ActionConcurrency->Concurrency.bLimitToOwner
        && ActionConcurrency->Concurrency.GetMaxCount() == ActionConcurrency->Concurrency.MaxCount
        && ContactConcurrency->Concurrency.MaxCount > 0 && !ContactConcurrency->Concurrency.bLimitToOwner;
}
