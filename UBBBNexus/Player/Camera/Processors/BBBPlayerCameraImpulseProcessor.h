#pragma once

class ABBBPlayerCameraSystem;

/** 消费相机冲击并更新最终相机姿态 */
class FBBBPlayerCameraImpulseProcessor final
{
public:
    /**
     * 更新相机冲击恢复和跟随姿态
     * @param CameraSystem	所属相机
     * @param DeltaSeconds	本帧间隔
     * @return 无
     */
    static void Update(ABBBPlayerCameraSystem &CameraSystem, float DeltaSeconds);
};
