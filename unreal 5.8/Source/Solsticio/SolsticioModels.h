#pragma once

#include "CoreMinimal.h"

class AActor;
class USceneComponent;
class UStaticMeshComponent;

struct FHumanoidStyle
{
    FLinearColor Suit = FLinearColor(0.1f, 0.12f, 0.2f);
    FLinearColor Plate = FLinearColor(0.8f, 0.6f, 0.1f);
    FLinearColor Skin = FLinearColor(0.85f, 0.65f, 0.5f);
    FLinearColor Hair = FLinearColor(0.08f, 0.05f, 0.03f);
    FLinearColor Glow = FLinearColor(1.f, 0.8f, 0.2f);
    bool bArmor = false;
    bool bLabCoat = false;
};

// Constroi modelos a partir de formas basicas do motor (sem assets externos).
namespace SolsticioModels
{
    UStaticMeshComponent* AddPart(AActor* Owner, USceneComponent* Parent, const TCHAR* Shape,
        const FVector& Loc, const FVector& SizeCm, const FRotator& Rot, const FLinearColor& Color);

    // FeetZ: altura dos pes relativa a Parent (ex.: -88 numa capsula de Character).
    void BuildHumanoid(AActor* Owner, USceneComponent* Parent, float FeetZ, const FHumanoidStyle& Style);

    void BuildHologramOrb(AActor* Owner, USceneComponent* Parent, const FLinearColor& Color);
}
