#include "SolsticioArmorCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"

ASolsticioArmorCharacter::ASolsticioArmorCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    Boom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Boom"));
    Boom->SetupAttachment(RootComponent);
    Boom->TargetArmLength = 400.f;
    Boom->bUsePawnControlRotation = true;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(Boom);

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->AirControl = 0.8f;
}

void ASolsticioArmorCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (bThrusting && SolarEnergy > 0.f)
    {
        SolarEnergy = FMath::Max(0.f, SolarEnergy - ThrustCostPerSecond * DeltaSeconds);
        GetCharacterMovement()->AddForce(FVector(0, 0, 150000.f));
        GetCharacterMovement()->SetMovementMode(MOVE_Falling);
    }
    else if (bInSunlight)
    {
        SolarEnergy = FMath::Min(MaxEnergy, SolarEnergy + RechargePerSecond * DeltaSeconds);
    }
}

bool ASolsticioArmorCharacter::TimeJump()
{
    if (SolarEnergy < TimeJumpCost) return false;
    SolarEnergy -= TimeJumpCost;
    // Prototipo: o salto temporal desloca a armadura para a frente; cada missao pode redefinir o destino.
    TeleportTo(GetActorLocation() + GetActorForwardVector() * TimeJumpDistance, GetActorRotation());
    return true;
}

void ASolsticioArmorCharacter::SetupPlayerInputComponent(UInputComponent* Input)
{
    Super::SetupPlayerInputComponent(Input);
    Input->BindAxis("MoveForward", this, &ASolsticioArmorCharacter::MoveForward);
    Input->BindAxis("MoveRight", this, &ASolsticioArmorCharacter::MoveRight);
    Input->BindAxis("Turn", this, &ASolsticioArmorCharacter::Turn);
    Input->BindAxis("LookUp", this, &ASolsticioArmorCharacter::LookUp);
    Input->BindAction("Thrust", IE_Pressed, this, &ASolsticioArmorCharacter::ThrustOn);
    Input->BindAction("Thrust", IE_Released, this, &ASolsticioArmorCharacter::ThrustOff);
    Input->BindAction("TimeJump", IE_Pressed, this, &ASolsticioArmorCharacter::OnTimeJump);
}

void ASolsticioArmorCharacter::MoveForward(float V) { AddMovementInput(GetActorForwardVector(), V); }
void ASolsticioArmorCharacter::MoveRight(float V) { AddMovementInput(GetActorRightVector(), V); }
void ASolsticioArmorCharacter::Turn(float V) { AddControllerYawInput(V); }
void ASolsticioArmorCharacter::LookUp(float V) { AddControllerPitchInput(V); }
void ASolsticioArmorCharacter::ThrustOn() { bThrusting = true; }
void ASolsticioArmorCharacter::ThrustOff() { bThrusting = false; }
void ASolsticioArmorCharacter::OnTimeJump() { TimeJump(); }

