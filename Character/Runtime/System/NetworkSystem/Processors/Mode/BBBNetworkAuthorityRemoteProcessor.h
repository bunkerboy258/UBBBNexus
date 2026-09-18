#pragma once

class FBBBCharacterNetworkSystem;

/** 权威远端角色不自行产生角色事实 */
class FBBBNetworkAuthorityRemoteProcessor final
{
public:
    void Update(FBBBCharacterNetworkSystem &System) const;
};
