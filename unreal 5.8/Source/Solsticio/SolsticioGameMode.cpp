#include "SolsticioGameMode.h"
#include "SolsticioArmorCharacter.h"
#include "SolsticioMissionController.h"
#include "SolsticioHUD.h"
#include "SolsticioNPC.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Engine/DirectionalLight.h"
#include "Engine/PointLight.h"
#include "Engine/SkyLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/PostProcessVolume.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Kismet/GameplayStatics.h"

namespace
{
    typedef FLinearColor C;
    FLinearColor Glow(float R, float G, float B, float K = 4.f) { return FLinearColor(R * K, G * K, B * K); }
}

ASolsticioGameMode::ASolsticioGameMode()
{
    DefaultPawnClass = ASolsticioArmorCharacter::StaticClass();
    PlayerControllerClass = ASolsticioMissionController::StaticClass();
    HUDClass = ASolsticioHUD::StaticClass();
}

void ASolsticioGameMode::BeginPlay()
{
    Super::BeginPlay();
    if (ASolsticioMissionController* PC = Cast<ASolsticioMissionController>(UGameplayStatics::GetPlayerController(this, 0)))
        PC->StartCampaign();
}

AActor* ASolsticioGameMode::Shape(const TCHAR* Mesh, FVector Loc, FVector SizeCm, FLinearColor Color, FRotator Rot, float Spin)
{
    UStaticMesh* SM = LoadObject<UStaticMesh>(nullptr, *FString::Printf(TEXT("/Engine/BasicShapes/%s.%s"), Mesh, Mesh));
    UMaterialInterface* Mat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    FActorSpawnParameters P; P.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    AStaticMeshActor* A = GetWorld()->SpawnActor<AStaticMeshActor>(Loc, Rot, P);
    if (!A || !SM) return A;
    A->SetMobility(EComponentMobility::Movable);
    UStaticMeshComponent* M = A->GetStaticMeshComponent();
    M->SetStaticMesh(SM);
    A->SetActorScale3D(SizeCm / 100.f);
    if (Mat)
    {
        UMaterialInstanceDynamic* D = UMaterialInstanceDynamic::Create(Mat, A);
        D->SetVectorParameterValue(TEXT("Color"), Color);
        M->SetMaterial(0, D);
    }
    if (Spin != 0.f)
    {
        URotatingMovementComponent* R = NewObject<URotatingMovementComponent>(A);
        R->RotationRate = FRotator(0, Spin, 0);
        R->RegisterComponent();
    }
    Scenery.Add(A);
    return A;
}

void ASolsticioGameMode::Light(FVector Loc, FLinearColor Col, float Intensity, float Radius)
{
    APointLight* L = GetWorld()->SpawnActor<APointLight>(Loc, FRotator::ZeroRotator);
    if (!L) return;
    L->SetMobility(EComponentMobility::Movable);
    UPointLightComponent* LC = L->PointLightComponent;
    LC->SetLightColor(Col.ToFColor(true));
    LC->SetIntensity(Intensity);
    LC->SetAttenuationRadius(Radius);
    LC->SetCastShadows(false);
    Scenery.Add(L);
}

void ASolsticioGameMode::Room(FVector S, FLinearColor Wall, FLinearColor Floor, FLinearColor Ceil)
{
    const float X = S.X / 2, Y = S.Y / 2, H = S.Z;
    Box(FVector(0, 0, -10), FVector(S.X, S.Y, 20), Floor);
    Box(FVector(0, 0, H + 10), FVector(S.X, S.Y, 20), Ceil);
    Box(FVector(X + 10, 0, H / 2), FVector(20, S.Y, H), Wall);
    Box(FVector(-X - 10, 0, H / 2), FVector(20, S.Y, H), Wall);
    Box(FVector(0, Y + 10, H / 2), FVector(S.X, 20, H), Wall);
    Box(FVector(0, -Y - 10, H / 2), FVector(S.X, 20, H), Wall);
    // rodape e friso luminoso
    Box(FVector(X - 1, 0, 8), FVector(4, S.Y, 16), Wall * 0.4f);
    Box(FVector(0, Y - 1, 8), FVector(S.X, 4, 16), Wall * 0.4f);
    Box(FVector(0, -Y + 1, 8), FVector(S.X, 4, 16), Wall * 0.4f);
}

