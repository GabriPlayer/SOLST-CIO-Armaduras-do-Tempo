#include "SolsticioGameMode.h"
#include "SolsticioArmorCharacter.h"
#include "SolsticioHUD.h"
#include "SolsticioMissionController.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Engine/DirectionalLight.h"
#include "Engine/PointLight.h"
#include "Engine/SkyLight.h"
#include "Components/StaticMeshComponent.h"
#include "Components/LightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

ASolsticioGameMode::ASolsticioGameMode()
{
    DefaultPawnClass = ASolsticioArmorCharacter::StaticClass();
    PlayerControllerClass = ASolsticioMissionController::StaticClass();
    HUDClass = ASolsticioHUD::StaticClass();
}

void ASolsticioGameMode::SpawnBox(const FVector& Center, const FVector& Size, const FLinearColor& Color)
{
    UStaticMesh* Cube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
    UMaterialInterface* Base = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    AStaticMeshActor* A = GetWorld()->SpawnActor<AStaticMeshActor>(Center, FRotator::ZeroRotator);
    if (!A || !Cube) return;
    A->SetMobility(EComponentMobility::Movable);
    UStaticMeshComponent* C = A->GetStaticMeshComponent();
    C->SetStaticMesh(Cube);
    A->SetActorScale3D(Size / 100.f);
    if (Base)
    {
        UMaterialInstanceDynamic* M = UMaterialInstanceDynamic::Create(Base, A);
        M->SetVectorParameterValue(TEXT("Color"), Color);
        C->SetMaterial(0, M);
    }
}

void ASolsticioGameMode::BuildLab()
{
    UWorld* W = GetWorld();
    const float S = 1500.f, Wall = 500.f;

    SpawnBox(FVector(0, 0, -50), FVector(2 * S, 2 * S, 100), FLinearColor(0.12f, 0.13f, 0.16f));
    SpawnBox(FVector(S, 0, Wall / 2), FVector(40, 2 * S, Wall), FLinearColor(0.2f, 0.22f, 0.28f));
    SpawnBox(FVector(-S, 0, Wall / 2), FVector(40, 2 * S, Wall), FLinearColor(0.2f, 0.22f, 0.28f));
    SpawnBox(FVector(0, S, Wall / 2), FVector(2 * S, 40, Wall), FLinearColor(0.2f, 0.22f, 0.28f));
    SpawnBox(FVector(0, -S, Wall / 2), FVector(2 * S, 40, Wall), FLinearColor(0.2f, 0.22f, 0.28f));

    // Consolas, bancada e plataforma de teste da armadura.
    SpawnBox(FVector(S - 200, -600, 60), FVector(120, 500, 120), FLinearColor(0.1f, 0.5f, 0.9f));
    SpawnBox(FVector(S - 200, 600, 60), FVector(120, 500, 120), FLinearColor(0.1f, 0.5f, 0.9f));
    SpawnBox(FVector(-600, S - 250, 50), FVector(900, 200, 100), FLinearColor(0.35f, 0.3f, 0.25f));
    SpawnBox(FVector(500, 0, 15), FVector(500, 500, 30), FLinearColor(1.f, 0.7f, 0.1f));

    W->SpawnActor<APlayerStart>(FVector(-900, 0, 100), FRotator::ZeroRotator);

    if (ADirectionalLight* Sun = W->SpawnActor<ADirectionalLight>(FVector(0, 0, 1000), FRotator(-50, 30, 0)))
    {
        Sun->SetMobility(EComponentMobility::Movable);
        Sun->GetLightComponent()->SetIntensity(6.f);
    }
    for (const FVector& P : { FVector(-700, -700, 420), FVector(-700, 700, 420), FVector(700, 0, 420) })
    {
        if (APointLight* L = W->SpawnActor<APointLight>(P, FRotator::ZeroRotator))
        {
            L->SetMobility(EComponentMobility::Movable);
            L->GetLightComponent()->SetIntensity(8000.f);
            L->GetLightComponent()->SetLightColor(FColor(255, 240, 210));
        }
    }
    if (ASkyLight* Sky = W->SpawnActor<ASkyLight>())
    {
        Sky->GetLightComponent()->SetMobility(EComponentMobility::Movable);
    }
}

void ASolsticioGameMode::StartMission1()
{
    ASolsticioMissionController* PC = Cast<ASolsticioMissionController>(UGameplayStatics::GetPlayerController(this, 0));
    if (!PC) return;

    auto Line = [](const TCHAR* Who, const TCHAR* Text)
    {
        FStoryStep S; S.Speaker = Who; S.Text = Text; return S;
    };
    TArray<FStoryStep> Steps;
    Steps.Add(Line(TEXT("Narrador"), TEXT("Num laboratório subterrâneo, Tim testa a armadura Solstício. No visor surgem os módulos A a G: energia solar, propulsão, escudo, sensores, ferramentas, navegação e núcleo temporal.")));
    FStoryStep Seq;
    Seq.Type = EStoryStepType::Sequence;
    Seq.Text = TEXT("Ativa os módulos pela ordem que preferires. O último ficará mais danificado na estação solar.");
    Seq.Items = { TEXT("A Energia"), TEXT("B Propulsão"), TEXT("C Escudo"), TEXT("D Sensores"), TEXT("E Ferramentas"), TEXT("F Navegação"), TEXT("G Núcleo") };
    Steps.Add(Seq);
    Steps.Add(Line(TEXT("Tim"), TEXT("A captação está a funcionar, mas o núcleo perde estabilidade quando tentamos ativar o teletransporte.")));
    Steps.Add(Line(TEXT("Martim"), TEXT("E o que acontece se o problema aparecer perto do Sol?")));
    Steps.Add(Line(TEXT("Tim"), TEXT("Espero que não aconteça.")));
    Steps.Add(Line(TEXT("Jack"), TEXT("É um plano reconfortante.")));
    Steps.Add(Line(TEXT("Narrador"), TEXT("Martim anuncia que Jack viajará com José até uma estação solar. Jack recusa: acusa José de ter abandonado a equipa numa missão antiga.")));
    Steps.Add(Line(TEXT("José"), TEXT("Tive de escolher entre esperar por ti ou salvar o resto da equipa.")));
    Steps.Add(Line(TEXT("Martim"), TEXT("O protótipo não é seguro para uso fora do laboratório, Tim.")));
    Steps.Add(Line(TEXT("Tim"), TEXT("Foi para isto que a construí. Não para a deixar numa caixa.")));

    PC->StartMission(TEXT("Missão 1 — O Projeto Solstício"), TEXT("Centro de investigação da NASA"), Steps);
}

void ASolsticioGameMode::BeginPlay()
{
    Super::BeginPlay();
    BuildLab();
    StartMission1();
}


