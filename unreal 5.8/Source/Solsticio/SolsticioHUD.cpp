#include "SolsticioHUD.h"
#include "SolsticioArmorCharacter.h"
#include "SolsticioMissionController.h"
#include "SolsticioNPC.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"

namespace
{
    const FLinearColor Cyan(0.25f, 0.85f, 1.f, 1.f);
    const FLinearColor Gold(1.f, 0.78f, 0.2f, 1.f);
    const FLinearColor White(1.f, 1.f, 1.f, 1.f);
}

void ASolsticioHUD::Text(const FString& S, float X, float Y, FLinearColor C, float Scale, bool bLarge)
{
    UFont* F = bLarge ? GEngine->GetLargeFont() : GEngine->GetMediumFont();
    Canvas->DrawText(F, S, X, Y, Scale, Scale, FFontRenderInfo());
    (void)C;
}

float ASolsticioHUD::Wrapped(const FString& S, float X, float Y, float MaxW, FLinearColor C, float Scale)
{
    UFont* F = GEngine->GetMediumFont();
    TArray<FString> Words;
    S.ParseIntoArray(Words, TEXT(" "));
    FString LineStr;
    float CY = Y, TW, TH = 0;
    Canvas->SetDrawColor(C.ToFColor(true));
    for (const FString& Wd : Words)
    {
        const FString Try = LineStr.IsEmpty() ? Wd : LineStr + TEXT(" ") + Wd;
        Canvas->TextSize(F, Try, TW, TH, Scale, Scale);
        if (TW > MaxW && !LineStr.IsEmpty())
        {
            Canvas->DrawText(F, LineStr, X, CY, Scale, Scale);
            CY += TH + 4; LineStr = Wd;
        }
        else LineStr = Try;
    }
    if (!LineStr.IsEmpty()) { Canvas->DrawText(F, LineStr, X, CY, Scale, Scale); CY += TH + 4; }
    return CY - Y;
}

void ASolsticioHUD::Panel(float X, float Y, float PW, float PH, FLinearColor C)
{
    DrawRect(C, X, Y, PW, PH);
}

void ASolsticioHUD::Line(float X1, float Y1, float X2, float Y2, FLinearColor C, float T)
{
    DrawLine(X1, Y1, X2, Y2, C, T);
}

void ASolsticioHUD::Ring(float CX, float CY, float R, FLinearColor C, float T, int32 Seg)
{
    for (int32 i = 0; i < Seg; ++i)
    {
        const float A = 2 * PI * i / Seg, B = 2 * PI * (i + 1) / Seg;
        DrawLine(CX + FMath::Cos(A) * R, CY + FMath::Sin(A) * R, CX + FMath::Cos(B) * R, CY + FMath::Sin(B) * R, C, T);
    }
}

// Todas as chamadas de texto usam SetDrawColor antes; este wrapper garante a cor.
#define COLORED_TEXT(Str, X, Y, Col, Scale, Large) { Canvas->SetDrawColor((Col).ToFColor(true)); Text(Str, X, Y, Col, Scale, Large); }

void ASolsticioHUD::DrawHUD()
{
    Super::DrawHUD();
    ASolsticioMissionController* PC = Cast<ASolsticioMissionController>(GetOwningPlayerController());
    if (!PC || !Canvas) return;
    W = Canvas->SizeX; H = Canvas->SizeY;

    DrawVisor();
    DrawLabels();

    if (PC->Phase == EPhase::Loading) return;
    if (PC->Phase == EPhase::MissionDone)
    {
        const FMission* M = PC->GetMission();
        DrawCard(TEXT("MISSÃO COMPLETA"), M ? M->Title : FString(), TEXT("Enter — continuar"));
        return;
    }
    if (PC->Phase == EPhase::Ending && PC->EndingData)
    {
        DrawCard(PC->EndingData->Title, PC->EndingData->Text + FString::Printf(TEXT("\n\nLaço entre Jack e José: %d"), PC->Bond), TEXT("Enter — recomeçar"));
        return;
    }

    const FStep* S = PC->GetStep();
    if (!S) return;
    // titulo da missao nos primeiros segundos
    if (const FMission* M = PC->GetMission())
        if (PC->MissionTime < 5.f)
        {
            const float A = FMath::Clamp(5.f - PC->MissionTime, 0.f, 1.f);
            Canvas->SetDrawColor(255, 200, 60, (uint8)(255 * A));
            Text(M->Title, W * 0.5f - 300, H * 0.14f, Gold, 1.6f, true);
            Canvas->SetDrawColor(180, 230, 255, (uint8)(255 * A));
            Text(M->Place, W * 0.5f - 300, H * 0.14f + 50, Cyan, 1.1f);
        }
    if (S->Type == EStep::Line) DrawDialogue(*S);
    else if (S->Type == EStep::Choice) DrawChoice(*S);
    else DrawMinigame(*S);
}