void ASolsticioGameMode::Env(FLinearColor SunColor, float SunIntensity, FLinearColor Fog, float FogDensity, float Exposure)
{
    ADirectionalLight* Sun = GetWorld()->SpawnActor<ADirectionalLight>(FVector(0, 0, 800), FRotator(-45, 30, 0));
    if (Sun)
    {
        Sun->SetMobility(EComponentMobility::Movable);
        Sun->GetLightComponent()->SetLightColor(SunColor.ToFColor(true));
        Sun->GetLightComponent()->SetIntensity(SunIntensity);
        Scenery.Add(Sun);
    }
    ASkyLight* Sky = GetWorld()->SpawnActor<ASkyLight>(FVector(0, 0, 500), FRotator::ZeroRotator);
    if (Sky)
    {
        Sky->GetLightComponent()->SetMobility(EComponentMobility::Movable);
        Sky->GetLightComponent()->SetLightColor(SunColor.ToFColor(true));
        Sky->GetLightComponent()->SetIntensity(0.6f);
        Sky->GetLightComponent()->RecaptureSky();
        Scenery.Add(Sky);
    }
    AExponentialHeightFog* F = GetWorld()->SpawnActor<AExponentialHeightFog>(FVector(0, 0, 100), FRotator::ZeroRotator);
    if (F)
    {
        F->GetComponent()->SetFogDensity(FogDensity);
        F->GetComponent()->SetFogInscatteringColor(Fog);
        Scenery.Add(F);
    }
    APostProcessVolume* PP = GetWorld()->SpawnActor<APostProcessVolume>(FVector::ZeroVector, FRotator::ZeroRotator);
    if (PP)
    {
        PP->bUnbound = true;
        PP->Settings.bOverride_BloomIntensity = true; PP->Settings.BloomIntensity = 1.4f;
        PP->Settings.bOverride_AutoExposureBias = true; PP->Settings.AutoExposureBias = Exposure;
        PP->Settings.bOverride_VignetteIntensity = true; PP->Settings.VignetteIntensity = 0.5f;
        PP->Settings.bOverride_SceneFringeIntensity = true; PP->Settings.SceneFringeIntensity = 0.6f;
        Scenery.Add(PP);
    }
}

void ASolsticioGameMode::Stars(int32 Count, float Radius, FLinearColor Col)
{
    FRandomStream R(7);
    for (int32 i = 0; i < Count; ++i)
    {
        const FVector Dir = R.VRand();
        const float S = R.FRandRange(4.f, 14.f);
        if (AActor* A = Shape(TEXT("Sphere"), Dir * Radius + FVector(0, 0, 300), FVector(S), Col * R.FRandRange(2.f, 8.f)))
            A->SetActorEnableCollision(false);
    }
}

void ASolsticioGameMode::SpawnNPCs(const TSet<FString>& Speakers)
{
    TArray<FString> Names;
    for (const FString& N : Speakers)
        if (N != TEXT("Tim") && N != TEXT("Narrador") && !N.StartsWith(TEXT("Tim "))) Names.Add(N);
    Names.Sort();
    const int32 N = Names.Num();
    for (int32 i = 0; i < N; ++i)
    {
        const float A = FMath::DegreesToRadians((i - (N - 1) * 0.5f) * 32.f);
        const FVector Loc = FVector(-600, 0, 90) + FVector(FMath::Cos(A), FMath::Sin(A), 0) * 520.f;
        FHumanoidStyle St;
        bool bHolo = false;
        const FString& Nm = Names[i];
        if (Nm == TEXT("Íris")) { bHolo = true; St.Suit = C(0.1f, 0.6f, 1.f); St.Glow = C(0.3f, 0.9f, 1.f); }
        else if (Nm == TEXT("Jack")) { St.Suit = C(0.08f, 0.1f, 0.22f); St.Plate = C(0.9f, 0.55f, 0.08f); St.bArmor = true; St.Hair = C(0.3f, 0.2f, 0.08f); }
        else if (Nm == TEXT("José")) { St.Suit = C(0.15f, 0.15f, 0.2f); St.bLabCoat = true; St.Skin = C(0.6f, 0.42f, 0.3f); }
        else if (Nm.StartsWith(TEXT("Dr."))) { St.Suit = C(0.35f, 0.35f, 0.38f); St.bLabCoat = true; St.Hair = C(0.7f, 0.7f, 0.72f); St.Skin = C(0.8f, 0.62f, 0.5f); }
        else if (Nm == TEXT("Martim")) { St.Suit = C(0.1f, 0.25f, 0.35f); St.Plate = C(0.7f, 0.7f, 0.75f); St.bLabCoat = true; }
        ASolsticioNPC* NPC = GetWorld()->SpawnActor<ASolsticioNPC>(Loc, FRotator::ZeroRotator);
        if (NPC) { NPC->Setup(Nm, St, bHolo); Scenery.Add(NPC); }
    }
}

