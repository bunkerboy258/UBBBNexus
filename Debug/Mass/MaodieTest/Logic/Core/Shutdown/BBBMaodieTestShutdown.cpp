#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/Core/Shutdown/BBBMaodieTestShutdown.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/BBBMaodieTest.h"
#include "AIController.h"
#include "Components/SkeletalMeshComponent.h"
void FBBBMaodieTestShutdown::Shutdown(ABBBMaodieTest &Actor)
{
    if (auto *Controller = Cast<AAIController>(Actor.GetController()))
    {
        Controller->StopMovement();
    }
    Actor.GetMesh()->SetMorphTarget(TEXT("Hiss"), 0.0f);
    Actor.RuntimeData.Behavior.State = {};
    Actor.RuntimeData.Animation.State = {};
    Actor.RuntimeData.Parse.State = {};
    Actor.RuntimeData.Network.State = {};
}
