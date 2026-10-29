#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SolsticioGameMode.generated.h"

// Constroi em runtime o cenario de cada missao e as personagens que nela falam.
UCLASS()
class SOLSTICIO_API ASolsticioGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ASolsticioGameMode();

    virtual void BeginPlay() override;
    void BuildMission(int32 Index, const TSet<FString>& Speakers);

private:
    UPROPERTY() TArray<AActor*> Scenery;

    AActor* Shape(const TCHAR* Mesh, FVector Loc, FVector SizeCm, FLinearColor Color, FRotator Rot = FRotator::ZeroRotator, float SpinDegPerSec = 0.f);
    void Box(FVector Loc, FVector Size, FLinearColor C, FRotator R = FRotator::ZeroRotator) { Shape(TEXT("Cube"), Loc, Size, C, R); }
    void Cyl(FVector Loc, FVector Size, FLinearColor C, FRotator R = FRotator::ZeroRotator, float Spin = 0.f) { Shape(TEXT("Cylinder"), Loc, Size, C, R, Spin); }
    void Sph(FVector Loc, FVector Size, FLinearColor C) { Shape(TEXT("Sphere"), Loc, Size, C); }
    void Light(FVector Loc, FLinearColor C, float Intensity, float Radius = 1800.f);
    void Room(FVector Size, FLinearColor Wall, FLinearColor Floor, FLinearColor Ceil);
    void Env(FLinearColor SunColor, float SunIntensity, FLinearColor Fog, float FogDensity, float Exposure);
    void Stars(int32 Count, float Radius, FLinearColor C);
    void SpawnNPCs(const TSet<FString>& Speakers);
    void Scene(int32 Index);
};
