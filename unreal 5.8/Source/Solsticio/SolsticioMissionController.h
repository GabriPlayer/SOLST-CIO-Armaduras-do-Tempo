#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SolsticioStory.h"
#include "SolsticioMissionController.generated.h"

enum class EPhase : uint8 { Loading, Step, MissionDone, Ending };

// Executa a campanha: dialogos, mini-jogos, escolhas, missoes e finais. O HUD le o estado publico.
UCLASS()
class SOLSTICIO_API ASolsticioMissionController : public APlayerController
{
    GENERATED_BODY()

public:
    ASolsticioMissionController();

    void StartCampaign();
    void LoadMission(int32 Index);

    FCampaign Campaign;
    int32 MissionIndex = 0;
    int32 Bond = 0;
    EPhase Phase = EPhase::Loading;
    float PhaseTime = 0.f;
    float MissionTime = 0.f;

    TArray<FStep> Steps;
    int32 StepIndex = 0;

    // Mini-jogos
    TArray<int32> Picked;
    TArray<int32> DisplayOrder;
    FString Message;
    float TimingPos = 0.f, TimingStart = 0.f;
    float SliderVal = 0.9f;
    TArray<FVector2D> TargetPos; // coordenadas de ecra normalizadas
    float TargetTimeLeft = 0.f;
    const FEnding* EndingData = nullptr;

    const FMission* GetMission() const { return Campaign.Missions.IsValidIndex(MissionIndex) ? &Campaign.Missions[MissionIndex] : nullptr; }
    const FStep* GetStep() const { return Phase == EPhase::Step && Steps.IsValidIndex(StepIndex) ? &Steps[StepIndex] : nullptr; }
    bool InMinigame() const;

protected:
    virtual void PlayerTick(float DeltaTime) override;

private:
    void EnterStep();
    void Advance();
    void FinishMission();
    bool Pressed(const FKey& Key) const { return WasInputKeyJustPressed(Key); }
    bool ClickOrEnter() const;
};