void ASolsticioHUD::DrawVisor()
{
    ASolsticioMissionController* PC = Cast<ASolsticioMissionController>(GetOwningPlayerController());
    ASolsticioArmorCharacter* Ch = PC ? Cast<ASolsticioArmorCharacter>(PC->GetPawn()) : nullptr;
    const float Energy = Ch ? Ch->SolarEnergy / FMath::Max(1.f, Ch->MaxEnergy) : 1.f;
    const float T = GetWorld()->GetTimeSeconds();
    const FLinearColor Edge(0.f, 0.05f, 0.12f, 0.55f), Blue(0.2f, 0.8f, 1.f, 0.9f), Dim(0.2f, 0.8f, 1.f, 0.35f);

    // vinheta do capacete
    for (int32 i = 0; i < 6; ++i)
    {
        const float a = 0.10f * (6 - i) / 6.f, t = 14.f * (i + 1);
        const FLinearColor C(0.f, 0.15f, 0.3f, a);
        DrawRect(C, 0, 0, W, t); DrawRect(C, 0, H - t, W, t);
        DrawRect(C, 0, 0, t, H); DrawRect(C, W - t, 0, t, H);
    }
    DrawRect(Edge, 0, 0, W, 6); DrawRect(Edge, 0, H - 6, W, 6);

    // cantos
    const float M = 36.f, L = 120.f;
    auto Corner = [&](float X, float Y, float SX, float SY)
    {
        Line(X, Y, X + SX * L, Y, Blue, 3); Line(X, Y, X, Y + SY * L, Blue, 3);
        Line(X + SX * 12, Y + SY * 12, X + SX * 60, Y + SY * 12, Dim, 1.5f);
    };
    Corner(M, M, 1, 1); Corner(W - M, M, -1, 1); Corner(M, H - M, 1, -1); Corner(W - M, H - M, -1, -1);

    // mira central
    const float CX = W * 0.5f, CY = H * 0.5f;
    Ring(CX, CY, 22.f, Dim, 1.5f, 32);
    Line(CX - 34, CY, CX - 12, CY, Blue, 2); Line(CX + 12, CY, CX + 34, CY, Blue, 2);
    Line(CX, CY - 34, CX, CY - 12, Blue, 2); Line(CX, CY + 12, CX, CY + 34, Blue, 2);
    Ring(CX, CY, 60.f + FMath::Sin(T * 2) * 2, FLinearColor(0.2f, 0.8f, 1.f, 0.12f), 1.f, 48);

    // titulo e energia
    COLORED_TEXT(TEXT("ARMADURA SOLSTÍCIO  •  MARCA 23"), M + 6, M + 10, Blue, 0.9f, false)
    const float BX = M + 6, BY = M + 38, BW = 260.f;
    DrawRect(FLinearColor(0.f, 0.1f, 0.2f, 0.6f), BX, BY, BW, 14);
    DrawRect(Energy > 0.25f ? FLinearColor(0.2f, 0.9f, 1.f, 0.9f) : FLinearColor(1.f, 0.3f, 0.2f, 0.9f), BX + 2, BY + 2, (BW - 4) * Energy, 10);
    COLORED_TEXT(FString::Printf(TEXT("ENERGIA SOLAR %d%%"), FMath::RoundToInt(Energy * 100)), BX, BY + 20, Blue, 0.8f, false)
    if (Ch && !Ch->bInSunlight) COLORED_TEXT(TEXT("SEM LUZ SOLAR — SEM RECARGA"), BX, BY + 42, FLinearColor(1, 0.5f, 0.2f), 0.8f, false)

    // icones circulares
    for (int32 i = 0; i < 4; ++i) Ring(M + 24 + i * 44, H - M - 70, 16.f, i == 0 ? Gold : Dim, 2.f, 24);

    // controlos
    const TCHAR* Help[] = { TEXT("WASD  mover"), TEXT("RATO  olhar"), TEXT("ESPAÇO  propulsão"), TEXT("E  salto temporal"), TEXT("ENTER  avançar"), TEXT("F9  saltar missão") };
    for (int32 i = 0; i < 6; ++i) COLORED_TEXT(Help[i], W - M - 230, H - M - 150 + i * 22, Dim, 0.75f, false)
}

