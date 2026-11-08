#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/BBBMaodieTest.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/Core/Initialization/BBBMaodieTestInitializer.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/Core/Update/BBBMaodieTestUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/Core/Shutdown/BBBMaodieTestShutdown.h"
ABBBMaodieTest::ABBBMaodieTest()
{
    FBBBMaodieTestInitializer::Construct(*this);
}
void ABBBMaodieTest::BeginPlay()
{
    Super::BeginPlay();
    FBBBMaodieTestInitializer::Initialize(*this);
}
void ABBBMaodieTest::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    FBBBMaodieTestUpdatePipeline::Update(*this, DeltaSeconds);
}
void ABBBMaodieTest::EndPlay(const EEndPlayReason::Type Reason)
{
    FBBBMaodieTestShutdown::Shutdown(*this);
    Super::EndPlay(Reason);
}
