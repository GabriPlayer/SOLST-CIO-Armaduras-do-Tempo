#include "SolsticioHUD.h"
#include "SolsticioMissionController.h"
#include "SolsticioArmorCharacter.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "SolsticioNPC.h"
#include "EngineUtils.h"

float ASolsticioHUD::DrawWrapped(const FString& Text, float X, float Y, float W, float Scale, const FLinearColor& Color)
{
    UFont* Font = GEngine->GetMediumFont();
    TArray<FString> Words;
    Text.ParseIntoArray(Words, TEXT(" "));
    FString Line;
    float LineH = 0.f;
    for (const FString& Word : Words)
    {
        const FString Test = Line.IsEmpty() ? Word : Line + TEXT(" ") + Word;
        float TW, TH;
        Canvas->TextSize(Font, Test, TW, TH, Scale, Scale);
        LineH = TH;
        if (TW > W && !Line.IsEmpty())
        {
            DrawText(Line, Color, X, Y, Font, Scale);
            Y += TH + 4.f;
            Line = Word;
        }
        else Line = Test;
    }
    if (!Line.IsEmpty()) { DrawText(Line, Color, X, Y, Font, Scale); Y += LineH + 4.f; }
    return Y;
}

void ASolsticioHUD::DrawHUD()
{
    Super::DrawHUD();
    const float W = Canvas->SizeX, H = Canvas->SizeY;

    if (const ASolsticioArmorCharacter* Armor = Cast<ASolsticioArmorCharacter>(GetOwningPawn()))
    {
        const float Pct = FMath::Clamp(Armor->SolarEnergy / Armor->MaxEnergy, 0.f, 1.f);
        DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.6f), 20.f, 20.f, 260.f, 24.f);
        DrawRect(FLinearColor(1.f, 0.75f, 0.1f, 1.f), 22.f, 22.f, 256.f * Pct, 20.f);
        DrawText(TEXT("ENERGIA SOLAR"), FLinearColor::White, 24.f, 48.f, GEngine->GetSmallFont(), 1.f);
    }

    {
        const ASolsticioMissionController* Ctl = Cast<ASolsticioMissionController>(GetOwningPlayerController());
        const FStoryStep* Cur = Ctl ? Ctl->GetStep() : nullptr;
        for (TActorIterator<ASolsticioNPC> It(GetWorld()); It; ++It)
        {
            const FVector P = Canvas->Project(It->GetActorLocation() + FVector(0, 0, It->bHologram ? 230.f : 200.f));
            if (P.Z <= 0.f) continue;
            const bool bTalking = Cur && Cur->Type == EStoryStepType::Line && Cur->Speaker == It->DisplayName;
            DrawText(It->DisplayName, bTalking ? FLinearColor(1.f, 0.85f, 0.2f) : FLinearColor::White, P.X - 20.f, P.Y, GEngine->GetMediumFont(), bTalking ? 1.3f : 1.f);
        }
    }
    const ASolsticioMissionController* PC = Cast<ASolsticioMissionController>(GetOwningPlayerController());
    if (!PC) return;

    DrawText(PC->MissionTitle + TEXT("  |  ") + PC->MissionPlace, FLinearColor(1.f, 0.85f, 0.4f), 20.f, 76.f, GEngine->GetSmallFont(), 1.f);

    if (PC->bFinished)
    {
        DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.6f), W * 0.2f, H * 0.4f, W * 0.6f, 90.f);
        DrawText(TEXT("Fim da Missão 1 (demo). Explora o laboratório: WASD, Espaço = propulsão, E = salto temporal."),
            FLinearColor::White, W * 0.2f + 16.f, H * 0.4f + 30.f, GEngine->GetMediumFont(), 1.f);
        return;
    }

    const FStoryStep* Step = PC->GetStep();
    if (!Step) return;

    const float BoxX = W * 0.1f, BoxW = W * 0.8f, BoxH = H * 0.28f, BoxY = H - BoxH - 30.f;
    DrawRect(FLinearColor(0.02f, 0.03f, 0.08f, 0.85f), BoxX, BoxY, BoxW, BoxH);

    if (Step->Type == EStoryStepType::Line)
    {
        const bool bNarr = Step->Speaker == TEXT("Narrador");
        DrawText(Step->Speaker, bNarr ? FLinearColor(0.6f, 0.7f, 0.9f) : FLinearColor(1.f, 0.8f, 0.3f), BoxX + 20.f, BoxY + 14.f, GEngine->GetMediumFont(), 1.2f);
        DrawWrapped(Step->Text, BoxX + 20.f, BoxY + 52.f, BoxW - 40.f, 1.1f, FLinearColor::White);
        DrawText(TEXT("[Enter / Clique] continuar"), FLinearColor(0.6f, 0.6f, 0.6f), BoxX + BoxW - 260.f, BoxY + BoxH - 28.f, GEngine->GetSmallFont(), 1.f);
        return;
    }

    float Y = DrawWrapped(Step->Text, BoxX + 20.f, BoxY + 14.f, BoxW - 40.f, 1.1f, FLinearColor::White);
    for (int32 i = 0; i < Step->Items.Num(); ++i)
    {
        const int32 Order = PC->Picked.IndexOfByKey(i);
        const FString Label = FString::Printf(TEXT("[%d] %s%s"), i + 1, *Step->Items[i], Order != INDEX_NONE ? *FString::Printf(TEXT("   ✔ #%d"), Order + 1) : TEXT(""));
        const float Col = (i % 2) * (BoxW * 0.5f);
        DrawText(Label, Order != INDEX_NONE ? FLinearColor::Green : FLinearColor::White, BoxX + 20.f + Col, Y + (i / 2) * 26.f, GEngine->GetMediumFont(), 1.f);
    }
    const FString Hint = PC->SequenceResult.IsEmpty() ? TEXT("Carrega nas teclas numéricas. Backspace desfaz.") : PC->SequenceResult + TEXT("  [Enter] continuar");
    DrawText(Hint, FLinearColor(1.f, 0.85f, 0.4f), BoxX + 20.f, BoxY + BoxH - 28.f, GEngine->GetSmallFont(), 1.f);
}


