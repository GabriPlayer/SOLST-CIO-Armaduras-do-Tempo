#include "SolsticioNPC.h"
#include "Kismet/GameplayStatics.h"

ASolsticioNPC::ASolsticioNPC()
{
    PrimaryActorTick.bCanEverTick = true;
    Body = CreateDefaultSubobject<USceneComponent>(TEXT("Body"));
    RootComponent = Body;
}

void ASolsticioNPC::Setup(const FString& Name, const FHumanoidStyle& Style, bool bInHologram)
{
    DisplayName = Name;
    bHologram = bInHologram;
    if (bHologram) SolsticioModels::BuildHologramOrb(this, Body, Style.Glow);
    else SolsticioModels::BuildHumanoid(this, Body, 0.f, Style);
}

void ASolsticioNPC::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    Time += DeltaSeconds;
    if (APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0))
    {
        FRotator R = (Player->GetActorLocation() - GetActorLocation()).Rotation();
        R.Pitch = 0; R.Roll = 0;
        SetActorRotation(FMath::RInterpTo(GetActorRotation(), R, DeltaSeconds, 4.f));
    }
    if (bHologram) Body->SetRelativeLocation(FVector(0, 0, 10.f * FMath::Sin(Time * 2.f)));
}
