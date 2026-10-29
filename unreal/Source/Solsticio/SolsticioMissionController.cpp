#include "SolsticioMissionController.h"
#include "SolsticioHUD.h"

ASolsticioMissionController::ASolsticioMissionController()
{
    bShowMouseCursor = false;
}

void ASolsticioMissionController::StartMission(const FString& InTitle, const FString& InPlace, const TArray<FStoryStep>& InSteps)
{
    MissionTitle = InTitle;
    MissionPlace = InPlace;
    Steps = InSteps;
    StepIndex = -1;
    bFinished = false;
    Advance();
}

void ASolsticioMissionController::EnterStep()
{
    Picked.Reset();
    SequenceResult.Reset();
    SetIgnoreMoveInput(InDialogue());
    SetIgnoreLookInput(InDialogue());
}

void ASolsticioMissionController::Advance()
{
    StepIndex++;
    if (!Steps.IsValidIndex(StepIndex))
    {
        bFinished = true;
        StepIndex = Steps.Num() - 1;
        SetIgnoreMoveInput(false);
        SetIgnoreLookInput(false);
        return;
    }
    EnterStep();
}

void ASolsticioMissionController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);

    const FStoryStep* Step = GetStep();
    if (!Step || bFinished) return;

    if (Step->Type == EStoryStepType::Line)
    {
        if (WasInputKeyJustPressed(EKeys::Enter) || WasInputKeyJustPressed(EKeys::LeftMouseButton))
            Advance();
        return;
    }

    // Sequencia: teclas 1..N ativam modulos; Backspace desfaz.
    const bool bDone = Picked.Num() >= Step->Items.Num();
    if (!bDone)
    {
        static const FKey Keys[] = { EKeys::One, EKeys::Two, EKeys::Three, EKeys::Four, EKeys::Five, EKeys::Six, EKeys::Seven, EKeys::Eight, EKeys::Nine };
        for (int32 i = 0; i < Step->Items.Num() && i < UE_ARRAY_COUNT(Keys); ++i)
        {
            if (WasInputKeyJustPressed(Keys[i]) && !Picked.Contains(i))
            {
                Picked.Add(i);
                if (Picked.Num() == Step->Items.Num())
                    SequenceResult = FString::Printf(TEXT("Último módulo ativado: %s. Ficará mais danificado na estação solar."), *Step->Items[i]);
            }
        }
        if (WasInputKeyJustPressed(EKeys::BackSpace) && Picked.Num() > 0) Picked.Pop();
    }
    else if (WasInputKeyJustPressed(EKeys::Enter))
    {
        Advance();
    }
}
