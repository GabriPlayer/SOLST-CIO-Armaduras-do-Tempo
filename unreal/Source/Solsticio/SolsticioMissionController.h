#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SolsticioMissionController.generated.h"

UENUM()
enum class EStoryStepType : uint8 { Line, Sequence };

USTRUCT()
struct FStoryStep
{
    GENERATED_BODY()
    EStoryStepType Type = EStoryStepType::Line;
    FString Speaker;
    FString Text;
    TArray<FString> Items;
};

// Executa os passos de uma missao: dialogo e mini-jogos. O HUD le o estado publico.
UCLASS()
class SOLSTICIO_API ASolsticioMissionController : public APlayerController
{
    GENERATED_BODY()

public:
    ASolsticioMissionController();

    void StartMission(const FString& InTitle, const FString& InPlace, const TArray<FStoryStep>& InSteps);

    FString MissionTitle;
    FString MissionPlace;
    TArray<FStoryStep> Steps;
    int32 StepIndex = INDEX_NONE;
    bool bFinished = false;

    // Mini-jogo de sequencia: ordem escolhida pelo jogador (indices de Items).
    TArray<int32> Picked;
    FString SequenceResult;

    const FStoryStep* GetStep() const { return Steps.IsValidIndex(StepIndex) ? &Steps[StepIndex] : nullptr; }
    bool InDialogue() const { return StepIndex != INDEX_NONE && !bFinished; }

protected:
    virtual void PlayerTick(float DeltaTime) override;

private:
    void Advance();
    void EnterStep();
};
