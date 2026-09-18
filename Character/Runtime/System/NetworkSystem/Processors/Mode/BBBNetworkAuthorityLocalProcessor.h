#pragma once

class FBBBCharacterNetworkSystem;

/** 观察权威且本机控制角色的已确认事实 */
class FBBBNetworkAuthorityLocalProcessor final
{
public:
    void Update(FBBBCharacterNetworkSystem &System) const;
};
