#pragma once

class FBBBCharacterNetworkSystem;

/** 非权威远端角色只等待网络组件投递还原事实 */
class FBBBNetworkClientRemoteProcessor final
{
public:
    void Update(FBBBCharacterNetworkSystem &System) const;
};
