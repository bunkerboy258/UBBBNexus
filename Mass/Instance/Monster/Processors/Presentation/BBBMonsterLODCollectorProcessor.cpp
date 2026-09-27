#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Presentation/BBBMonsterLODCollectorProcessor.h"

UBBBMonsterLODCollectorProcessor::UBBBMonsterLODCollectorProcessor()
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
}
