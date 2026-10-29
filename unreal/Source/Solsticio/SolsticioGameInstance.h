#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SolsticioTypes.h"
#include "SolsticioGameInstance.generated.h"

// Estado persistente da campanha: missao atual e ligacao Tim-Iris.
UCLASS()
class SOLSTICIO_API USolsticioGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;

    UPROPERTY(BlueprintReadOnly) TArray<FMissionInfo> Missions;
    UPROPERTY(BlueprintReadWrite) int32 CurrentMission = 0;
    UPROPERTY(BlueprintReadWrite) int32 Bond = 0;

    UFUNCTION(BlueprintCallable) void AddBond(int32 Delta) { Bond += Delta; }
    UFUNCTION(BlueprintCallable) bool GetCurrentMission(FMissionInfo& Out) const;
    UFUNCTION(BlueprintCallable) void StartCurrentMission();
    UFUNCTION(BlueprintCallable) void CompleteMission();
    UFUNCTION(BlueprintPure) FString GetEndingTitle() const;

private:
    void LoadMissions();
};
