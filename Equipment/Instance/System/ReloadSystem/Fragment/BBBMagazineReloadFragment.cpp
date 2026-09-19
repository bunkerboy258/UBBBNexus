#include "BBBWork/UBBBNexus/Equipment/Instance/System/ReloadSystem/Fragment/BBBMagazineReloadFragment.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Presentation/BBBMontagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/ReloadSystem/Definition/BBBEquipmentReloadContext.h"

bool FBBBMagazineReloadFragment::CanReload(
    const int32 LoadedAmmo,
    const int32 AmmoCapacity) const
{
    // 只有存在换弹动画且弹药未满时才能换弹
    return Montage && LoadedAmmo < AmmoCapacity;
}

bool FBBBMagazineReloadFragment::Begin(FBBBEquipmentReloadContext &Context) const
{
    // 先提交换弹动画确认表现可以开始
    if (!Montage)
    {
        return false;
    }

    FBBBMontagePacket Packet;
    Packet.Montage = Montage;
    Packet.PlayRate = 1.0f;
    Packet.Sequence = Context.Sequence;
    Packet.bReload = true;
    if (!Context.CharacterAPI.SubmitInput(Packet))
    {
        return false;
    }

    // 镜像实例只播放换弹表现不修改本地状态
    if (Context.bIsMirror)
    {
        return true;
    }

    Context.bIsReloading = true;
    Context.bMagazineDetached = false;
    Context.ReloadSequence = Context.Sequence;
    return true;
}

bool FBBBMagazineReloadFragment::DetachMagazine(FBBBEquipmentReloadContext &Context) const
{
    // 脱下弹匣后清空当前弹药
    Context.LoadedAmmo = 0;
    Context.bMagazineDetached = true;
    return true;
}

bool FBBBMagazineReloadFragment::LoadMagazine(FBBBEquipmentReloadContext &Context) const
{
    // 装入弹匣后恢复满弹并结束换弹
    Context.LoadedAmmo = Context.AmmoCapacity;
    Context.bIsReloading = false;
    return true;
}

bool FBBBMagazineReloadFragment::Cancel(FBBBEquipmentReloadContext &Context) const
{
    // 取消换弹时结束换弹状态
    Context.bIsReloading = false;
    return true;
}
