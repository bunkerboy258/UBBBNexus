#include "BBBWork/UBBBNexus/Notify/Character/Display/BBBCharacterNewMagazineDisplayAnimNotifyState.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"

namespace
{
    const FName NewMagazineTag(TEXT("BBB_CharacterNewHandMagazine"));

    UStaticMeshComponent *FindNewMagazine(USkeletalMeshComponent &HandMesh)
    {
        TArray<USceneComponent *> Children;
        HandMesh.GetChildrenComponents(false, Children);

        for (USceneComponent *Child : Children)
        {
            UStaticMeshComponent *Magazine = Cast<UStaticMeshComponent>(Child);
            if (Magazine && Magazine->ComponentHasTag(NewMagazineTag))
            {
                return Magazine;
            }
        }

        return nullptr;
    }
}

void UBBBCharacterNewMagazineDisplayAnimNotifyState::NotifyBegin(
    USkeletalMeshComponent *MeshComp,
    UAnimSequenceBase *,
    float,
    const FAnimNotifyEventReference &)
{
    ABBBCharacter *Character = MeshComp ? Cast<ABBBCharacter>(MeshComp->GetOwner()) : nullptr;
    if (!Character)
    {
        return;
    }

    if (!ensureMsgf(IsInGameThread() && MagazineMesh
        && MeshComp->GetBoneIndex(HandBoneName) != INDEX_NONE
        && !HandMagazineTransform.ContainsNaN()
        && !FindNewMagazine(*MeshComp),
        TEXT("新弹匣角色表现缺少网格 骨骼 位置或存在重复组件 %s"), *Character->GetName()))
    {
        return;
    }

    // 新弹匣仅是角色动画的临时视觉对象 不读取装备也不改变装填状态
    FName MagazineName = NAME_None;
    EObjectFlags MagazineFlags = RF_Transient;

#if WITH_EDITOR
    // 编辑器中的临时实例使用可辨认的名称和事务标记 便于选中后调整手部相对变换
    MagazineName = MakeUniqueObjectName(Character, UStaticMeshComponent::StaticClass(), TEXT("BBB_NewHandMagazine"));
    MagazineFlags |= RF_Transactional;
#endif

    UStaticMeshComponent *Magazine = NewObject<UStaticMeshComponent>(Character, MagazineName, MagazineFlags);
    if (!ensureMsgf(Magazine, TEXT("新弹匣角色表现创建组件失败 %s"), *Character->GetName()))
    {
        return;
    }

    Magazine->ComponentTags.Add(NewMagazineTag);
    Magazine->SetStaticMesh(MagazineMesh);
    Magazine->SetMobility(EComponentMobility::Movable);
    Magazine->SetCollisionEnabled(ECollisionEnabled::NoCollision);

#if WITH_EDITOR
    // 实例组件注册使详情面板开放变换编辑 销毁时引擎同步移除实例列表 不写回通知资产
    Character->AddInstanceComponent(Magazine);
    Magazine->bSelectable = true;
    Magazine->bWantsEditorEffects = true;
#endif

    Magazine->RegisterComponent();
    if (!ensureMsgf(Magazine->IsRegistered()
        && Magazine->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, HandBoneName),
        TEXT("新弹匣角色表现注册或手部附着失败 %s"), *Character->GetName()))
    {
        Magazine->DestroyComponent();
        return;
    }

    Magazine->SetRelativeTransform(HandMagazineTransform);

    UE_LOG(LogTemp, Log, TEXT("[BBBRifleMagazine] New magazine spawned on character hand Character=%s"), *Character->GetName());
}

void UBBBCharacterNewMagazineDisplayAnimNotifyState::NotifyEnd(
    USkeletalMeshComponent *MeshComp,
    UAnimSequenceBase *,
    const FAnimNotifyEventReference &)
{
    if (!MeshComp)
    {
        return;
    }

    UStaticMeshComponent *Magazine = FindNewMagazine(*MeshComp);
    if (!Magazine)
    {
        return;
    }

    Magazine->DestroyComponent();
    UE_LOG(LogTemp, Log, TEXT("[BBBRifleMagazine] New magazine display ended Mesh=%s"), *MeshComp->GetName());
}
