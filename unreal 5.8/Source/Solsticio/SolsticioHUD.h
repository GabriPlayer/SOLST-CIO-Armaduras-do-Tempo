#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SolsticioHUD.generated.h"

// Visor da armadura (estilo capacete) + dialogos + interface dos mini-jogos.
UCLASS()
class SOLSTICIO_API ASolsticioHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void DrawHUD() override;

private:
    float W = 0.f, H = 0.f;

    void Text(const FString& S, float X, float Y, FLinearColor C, float Scale = 1.f, bool bLarge = false);
    float Wrapped(const FString& S, float X, float Y, float MaxW, FLinearColor C, float Scale = 1.f);
    void Panel(float X, float Y, float PW, float PH, FLinearColor C);
    void Line(float X1, float Y1, float X2, float Y2, FLinearColor C, float T = 2.f);
    void Ring(float CX, float CY, float R, FLinearColor C, float T = 2.f, int32 Seg = 40);

    void DrawVisor();
    void DrawLabels();
    void DrawDialogue(const struct FStep& S);
    void DrawMinigame(const struct FStep& S);
    void DrawChoice(const struct FStep& S);
    void DrawCard(const FString& Title, const FString& Body, const FString& Hint);
};
