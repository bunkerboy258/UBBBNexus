
#pragma once
class FBBBCharacterItemStorage;
struct FBBBCharacterItemConfig;
struct FBBBCharacterItemRuntimeData;

class FBBBCharacterDefaultItemInitializer final
{
public:

    void Initialize(
        FBBBCharacterItemRuntimeData &ItemData,
        FBBBCharacterItemStorage &Storage,
        UObject &ItemOuter,
        const FBBBCharacterItemConfig &InItemConfig) const;
};
