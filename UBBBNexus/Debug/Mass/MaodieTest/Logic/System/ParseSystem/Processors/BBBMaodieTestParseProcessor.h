#pragma once
class ABBBMaodieTest;
/** 耄耋Parse逻辑处理器 */
class FBBBMaodieTestParseProcessor final
{
public:
    /** @param Actor 耄耋 @param DeltaSeconds 帧间隔 @param bCausal 是否为业务控制端 @return 无 */
    static void Update(ABBBMaodieTest &Actor, float DeltaSeconds, bool bCausal);
};