void ASolsticioHUD::DrawLabels()
{
    for (TActorIterator<ASolsticioNPC> It(GetWorld()); It; ++It)
    {
        const FVector P = It->GetActorLocation() + FVector(0, 0, 130);
        const FVector2D Cp = FVector2D(Canvas->Project(P));
        FVector Dir = P - PlayerOwner->PlayerCameraManager->GetCameraLocation();
        if (FVector::DotProduct(Dir, PlayerOwner->PlayerCameraManager->GetActorForwardVector()) <= 0.f || Dir.Size() > 2500.f) continue;
        const FLinearColor C = It->bHologram ? Cyan : Gold;
        DrawRect(FLinearColor(0, 0.05f, 0.1f, 0.6f), Cp.X - 60, Cp.Y - 14, 120, 24);
        COLORED_TEXT(It->DisplayName, Cp.X - 50, Cp.Y - 11, C, 0.8f, false)
    }
}

void ASolsticioHUD::DrawDialogue(const FStep& S)
{
    const float BW = FMath::Min(W - 200, 1100.f), BH = 170.f, X = (W - BW) * 0.5f, Y = H - BH - 50;
    DrawRect(FLinearColor(0.f, 0.05f, 0.1f, 0.78f), X, Y, BW, BH);
    Line(X, Y, X + BW, Y, Cyan, 2); Line(X, Y + BH, X + BW, Y + BH, Cyan, 1);
    const bool Narr = S.Who == TEXT("Narrador");
    const FLinearColor C = Narr ? FLinearColor(0.8f, 0.85f, 0.9f) : (S.Who == TEXT("Íris") ? Cyan : Gold);
    COLORED_TEXT(Narr ? FString(TEXT("— Narração —")) : S.Who.ToUpper(), X + 24, Y + 14, C, 1.1f, false)
    Wrapped(S.Text, X + 24, Y + 48, BW - 48, White, 1.0f);
    COLORED_TEXT(TEXT("Enter ▸"), X + BW - 110, Y + BH - 28, Cyan, 0.8f, false)
}

void ASolsticioHUD::DrawChoice(const FStep& S)
{
    const float BW = FMath::Min(W - 200, 1100.f), X = (W - BW) * 0.5f;
    float Y = H * 0.25f;
    DrawRect(FLinearColor(0.f, 0.05f, 0.1f, 0.82f), X, Y - 20, BW, 120 + S.Options.Num() * 70);
    Y += Wrapped(S.Text, X + 24, Y, BW - 48, Gold, 1.1f) + 20;
    for (int32 i = 0; i < S.Options.Num(); ++i)
    {
        DrawRect(FLinearColor(0.05f, 0.2f, 0.3f, 0.7f), X + 24, Y, BW - 48, 52);
        Canvas->SetDrawColor(255, 255, 255);
        Wrapped(FString::Printf(TEXT("[%d]  %s"), i + 1, *S.Options[i].Text), X + 38, Y + 12, BW - 80, White, 0.95f);
        Y += 62;
    }
}

