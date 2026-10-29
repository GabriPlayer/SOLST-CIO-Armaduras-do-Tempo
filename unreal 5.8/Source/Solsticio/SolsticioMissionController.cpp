#include "SolsticioMissionController.h"
#include "SolsticioGameMode.h"
#include "Engine/World.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/Pawn.h"

static FKey NumKey(int32 I)
{
    static const FKey K[9] = { EKeys::One, EKeys::Two, EKeys::Three, EKeys::Four, EKeys::Five, EKeys::Six, EKeys::Seven, EKeys::Eight, EKeys::Nine };
    return K[FMath::Clamp(I, 0, 8)];
}

ASolsticioMissionController::ASolsticioMissionController()
{
    PrimaryActorTick.bCanEverTick = true;
}

bool ASolsticioMissionController::InMinigame() const
{
    const FStep* S = GetStep();
    return S && S->Type != EStep::Line;
}

bool ASolsticioMissionController::ClickOrEnter() const
{
    return Pressed(EKeys::Enter) || Pressed(EKeys::LeftMouseButton);
}

void ASolsticioMissionController::StartCampaign()
{
    Bond = 0;
    if (Campaign.Missions.Num() == 0 && !Campaign.Load())
    {
        UE_LOG(LogTemp, Error, TEXT("campaign.json em falta"));
        return;
    }
    UE_LOG(LogTemp, Display, TEXT("Solsticio: campanha com %d missoes"), Campaign.Missions.Num());
    if (FParse::Param(FCommandLine::Get(), TEXT("SolsticioTest")))
        for (int32 i = Campaign.Missions.Num() - 1; i >= 0; --i) { LoadMission(i); UE_LOG(LogTemp, Display, TEXT("Solsticio: missao %d ok"), i + 1); }
    LoadMission(0);
}

void ASolsticioMissionController::LoadMission(int32 Index)
{
    MissionIndex = Index;
    const FMission& M = Campaign.Missions[Index];
    Steps = M.Steps;
    StepIndex = -1;
    MissionTime = 0.f;
    Phase = EPhase::Loading;
    if (ASolsticioGameMode* GM = GetWorld()->GetAuthGameMode<ASolsticioGameMode>())
        GM->BuildMission(Index, M.Speakers);
    Phase = EPhase::Step;
    Advance();
}

void ASolsticioMissionController::Advance()
{
    ++StepIndex;
    if (StepIndex >= Steps.Num()) { FinishMission(); return; }
    EnterStep();
}

void ASolsticioMissionController::FinishMission()
{
    Phase = EPhase::MissionDone;
    PhaseTime = 0.f;
    bShowMouseCursor = false;
    ResetIgnoreLookInput();
}

void ASolsticioMissionController::EnterStep()
{
    PhaseTime = 0.f;
    Message.Empty();
    Picked.Empty();
    TargetPos.Empty();
    ResetIgnoreLookInput();
    bShowMouseCursor = false;

    const FStep& S = Steps[StepIndex];
    switch (S.Type)
    {
    case EStep::Timing:
        TimingStart = FMath::FRandRange(0.05f, 0.95f - S.Width);
        break;
    case EStep::Slider:
        SliderVal = 0.9f;
        break;
    case EStep::Targets:
        for (int32 i = 0; i < S.Count; ++i)
            TargetPos.Add(FVector2D(FMath::FRandRange(0.18f, 0.82f), FMath::FRandRange(0.22f, 0.62f)));
        TargetTimeLeft = S.Time;
        bShowMouseCursor = true;
        SetIgnoreLookInput(true);
        break;
    case EStep::Sequence:
        DisplayOrder.Empty();
        for (int32 i = 0; i < S.Items.Num(); ++i) DisplayOrder.Add(i);
        break;
    default: break;
    }
}

