#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SolsticioGameMode.generated.h"

UCLASS()
class SOLSTICIO_API ASolsticioGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ASolsticioGameMode();

protected:
    virtual void BeginPlay() override;

private:
    void BuildLab();
    void SpawnBox(const FVector& Center, const FVector& Size, const FLinearColor& Color);
    void StartMission1();
};
