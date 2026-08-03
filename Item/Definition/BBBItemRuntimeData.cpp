
#include "BBBWork/UBBBNexus/Item/Definition/BBBItemRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Base/BBBItemDefinition.h"
#include "BBBWork/UBBBNexus/Item/Base/Fragment/BBBItemFragment.h"
#include "BBBWork/UBBBNexus/Item/Base/Fragment/BBBItemFragmentRuntimeData.h"

void UBBBItemRuntimeData::Initialize(const UBBBItemDefinition &Definition)
{

    RuntimeDataList.Reset();

    //收集定义中的全部 Fragment 命名插槽
    TArray<const UBBBItemFragment *> Fragments;
    Definition.CollectFragments(Fragments);

    for (const UBBBItemFragment *Fragment : Fragments)
    {

        if (!Fragment)
        {
            UE_LOG(LogTemp, Warning, TEXT("Item definition contains an invalid fragment"));
            continue;
        }

        UBBBItemFragmentRuntimeData *RuntimeData = Fragment->InitializeRuntimeData(*this);
        if (!RuntimeData)
        { continue; }

        RuntimeDataList.Add(RuntimeData);
    }
}
