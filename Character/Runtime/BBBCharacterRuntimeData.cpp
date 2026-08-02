
#include "BBBWork/UBBBNexus/Character/Runtime/BBBCharacterRuntimeData.h"

void FBBBCharacterRuntimeData::Clean()
{
    //动作请求与仲裁结果只允许在当前帧消费
    CharacterData.Decision.CleanFrame();

    //按下松开等输入边沿只允许触发一次
    InputRawData.CleanFrameInput();

    //行为意图由下一帧输入重新生成
    CharacterData.Intent.CleanFrame();

    //动画播放请求提交后立即清空
    CharacterData.Animation.CleanFrame();

    //物品命令与事件完成处理后立即清空
    CharacterData.Item.CleanFrame();
}
