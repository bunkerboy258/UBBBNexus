#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/Core/Shutdown/BBBMaodieTestShutdown.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/BBBMaodieTest.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/AppearanceSystem/BBBMaodieTestAppearanceSystem.h"
#include "AIController.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/AudioComponent.h"
void FBBBMaodieTestShutdown::Shutdown(ABBBMaodieTest &Actor)
{
    FBBBMaodieTestAppearanceSystem::Shutdown(Actor);
    if (auto *Controller = Cast<AAIController>(Actor.GetController()))
    {
        Controller->StopMovement();
    }
    if (Actor.RuntimeData.Animation.State.ExpressionMaterial)
    {
        Actor.RuntimeData.Animation.State.ExpressionMaterial->SetScalarParameterValue(TEXT("Hiss"), 0.0f);
    }
    if (IsValid(Actor.RuntimeData.Animation.State.HissAudio))
    {
        Actor.RuntimeData.Animation.State.HissAudio->Stop();
    }
    Actor.RuntimeData.Behavior.State = {};
    Actor.RuntimeData.Animation.State = {};
    Actor.RuntimeData.Parse.State = {};
    Actor.RuntimeData.Network.State = {};
}
