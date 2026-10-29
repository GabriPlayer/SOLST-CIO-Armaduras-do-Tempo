#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SolsticioModels.h"
#include "SolsticioNPC.generated.h"

// Personagem nao jogavel: olha sempre para o jogador.
UCLASS()
class SOLSTICIO_API ASolsticioNPC : public AActor
{
    GENERATED_BODY()

public:
    ASolsticioNPC();

    FString DisplayName;
    bool bHologram = false;

    void Setup(const FString& Name, const FHumanoidStyle& Style, bool bInHologram = false);
    virtual void Tick(float DeltaSeconds) override;

private:
    UPROPERTY() USceneComponent* Body;
    float Time = 0.f;
};
