
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Input/BBBInputTypes.h"

struct FBBBInputRawData
{

    void SetMoveAxis(const FVector2D &Value)
    {
        RawInput.MoveAxis = Value;
    }

    void SetLookAxis(const FVector2D &Value)
    {
        RawInput.LookAxis = Value;
    }

    void SetFireHeld(bool bHeld)
    {
        RawInput.bFireHeld = bHeld;
    }

    void MarkFireStarted()
    {
        RawInput.bFireStarted = true;
    }

    void MarkFireCompleted()
    {
        RawInput.bFireCompleted = true;
    }

    void SetPrecisionAimHeld(bool bHeld)
    {
        RawInput.bPrecisionAimHeld = bHeld;
    }

    void MarkPrecisionAimStarted()
    {
        RawInput.bPrecisionAimStarted = true;
    }

    void MarkPrecisionAimCompleted()
    {
        RawInput.bPrecisionAimCompleted = true;
    }

    void SetSprintHeld(bool bHeld)
    {
        RawInput.bSprintHeld = bHeld;
    }

    void MarkReloadPressed()
    {
        RawInput.bReloadPressed = true;
    }

    void MarkEquipSlot1Pressed()
    {
        RawInput.bEquipSlot1Pressed = true;
    }

    void MarkEquipSlot2Pressed()
    {
        RawInput.bEquipSlot2Pressed = true;
    }

    const FBBBRawInputFrame &GetAccumulatedRawInput() const
    {
        return RawInput;
    }

    void CleanFrameInput()
    {
        //增量 不得跨帧保留
        RawInput.LookAxis = FVector2D::ZeroVector;

        RawInput.bFireStarted = false;

        RawInput.bFireCompleted = false;

        RawInput.bPrecisionAimStarted = false;

        RawInput.bPrecisionAimCompleted = false;

        RawInput.bReloadPressed = false;

        RawInput.bEquipSlot1Pressed = false;

        RawInput.bEquipSlot2Pressed = false;
    }
    
private:

    FBBBRawInputFrame RawInput;
    
};
