
#include "BBBWork/UBBBNexus/Item/Definition/BBBItemRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Base/BBBItemDefinition.h"
#include "BBBWork/UBBBNexus/Item/Base/Fragment/BBBItemFragment.h"
#include "BBBWork/UBBBNexus/Item/Base/Fragment/BBBItemFragmentRuntimeData.h"

void UBBBItemRuntimeData::Initialize(const UBBBItemDefinition &Definition)
{

    RuntimeDataList.Reset();

    for (const UBBBItemFragment *Fragment : Definition.Fragments)
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