void ASolsticioHUD::DrawMinigame(const FStep& S)
{
    ASolsticioMissionController* PC = Cast<ASolsticioMissionController>(GetOwningPlayerController());
    const float BW = FMath::Min(W - 240, 900.f), X = (W - BW) * 0.5f;
    DrawRect(FLinearColor(0.f, 0.05f, 0.1f, 0.7f), X - 20, 60, BW + 40, 40 + 70);
    Wrapped(S.Text, X, 76, BW, Gold, 1.05f);

    switch (S.Type)
    {
    case EStep::Timing:
    {
        const float Y = H * 0.5f - 20;
        DrawRect(FLinearColor(0.f, 0.1f, 0.2f, 0.85f), X, Y, BW, 40);
        DrawRect(FLinearColor(1.f, 0.78f, 0.2f, 0.55f), X + PC->TimingStart * BW, Y, S.Width * BW, 40);
        DrawRect(FLinearColor(1.f, 1.f, 1.f, 1.f), X + PC->TimingPos * BW - 3, Y - 10, 6, 60);
        COLORED_TEXT(TEXT("ESPAÇO — carregar na zona dourada"), X, Y + 60, Cyan, 0.9f, false)
        break;
    }
    case EStep::Slider:
    {
        const float Y = H * 0.5f - 20;
        DrawRect(FLinearColor(0.f, 0.1f, 0.2f, 0.85f), X, Y, BW, 30);
        DrawRect(FLinearColor(1.f, 0.78f, 0.2f, 0.45f), X + (S.Target - S.Tol) * BW, Y, S.Tol * 2 * BW, 30);
        DrawRect(FLinearColor(1.f, 1.f, 1.f, 1.f), X + PC->SliderVal * BW - 4, Y - 10, 8, 50);
        COLORED_TEXT(TEXT("A / D ou setas — ajustar    Enter — confirmar"), X, Y + 55, Cyan, 0.9f, false)
        break;
    }
    case EStep::Targets:
    {
        for (const FVector2D& P : PC->TargetPos)
        {
            Ring(P.X * W, P.Y * H, 38.f, Gold, 3.f, 32);
            Ring(P.X * W, P.Y * H, 20.f, Cyan, 2.f, 24);
            DrawRect(Cyan, P.X * W - 3, P.Y * H - 3, 6, 6);
        }
        COLORED_TEXT(FString::Printf(TEXT("TEMPO: %.1fs   ALVOS: %d"), PC->TargetTimeLeft, PC->TargetPos.Num()), W * 0.5f - 120, H - 140, Cyan, 1.f, false)
        break;
    }
    case EStep::Sequence:
    {
        float Y = H * 0.32f;
        for (int32 d = 0; d < PC->DisplayOrder.Num(); ++d)
        {
            const int32 Item = PC->DisplayOrder[d];
            const bool Done = PC->Picked.Contains(Item);
            DrawRect(Done ? FLinearColor(0.1f, 0.5f, 0.25f, 0.8f) : FLinearColor(0.05f, 0.2f, 0.3f, 0.75f), X, Y, BW, 44);
            Canvas->SetDrawColor(255, 255, 255);
            Text(FString::Printf(TEXT("[%d]  %s%s"), d + 1, *S.Items[Item], Done ? TEXT("   ✔") : TEXT("")), X + 16, Y + 10, White, 0.95f);
            Y += 52;
        }
        break;
    }
    default: break;
    }
    if (!PC->Message.IsEmpty()) COLORED_TEXT(PC->Message, X, H - 200, FLinearColor(1.f, 0.45f, 0.35f), 1.f, false)
}

void ASolsticioHUD::DrawCard(const FString& Title, const FString& Body, const FString& Hint)
{
    DrawRect(FLinearColor(0.f, 0.03f, 0.08f, 0.88f), 0, 0, W, H);
    const float BW = FMath::Min(W - 200, 1000.f), X = (W - BW) * 0.5f;
    COLORED_TEXT(Title, X, H * 0.22f, Gold, 1.8f, true)
    Wrapped(Body, X, H * 0.22f + 90, BW, White, 1.1f);
    COLORED_TEXT(Hint, X, H * 0.8f, Cyan, 1.f, false)
}
