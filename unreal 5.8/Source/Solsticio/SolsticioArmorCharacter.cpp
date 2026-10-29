#include "SolsticioArmorCharacter.h"
#include "SolsticioMissionController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "SolsticioModels.h"

ASolsticioArmorCharacter::ASolsticioArmorCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(RootComponent);
    Camera->SetRelativeLocation(FVector(12.f, 0.f, 62.f));
    Camera->bUsePawnControlRotation = true;
    Camera->SetFieldOfView(95.f);

    bUseControllerRotationYaw = true;
    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->AirControl = 0.8f;
    GetCharacterMovement()->MaxWalkSpeed = 520.f;
}

// Bracos da armadura vistos de dentro do capacete (primeira pessoa).
void ASolsticioArmorCharacter::BeginPlay()
{
    Super::BeginPlay();
    const FRotator Z = FRotator::ZeroRotator;
    const FLinearColor Red(0.55f, 0.02f, 0.03f), Gold(0.95f, 0.6f, 0.08f), Dark(0.03f, 0.04f, 0.06f), Cyan(0.2f, 0.9f, 1.f);

    ArmRig = NewObject<USceneComponent>(this);
    ArmRig->RegisterComponent();
    ArmRig->AttachToComponent(Camera, FAttachmentTransformRules::KeepRelativeTransform);

    for (float Side : { -1.f, 1.f })
    {
        const float Y = Side * 26.f;
        auto Add = [&](const TCHAR* Shape, FVector L, FVector S, FRotator R, FLinearColor C)
        {
            UStaticMeshComponent* M = SolsticioModels::AddPart(this, ArmRig, Shape, L, S, R, C);
            M->SetCastShadow(false);
        };
        Add(TEXT("Cylinder"), FVector(26, Y * 1.15f, -26), FVector(11, 11, 32), FRotator(-72, 0, Side * 6), Red);
        Add(TEXT("Cylinder"), FVector(34, Y, -23), FVector(13, 13, 18), FRotator(-72, 0, Side * 6), Gold);
        Add(TEXT("Cube"), FVector(46, Y * 0.95f, -20), FVector(10, 10, 4), FRotator(-8, 0, 0), Gold);
        Add(TEXT("Cube"), FVector(53, Y * 0.95f, -20), FVector(8, 9, 3), FRotator(-8, 0, 0), Red);
        for (int32 F = 0; F < 4; ++F)
            Add(TEXT("Cube"), FVector(60, Y * 0.95f + (F - 1.5f) * 2.4f, -20.5f), FVector(7, 2, 2), FRotator(-8, 0, 0), Gold);
        Add(TEXT("Cube"), FVector(52, Y * 0.95f + Side * 6.f, -21), FVector(7, 2.5f, 2.5f), FRotator(-8, Side * 25.f, 0), Gold);
        Add(TEXT("Sphere"), FVector(49, Y * 0.95f, -19.2f), FVector(5, 5, 1.5f), Z, Cyan * 5.f);
        Add(TEXT("Cube"), FVector(30, Y * 1.4f, -30), FVector(14, 4, 3), Z, Dark);
    }
    UPointLightComponent* L = NewObject<UPointLightComponent>(this);
    L->SetMobility(EComponentMobility::Movable);
    L->SetLightColor(FColor(80, 220, 255));
    L->SetIntensity(120.f);
    L->SetAttenuationRadius(120.f);
    L->SetCastShadows(false);
    L->RegisterComponent();
    L->AttachToComponent(ArmRig, FAttachmentTransformRules::KeepRelativeTransform);
    L->SetRelativeLocation(FVector(55, 0, -15));
}

bool ASolsticioArmorCharacter::Locked() const
{
    const ASolsticioMissionController* PC = Cast<ASolsticioMissionController>(GetController());
    return PC && (PC->InMinigame() || (PC->Phase != EPhase::Step && PC->Phase != EPhase::Loading));
}

void ASolsticioArmorCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    const float Speed = GetVelocity().Size2D();
    BobTime += DeltaSeconds * (3.f + Speed / 90.f);
    if (ArmRig)
    {
        const float Amp = FMath::Min(Speed / 520.f, 1.f);
        ArmRig->SetRelativeLocation(FVector(0.f, FMath::Sin(BobTime * 0.5f) * 1.5f * Amp,
            FMath::Sin(BobTime) * 1.2f * Amp + FMath::Sin(GetWorld()->GetTimeSeconds() * 1.3f) * 0.3f));
    }

    if (bThrusting && SolarEnergy > 0.f && !Locked())
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

void ASolsticioArmorCharacter::MoveForward(float V) { if (!Locked()) AddMovementInput(GetActorForwardVector(), V); }
void ASolsticioArmorCharacter::MoveRight(float V) { if (!Locked()) AddMovementInput(GetActorRightVector(), V); }
void ASolsticioArmorCharacter::Turn(float V) { AddControllerYawInput(V); }
void ASolsticioArmorCharacter::LookUp(float V) { AddControllerPitchInput(V); }
void ASolsticioArmorCharacter::ThrustOn() { bThrusting = true; }
void ASolsticioArmorCharacter::ThrustOff() { bThrusting = false; }
void ASolsticioArmorCharacter::OnTimeJump() { if (!Locked()) TimeJump(); }
