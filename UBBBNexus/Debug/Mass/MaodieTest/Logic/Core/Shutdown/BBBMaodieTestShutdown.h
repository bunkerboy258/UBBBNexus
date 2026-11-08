#pragma once
class ABBBMaodieTest;
/** 耄耋生命周期调度 */
class FBBBMaodieTestShutdown final
{
public:
    /** @param Actor 耄耋 @return 无 */
    static void Shutdown(ABBBMaodieTest &Actor);
};
