#pragma once

class ABBBCharacter;

/** 集中收束角色更新与持有的运行时实体 */
class FBBBCharacterShutdown final
{
public:
    /**
     * 销毁角色持有的运行时实体并停止后更新
     * @param Character	结束运行的角色
     * @return 无
     */
    static void Shutdown(ABBBCharacter &Character);
};