void ASolsticioGameMode::BuildMission(int32 Index, const TSet<FString>& Speakers)
{
    for (AActor* A : Scenery) if (IsValid(A)) A->Destroy();
    Scenery.Reset();
    Scene(Index);
    SpawnNPCs(Speakers);

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (PC && PC->GetPawn())
    {
        PC->GetPawn()->SetActorLocation(FVector(-600, 0, 100), false, nullptr, ETeleportType::TeleportPhysics);
        PC->SetControlRotation(FRotator::ZeroRotator);
        if (ASolsticioArmorCharacter* Ch = Cast<ASolsticioArmorCharacter>(PC->GetPawn()))
            Ch->bInSunlight = (Index != 5 && Index != 6);
    }
}

void ASolsticioGameMode::Scene(int32 I)
{
    const FVector Big(2400, 1800, 600);
    switch (I)
    {
    case 0: // Laboratorio NASA
        Env(C(1, 0.97f, 0.9f), 3.f, C(0.6f, 0.7f, 0.9f), 0.002f, 0.3f);
        Room(Big, C(0.82f, 0.85f, 0.9f), C(0.3f, 0.32f, 0.36f), C(0.9f, 0.9f, 0.92f));
        for (int32 k = -2; k <= 2; ++k) { Box(FVector(k * 400, 0, 595), FVector(260, 40, 8), Glow(1, 1, 1, 3.f)); Light(FVector(k * 400, 0, 500), C(1, 0.97f, 0.92f), 6000.f); }
        for (int32 k = 0; k < 5; ++k) { Box(FVector(-300 + k * 150, 840, 60), FVector(110, 70, 120), C(0.12f, 0.14f, 0.18f)); Box(FVector(-300 + k * 150, 805, 130), FVector(100, 6, 50), Glow(0.2f, 0.7f, 1.f, 2.5f), FRotator(-20, 0, 0)); }
        Box(FVector(900, -600, 50), FVector(600, 120, 100), C(0.65f, 0.67f, 0.7f));
        Cyl(FVector(0, 0, 8), FVector(260, 260, 16), C(0.15f, 0.17f, 0.22f));
        Cyl(FVector(0, 0, 18), FVector(230, 230, 4), Glow(0.2f, 0.8f, 1.f, 2.f));
        Cyl(FVector(950, 450, 100), FVector(70, 70, 200), C(0.2f, 0.2f, 0.25f));
        Sph(FVector(950, 450, 230), FVector(60, 60, 60), C(0.9f, 0.55f, 0.08f));
        Box(FVector(-1150, 0, 300), FVector(8, 700, 260), Glow(0.15f, 0.5f, 1.f, 1.2f));
        Light(FVector(0, 0, 120), C(0.3f, 0.8f, 1.f), 5000.f, 700.f);
        break;
    case 1: // Porao de carga Solarix-1
        Env(C(0.6f, 0.65f, 0.8f), 0.6f, C(0.4f, 0.1f, 0.1f), 0.006f, 0.6f);
        Room(FVector(2600, 1400, 450), C(0.25f, 0.27f, 0.3f), C(0.14f, 0.15f, 0.17f), C(0.12f, 0.12f, 0.14f));
        for (int32 k = -3; k <= 3; ++k) { Light(FVector(k * 350, 0, 420), C(1, 0.15f, 0.1f), 4500.f, 900.f); Box(FVector(k * 350, 0, 440), FVector(120, 30, 6), Glow(1, 0.1f, 0.05f)); }
        for (int32 k = 0; k < 12; ++k) { const float x = -1100 + (k % 6) * 380, y = (k < 6 ? 1 : -1) * 560; Box(FVector(x, y, 90), FVector(220, 220, 180), C(0.35f, 0.3f, 0.2f)); Box(FVector(x, y, 181), FVector(222, 222, 4), C(0.9f, 0.75f, 0.05f)); }
        Box(FVector(1290, 0, 220), FVector(8, 500, 260), C(0.02f, 0.03f, 0.06f));
        Stars(60, 3500, C(1, 1, 1));
        break;
    case 2: // Estacao orbital
        Env(C(1, 0.9f, 0.7f), 6.f, C(0.2f, 0.3f, 0.6f), 0.0005f, 0.2f);
        Room(FVector(2400, 1600, 500), C(0.7f, 0.72f, 0.78f), C(0.22f, 0.24f, 0.3f), C(0.6f, 0.62f, 0.68f));
        Box(FVector(1190, 0, 260), FVector(10, 1200, 320), C(0.01f, 0.02f, 0.05f));
        Sph(FVector(2600, -300, 120), FVector(1800, 1800, 1800), C(0.1f, 0.35f, 0.9f));
        Sph(FVector(3200, 900, 700), FVector(260, 260, 260), Glow(1, 0.85f, 0.4f, 12.f));
        Light(FVector(1000, 900, 400), C(1, 0.85f, 0.5f), 9000.f);
        Stars(70, 4500, C(1, 1, 1));
        for (int32 k = -2; k <= 2; ++k) Box(FVector(-1190, k * 300, 250), FVector(8, 60, 200), Glow(0.2f, 0.8f, 1.f, 2.5f));
        break;
    case 3: // Hangar 1986
        Env(C(1, 0.85f, 0.6f), 1.2f, C(0.6f, 0.5f, 0.3f), 0.003f, 0.5f);
        Room(FVector(3000, 2200, 700), C(0.5f, 0.45f, 0.35f), C(0.2f, 0.19f, 0.17f), C(0.3f, 0.28f, 0.25f));
        Cyl(FVector(900, 0, 130), FVector(160, 160, 520), C(0.8f, 0.8f, 0.82f), FRotator(0, 0, 90));
        Shape(TEXT("Cone"), FVector(1230, 0, 130), FVector(150, 150, 140), C(0.85f, 0.2f, 0.1f), FRotator(0, 0, -90));
        Box(FVector(760, 280, 60), FVector(200, 300, 12), C(0.7f, 0.7f, 0.72f), FRotator(0, 20, 0));
        Box(FVector(760, -280, 60), FVector(200, 300, 12), C(0.7f, 0.7f, 0.72f), FRotator(0, -20, 0));
        for (int32 k = 0; k < 4; ++k) { Box(FVector(-200 + k * 160, -1050, 90), FVector(110, 80, 90), C(0.7f, 0.65f, 0.55f)); Box(FVector(-200 + k * 160, -1008, 100), FVector(80, 4, 60), Glow(0.2f, 1.f, 0.3f, 2.5f)); }
        for (int32 k = -2; k <= 2; ++k) Light(FVector(k * 550, 0, 600), C(1, 0.9f, 0.7f), 9000.f, 1400.f);
        break;
    case 4: // Observatorio do seculo XVI
        Env(C(0.5f, 0.55f, 0.8f), 0.8f, C(0.2f, 0.2f, 0.35f), 0.004f, 0.8f);
        Room(FVector(2000, 2000, 650), C(0.38f, 0.35f, 0.32f), C(0.25f, 0.22f, 0.19f), C(0.2f, 0.18f, 0.16f));
        for (int32 k = 0; k < 4; ++k) { Cyl(FVector(-700 + k * 470, 880, 60), FVector(10, 10, 120), C(0.3f, 0.2f, 0.1f)); Light(FVector(-700 + k * 470, 880, 150), C(1, 0.6f, 0.2f), 1500.f, 600.f); Sph(FVector(-700 + k * 470, 880, 135), FVector(12, 12, 20), Glow(1, 0.6f, 0.1f, 8.f)); }
        Cyl(FVector(800, -600, 120), FVector(14, 14, 400), C(0.7f, 0.55f, 0.15f), FRotator(0, 0, 55));
        Cyl(FVector(500, 500, 100), FVector(200, 200, 200), C(0.8f, 0.65f, 0.2f), FRotator::ZeroRotator, 20.f);
        Sph(FVector(1000, 0, 420), FVector(280, 280, 280), C(0.02f, 0.02f, 0.03f));
        Sph(FVector(1030, 0, 420), FVector(340, 340, 340), Glow(1, 0.8f, 0.4f, 5.f));
        Stars(40, 3500, C(0.8f, 0.85f, 1));
        break;
    case 5: // Cidade subterranea 2180
        Env(C(0.4f, 0.2f, 0.6f), 0.2f, C(0.2f, 0.05f, 0.3f), 0.008f, 1.2f);
        Room(FVector(3000, 2200, 800), C(0.05f, 0.05f, 0.08f), C(0.06f, 0.06f, 0.09f), C(0.03f, 0.03f, 0.05f));
        for (int32 k = 0; k < 14; ++k)
        {
            const float x = -1300 + k * 200, y = (k % 2 ? 1 : -1) * 980;
            const float H = 300 + (k * 53 % 350);
            Box(FVector(x, y, H / 2), FVector(180, 180, H), C(0.07f, 0.07f, 0.1f));
            const FLinearColor N = (k % 3 == 0) ? Glow(1, 0.1f, 0.7f) : (k % 3 == 1 ? Glow(0.1f, 0.9f, 1.f) : Glow(0.6f, 0.2f, 1.f));
            Box(FVector(x, y * 0.91f, H * 0.6f), FVector(120, 4, 30), N);
            Light(FVector(x, y * 0.8f, H * 0.6f), N * 0.25f, 4000.f, 800.f);
        }
        for (int32 k = 0; k < 6; ++k) Sph(FVector(-300 + k * 220, (k % 2 ? 300 : -300), 380 + k * 20), FVector(40, 40, 40), Glow(0.2f, 0.8f, 1.f, 3.f));
        break;
    case 6: // Estacao abandonada
        Env(C(0.5f, 0.2f, 0.15f), 0.3f, C(0.35f, 0.08f, 0.05f), 0.01f, 0.9f);
        Room(FVector(2400, 1800, 500), C(0.22f, 0.22f, 0.24f), C(0.1f, 0.1f, 0.11f), C(0.1f, 0.1f, 0.11f));
        for (int32 k = -2; k <= 2; ++k) { Light(FVector(k * 420, 0, 450), C(1, 0.1f, 0.05f), 5000.f, 900.f); Box(FVector(k * 420, 0, 490), FVector(80, 60, 10), Glow(1, 0.05f, 0.02f)); }
        for (int32 k = 0; k < 9; ++k) Box(FVector(-1000 + k * 250, (k % 2 ? 700 : -700), 40 + (k % 3) * 20), FVector(120 + k * 6, 90, 70 + (k % 3) * 40), C(0.2f, 0.2f, 0.22f), FRotator(k * 15, k * 33, k * 8));
        Cyl(FVector(1000, 0, 250), FVector(40, 40, 500), C(0.18f, 0.18f, 0.2f));
        break;
    case 7: // Laboratorio 1986
        Env(C(1, 0.9f, 0.7f), 1.5f, C(0.7f, 0.65f, 0.5f), 0.002f, 0.4f);
        Room(FVector(2200, 1700, 420), C(0.78f, 0.74f, 0.6f), C(0.4f, 0.37f, 0.3f), C(0.8f, 0.78f, 0.7f));
        for (int32 k = -2; k <= 2; ++k) Light(FVector(k * 400, 0, 380), C(1, 0.95f, 0.8f), 5000.f);
        for (int32 k = 0; k < 4; ++k) { Box(FVector(-500 + k * 340, 800, 55), FVector(160, 100, 110), C(0.55f, 0.5f, 0.4f)); Box(FVector(-500 + k * 340, 760, 140), FVector(90, 70, 70), C(0.6f, 0.58f, 0.5f)); Box(FVector(-500 + k * 340, 724, 140), FVector(70, 4, 50), Glow(0.2f, 1.f, 0.3f, 2.5f)); }
        Box(FVector(1090, 0, 220), FVector(6, 700, 220), C(0.55f, 0.35f, 0.2f));
        for (int32 k = 0; k < 8; ++k) Box(FVector(1084, -300 + k * 85, 200 + (k % 3) * 40), FVector(2, 55, 40), C(0.95f, 0.95f, 0.85f));
        break;
    case 8: // Nucleo solar
        Env(C(1, 0.5f, 0.15f), 8.f, C(1, 0.4f, 0.1f), 0.012f, 0.8f);
        Room(FVector(2400, 2000, 700), C(0.3f, 0.08f, 0.03f), C(0.2f, 0.06f, 0.02f), C(0.25f, 0.07f, 0.02f));
        Sph(FVector(900, 0, 330), FVector(500, 500, 500), Glow(1, 0.45f, 0.05f, 6.f));
        Sph(FVector(900, 0, 330), FVector(580, 580, 580), Glow(1, 0.3f, 0.02f, 1.5f));
        Light(FVector(700, 0, 330), C(1, 0.45f, 0.1f), 60000.f, 3000.f);
        for (int32 k = 0; k < 8; ++k) Box(FVector(1100, -700 + k * 200, 120 + (k % 2) * 160), FVector(40, 24, 24 + (k % 3) * 40), Glow(1, 0.6f, 0.1f, 3.f), FRotator(k * 20, k * 45, 0));
        break;
    case 9: // Nucleo temporal
        Env(C(0.5f, 0.3f, 1), 0.5f, C(0.3f, 0.1f, 0.7f), 0.006f, 1.f);
        Room(FVector(2400, 2400, 800), C(0.04f, 0.03f, 0.1f), C(0.05f, 0.04f, 0.12f), C(0.03f, 0.02f, 0.08f));
        for (int32 k = 0; k < 4; ++k)
            Cyl(FVector(900, 0, 320), FVector(200 + k * 130, 200 + k * 130, 8), (k % 2) ? Glow(0.2f, 0.9f, 1.f, 3.f) : Glow(0.6f, 0.2f, 1.f, 3.f), FRotator(0, 0, 90 * (k % 2) + k * 20), 25.f + k * 15.f);
        Sph(FVector(900, 0, 320), FVector(90, 90, 90), Glow(1, 1, 1, 6.f));
        Light(FVector(800, 0, 320), C(0.6f, 0.4f, 1), 25000.f, 2500.f);
        for (int32 k = 0; k < 7; ++k) { const float a = k * 0.9f; Cyl(FVector(-200 + FMath::Cos(a) * 700, FMath::Sin(a) * 700, 200 + k * 40), FVector(70, 70, 6), Glow(0.2f, 0.9f, 1.f, 2.5f), FRotator(0, 0, 90), 40.f); }
        break;
    case 10: // Hangar NASA atual
        Env(C(1, 0.97f, 0.9f), 4.f, C(0.7f, 0.75f, 0.85f), 0.0015f, 0.4f);
        Room(FVector(3200, 2400, 800), C(0.75f, 0.78f, 0.82f), C(0.35f, 0.36f, 0.4f), C(0.7f, 0.72f, 0.76f));
        for (int32 k = -2; k <= 2; ++k) { Box(FVector(k * 600, 0, 790), FVector(300, 40, 8), Glow(1, 1, 1, 3.f)); Light(FVector(k * 600, 0, 700), C(1, 0.97f, 0.92f), 12000.f, 1800.f); }
        Cyl(FVector(1000, -500, 160), FVector(180, 180, 640), C(0.9f, 0.9f, 0.92f), FRotator(0, 0, 90));
        Shape(TEXT("Cone"), FVector(1340, -500, 160), FVector(170, 170, 160), C(0.2f, 0.2f, 0.25f), FRotator(0, 0, -90));
        Box(FVector(1180, 0, 300), FVector(8, 1000, 360), Glow(0.4f, 0.7f, 1.f, 1.2f));
        break;
    default: // Laboratorio de analise
        Env(C(0.6f, 0.75f, 1), 1.5f, C(0.2f, 0.4f, 0.8f), 0.003f, 0.6f);
        Room(FVector(2400, 1800, 500), C(0.08f, 0.1f, 0.16f), C(0.07f, 0.08f, 0.12f), C(0.06f, 0.07f, 0.1f));
        for (int32 k = 0; k < 6; ++k) { Box(FVector(-900 + k * 110, 860, 150), FVector(90, 70, 300), C(0.1f, 0.12f, 0.18f)); Box(FVector(-900 + k * 110, 822, 150), FVector(60, 3, 200), Glow(0.2f, 0.6f, 1.f, 2.f)); Light(FVector(-900 + k * 110, 700, 250), C(0.2f, 0.5f, 1.f), 2500.f, 700.f); }
        for (int32 k = 0; k < 3; ++k) Box(FVector(1180, -500 + k * 500, 260), FVector(8, 400, 220), Glow(0.2f, 0.7f, 1.f, 1.5f));
        Cyl(FVector(300, 0, 10), FVector(300, 300, 20), C(0.1f, 0.14f, 0.2f));
        break;
    }
}
