#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SolsticioHUD.generated.h"

UCLASS()
class SOLSTICIO_API ASolsticioHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void DrawHUD() override;

private:
    float DrawWrapped(const FString& Text, float X, float Y, float W, float Scale, const FLinearColor& Color);
};