void ASolsticioMissionController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);
    PhaseTime += DeltaTime;
    MissionTime += DeltaTime;

    if (Pressed(EKeys::F9) && Phase == EPhase::Step) { FinishMission(); return; }

    if (Phase == EPhase::MissionDone)
    {
        if (PhaseTime > 0.5f && ClickOrEnter())
        {
            if (Campaign.Missions.IsValidIndex(MissionIndex + 1)) LoadMission(MissionIndex + 1);
            else
            {
                Phase = EPhase::Ending;
                PhaseTime = 0.f;
                EndingData = nullptr;
                for (const FEnding& E : Campaign.Endings)
                    if (Bond >= E.Min && (!EndingData || E.Min > EndingData->Min)) EndingData = &E;
                if (!EndingData && Campaign.Endings.Num()) EndingData = &Campaign.Endings.Last();
            }
        }
        return;
    }
    if (Phase == EPhase::Ending)
    {
        if (PhaseTime > 1.f && ClickOrEnter()) StartCampaign();
        return;
    }
    if (Phase != EPhase::Step || !Steps.IsValidIndex(StepIndex)) return;

    const FStep S = Steps[StepIndex];
    switch (S.Type)
    {
    case EStep::Line:
        if (PhaseTime > 0.25f && ClickOrEnter()) Advance();
        break;

    case EStep::Choice:
        for (int32 i = 0; i < S.Options.Num(); ++i)
        {
            if (!Pressed(NumKey(i))) continue;
            const FChoiceOpt& O = S.Options[i];
            Bond += O.Bond;
            int32 At = StepIndex + 1;
            for (const FLineData& L : O.Lines)
            {
                FStep Line; Line.Type = EStep::Line; Line.Who = L.Who; Line.Text = L.Text;
                Steps.Insert(Line, At++);
            }
            Advance();
            return;
        }
        break;

    case EStep::Timing:
        TimingPos = FMath::Abs(FMath::Fmod(PhaseTime * S.Speed, 2.f) - 1.f);
        if (PhaseTime > 0.2f && (Pressed(EKeys::SpaceBar) || Pressed(EKeys::Enter) || Pressed(EKeys::LeftMouseButton)))
        {
            if (TimingPos >= TimingStart && TimingPos <= TimingStart + S.Width) Advance();
            else { Message = TEXT("Falhou — tenta outra vez."); TimingStart = FMath::FRandRange(0.05f, 0.95f - S.Width); }
        }
        break;

    case EStep::Slider:
    {
        const float Dir = (IsInputKeyDown(EKeys::D) || IsInputKeyDown(EKeys::Right) ? 1.f : 0.f)
            - (IsInputKeyDown(EKeys::A) || IsInputKeyDown(EKeys::Left) ? 1.f : 0.f);
        SliderVal = FMath::Clamp(SliderVal + Dir * 0.35f * DeltaTime, 0.f, 1.f);
        if (PhaseTime > 0.2f && (Pressed(EKeys::Enter) || Pressed(EKeys::SpaceBar)))
        {
            if (FMath::Abs(SliderVal - S.Target) <= S.Tol) Advance();
            else Message = TEXT("Ângulo errado — ajusta e confirma outra vez.");
        }
        break;
    }

    case EStep::Targets:
    {
        TargetTimeLeft -= DeltaTime;
        if (Pressed(EKeys::LeftMouseButton))
        {
            float MX, MY; int32 VX, VY;
            GetMousePosition(MX, MY);
            GetViewportSize(VX, VY);
            for (int32 i = TargetPos.Num() - 1; i >= 0; --i)
            {
                const FVector2D P(TargetPos[i].X * VX, TargetPos[i].Y * VY);
                if (FVector2D::Distance(P, FVector2D(MX, MY)) < 40.f) { TargetPos.RemoveAt(i); break; }
            }
            if (TargetPos.Num() == 0) { Advance(); return; }
        }
        if (TargetTimeLeft <= 0.f)
        {
            EnterStep();
            Message = TEXT("Tempo esgotado — tenta de novo.");
        }
        break;
    }

    case EStep::Sequence:
        for (int32 d = 0; d < DisplayOrder.Num() && d < 9; ++d)
        {
            if (!Pressed(NumKey(d))) continue;
            const int32 Item = DisplayOrder[d];
            if (Picked.Contains(Item)) continue;
            if (S.Ordered.Num() && S.Ordered[Picked.Num()] != S.Items[Item])
            {
                Picked.Empty();
                Message = TEXT("Sequência errada — o sistema reinicia.");
                break;
            }
            Picked.Add(Item);
            Message.Empty();
            if (Picked.Num() == S.Items.Num()) { Advance(); return; }
        }
        if (Pressed(EKeys::BackSpace) && Picked.Num() && S.Ordered.Num() == 0) Picked.Pop();
        break;
    }
}
