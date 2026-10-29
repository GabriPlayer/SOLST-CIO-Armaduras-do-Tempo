#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SolsticioArmorCharacter.generated.h"

class UCameraComponent;

// Armadura Solsticio: a energia solar alimenta propulsores e teletransporte temporal.
UCLASS()
class SOLSTICIO_API ASolsticioArmorCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ASolsticioArmorCharacter();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(UInputComponent* Input) override;

    UPROPERTY(BlueprintReadOnly) float SolarEnergy = 100.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxEnergy = 100.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float RechargePerSecond = 8.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float ThrustCostPerSecond = 15.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float TimeJumpCost = 60.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float TimeJumpDistance = 1500.f;

    // Fica verdadeiro quando a armadura esta exposta a luz solar (ex.: fora do eclipse/cidade subterranea).
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bInSunlight = true;

    UFUNCTION(BlueprintCallable) bool TimeJump();

private:
    UPROPERTY(VisibleAnywhere) UCameraComponent* Camera;
    bool bThrusting = false;
    float BobTime = 0.f;
    UPROPERTY() USceneComponent* ArmRig = nullptr;
    bool Locked() const;

    void MoveForward(float V);
    void MoveRight(float V);
    void Turn(float V);
    void LookUp(float V);
    void ThrustOn();
    void ThrustOff();
    void OnTimeJump();
};


