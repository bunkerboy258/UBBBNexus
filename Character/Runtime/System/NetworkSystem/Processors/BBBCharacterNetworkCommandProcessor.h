#pragma once

struct FBBBCharacterInputFrame;
class UBBBCharacterNetworkComponent;

/** 将本机客户端输入翻译为发往权威角色的网络命令 */
class FBBBCharacterNetworkCommandProcessor final
{
public:
    /**
     * 上传仍保持激活的客户端命令并消费对应槽位
     * @param InputFrame        角色固定输入帧
     * @param NetworkComponent  角色网络传输组件
     */
    void Update(
        FBBBCharacterInputFrame &InputFrame,
        UBBBCharacterNetworkComponent &NetworkComponent) const;
};
