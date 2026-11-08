#pragma once
class ABBBMaodieTest;

/** 耄耋本机专用补光与橙色内侧轮廓处理 */
class FBBBMaodieTestAppearanceProcessor final
{
public:
    /** @param Actor 耄耋 @param DeltaSeconds 帧间隔 @return 无 */
    static void Update(ABBBMaodieTest &Actor, float DeltaSeconds);

    /** @param Actor 耄耋 @return 无 恢复身体显示属性并销毁专用灯 */
    static void Shutdown(ABBBMaodieTest &Actor);
};
