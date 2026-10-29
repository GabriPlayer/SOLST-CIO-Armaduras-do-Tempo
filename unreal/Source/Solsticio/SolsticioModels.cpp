#include "SolsticioModels.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/Actor.h"

namespace SolsticioModels
{
    UStaticMeshComponent* AddPart(AActor* Owner, USceneComponent* Parent, const TCHAR* Shape,
        const FVector& Loc, const FVector& SizeCm, const FRotator& Rot, const FLinearColor& Color)
    {
        const FString MeshPath = FString::Printf(TEXT("/Engine/BasicShapes/%s.%s"), Shape, Shape);
        UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, *MeshPath);
        UMaterialInterface* Base = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));

        UStaticMeshComponent* C = NewObject<UStaticMeshComponent>(Owner);
        C->SetMobility(EComponentMobility::Movable);
        C->SetStaticMesh(Mesh);
        C->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        C->SetCastShadow(true);
        C->RegisterComponent();
        C->AttachToComponent(Parent, FAttachmentTransformRules::KeepRelativeTransform);
        C->SetRelativeLocationAndRotation(Loc, Rot);
        C->SetRelativeScale3D(SizeCm / 100.f);
        if (Base)
        {
            UMaterialInstanceDynamic* M = UMaterialInstanceDynamic::Create(Base, Owner);
            M->SetVectorParameterValue(TEXT("Color"), Color);
            C->SetMaterial(0, M);
        }
        return C;
    }

    void BuildHumanoid(AActor* Owner, USceneComponent* P, float F, const FHumanoidStyle& S)
    {
        const FLinearColor Boot = S.Suit * 0.4f;
        const FRotator Z = FRotator::ZeroRotator;

        // Pernas e botas
        for (float Side : { -1.f, 1.f })
        {
            AddPart(Owner, P, TEXT("Cylinder"), FVector(0, Side * 11, F + 42), FVector(17, 17, 70), Z, S.Suit);
            AddPart(Owner, P, TEXT("Cube"), FVector(5, Side * 11, F + 5), FVector(30, 16, 10), Z, Boot);
            if (S.bArmor)
            {
                AddPart(Owner, P, TEXT("Cube"), FVector(5, Side * 11, F + 55), FVector(20, 20, 30), Z, S.Plate);
                AddPart(Owner, P, TEXT("Cube"), FVector(8, Side * 11, F + 20), FVector(22, 19, 14), Z, S.Plate);
            }
        }

        // Tronco
        AddPart(Owner, P, TEXT("Cube"), FVector(0, 0, F + 98), FVector(22, 44, 56), Z, S.Suit);
        AddPart(Owner, P, TEXT("Cube"), FVector(0, 0, F + 76), FVector(24, 34, 12), Z, Boot);
        if (S.bArmor)
        {
            AddPart(Owner, P, TEXT("Cube"), FVector(5, 0, F + 104), FVector(20, 46, 40), Z, S.Plate);
            AddPart(Owner, P, TEXT("Sphere"), FVector(14, 0, F + 104), FVector(14, 14, 14), Z, S.Glow);
            // Mochila solar com dois paineis
            AddPart(Owner, P, TEXT("Cube"), FVector(-20, 0, F + 100), FVector(14, 40, 50), Z, S.Suit * 0.6f);
            for (float Side : { -1.f, 1.f })
                AddPart(Owner, P, TEXT("Cube"), FVector(-30, Side * 26, F + 108), FVector(3, 24, 44), FRotator(0, Side * 10, 0), FLinearColor(0.05f, 0.15f, 0.6f));
        }
        if (S.bLabCoat)
            AddPart(Owner, P, TEXT("Cube"), FVector(-2, 0, F + 82), FVector(26, 48, 70), Z, FLinearColor(0.9f, 0.9f, 0.92f));

        // Bracos e maos
        for (float Side : { -1.f, 1.f })
        {
            AddPart(Owner, P, TEXT("Cylinder"), FVector(0, Side * 30, F + 96), FVector(13, 13, 56), FRotator(0, 0, Side * 6), S.bLabCoat ? FLinearColor(0.9f, 0.9f, 0.92f) : S.Suit);
            AddPart(Owner, P, TEXT("Sphere"), FVector(0, Side * 31, F + 123), FVector(20, 20, 20), Z, S.bArmor ? S.Plate : S.Suit);
            AddPart(Owner, P, TEXT("Sphere"), FVector(0, Side * 34, F + 66), FVector(12, 12, 12), Z, S.bArmor ? S.Plate : S.Skin);
        }

        // Cabeca
        AddPart(Owner, P, TEXT("Cylinder"), FVector(0, 0, F + 130), FVector(10, 10, 8), Z, S.Skin);
        AddPart(Owner, P, TEXT("Sphere"), FVector(0, 0, F + 145), FVector(26, 24, 28), Z, S.bArmor ? S.Plate : S.Skin);
        if (S.bArmor)
        {
            AddPart(Owner, P, TEXT("Cube"), FVector(9, 0, F + 146), FVector(10, 22, 10), Z, FLinearColor(0.02f, 0.05f, 0.1f));
            AddPart(Owner, P, TEXT("Cone"), FVector(0, 0, F + 162), FVector(8, 8, 8), Z, S.Glow);
        }
        else
        {
            AddPart(Owner, P, TEXT("Sphere"), FVector(-2, 0, F + 151), FVector(28, 26, 20), Z, S.Hair);
            for (float Side : { -1.f, 1.f })
                AddPart(Owner, P, TEXT("Sphere"), FVector(11, Side * 6, F + 146), FVector(4, 4, 4), Z, FLinearColor::Black);
        }
    }

    void BuildHologramOrb(AActor* Owner, USceneComponent* P, const FLinearColor& Color)
    {
        AddPart(Owner, P, TEXT("Sphere"), FVector(0, 0, 150), FVector(50, 50, 50), FRotator::ZeroRotator, Color);
        AddPart(Owner, P, TEXT("Cylinder"), FVector(0, 0, 150), FVector(90, 90, 2), FRotator::ZeroRotator, Color * 0.6f);
        AddPart(Owner, P, TEXT("Cylinder"), FVector(0, 0, 10), FVector(60, 60, 20), FRotator::ZeroRotator, FLinearColor(0.1f, 0.1f, 0.15f));
        UPointLightComponent* L = NewObject<UPointLightComponent>(Owner);
        L->SetMobility(EComponentMobility::Movable);
        L->SetLightColor(Color.ToFColor(true));
        L->SetIntensity(4000.f);
        L->SetAttenuationRadius(500.f);
        L->RegisterComponent();
        L->AttachToComponent(P, FAttachmentTransformRules::KeepRelativeTransform);
        L->SetRelativeLocation(FVector(0, 0, 150));
    }
}
