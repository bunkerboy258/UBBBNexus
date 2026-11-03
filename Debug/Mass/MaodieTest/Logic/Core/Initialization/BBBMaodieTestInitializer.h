#pragma once
class ABBBMaodieTest;
/** 耄耋生命周期调度 */
class FBBBMaodieTestInitializer final
{
public:
    /** @param Actor 耄耋 @return 无 */
    static void Construct(ABBBMaodieTest &Actor);
    static void Initialize(ABBBMaodieTest &Actor);
};
