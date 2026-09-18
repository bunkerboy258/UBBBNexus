#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Input/BBBInputPipeline.h"
#include "BBBWork/UBBBNexus/Character/Instance/Runtime/BBBCharacterRuntimeData.h"

void FBBBInputPipeline::Update() const
{
    if (!ensureMsgf(Data, TEXT("[UBBBC]Input pipeline is uninitialized")))
    {
        return;
    }
    // 交换收件箱后新到达的动画与装备反馈进入下一帧 防止重入修改当前批次
    Data->Input.Frame = MoveTemp(Data->Input.Pending);
    Data->Input.Pending = FBBBInputBatch();
    Data->Input.FrameControl = Data->Input.Control;
    Data->Input.Control.bJump = false;
}
