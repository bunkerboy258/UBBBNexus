#pragma once

class FBBBCharacterNetworkSystem;

/** 观察非权威本机角色的已确认事实并提交服务器 */
class FBBBNetworkClientLocalProcessor final
{
public:
    void Update(FBBBCharacterNetworkSystem &System) const;
};
