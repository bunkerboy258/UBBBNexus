#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/Processors/BBBCharacterRescueMirrorProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/Context/BBBCharacterLifeUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"

void FBBBCharacterRescueMirrorProcessor::Update(FBBBCharacterLifeUpdateContext &Context) const
{
    auto &State = Context.Data.Life.RescueState;
    auto &Input = Context.Data.Life.RescueInputState;
    const double Now = Context.Data.External.ReadWorldState().WorldTimeSeconds;
    if (Context.Data.External.ReadNetworkIdentityState().bIsMirror)
    {
        for (int32 Index = 0; Index < Input.SnapshotRevisions.Num(); ++Index)
        {
            if (Input.SnapshotRevisions[Index] <= State.Revision)
            {
                continue;
            }
            State.Partner = Input.SnapshotPartners[Index];
            State.OperationId = Input.SnapshotOperations[Index];
            State.DownedRevision = Input.SnapshotRounds[Index];
            State.Revision = Input.SnapshotRevisions[Index];
            State.bHelping = Input.SnapshotHelping[Index];
            State.bReceiving = Input.SnapshotReceiving[Index];
            State.bAccepted = Input.SnapshotAccepted[Index];
            State.StartTime = Now - FMath::Max(0.0f, Input.SnapshotElapsed[Index]);
            State.Duration = FMath::Max(0.01f, Input.SnapshotDurations[Index]);
            State.EndReason = Input.SnapshotReasons[Index];
        }
        State.Progress = State.bAccepted && (State.bHelping || State.bReceiving)
            ? FMath::Clamp(float((Now - State.StartTime) / State.Duration), 0.0f, 1.0f) : 0.0f;
    }
    Input.SnapshotPartners.Reset();
    Input.SnapshotOperations.Reset();
    Input.SnapshotRounds.Reset();
    Input.SnapshotRevisions.Reset();
    Input.SnapshotHelping.Reset();
    Input.SnapshotReceiving.Reset();
    Input.SnapshotAccepted.Reset();
    Input.SnapshotElapsed.Reset();
    Input.SnapshotDurations.Reset();
    Input.SnapshotReasons.Reset();
}
