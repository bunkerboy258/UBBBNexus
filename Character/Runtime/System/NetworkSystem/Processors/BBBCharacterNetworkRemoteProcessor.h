#pragma once

/**
 * 普通远端角色的网络扩展点
 *
 * 当前远端角色只接收网络组件投递的还原输入，不主动生成或上传事实
 */
class FBBBCharacterNetworkRemoteProcessor final
{
public:
    /** 当前无主动工作，保留统一Update入口供后续明确需求扩展 */
    void Update() const;
};
