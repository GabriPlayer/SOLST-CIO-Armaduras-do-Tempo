#pragma once

#include "CoreMinimal.h"

enum class EStep : uint8 { Line, Sequence, Timing, Slider, Targets, Choice };

struct FLineData
{
    FString Who;
    FString Text;
};

struct FChoiceOpt
{
    FString Text;
    int32 Bond = 0;
    TArray<FLineData> Lines;
};

struct FStep
{
    EStep Type = EStep::Line;
    FString Who;
    FString Text; // fala ou instrucao do mini-jogo
    TArray<FString> Items;
    TArray<FString> Ordered; // ordem correta (vazio = livre)
    float Width = 0.2f, Speed = 1.f, Target = 0.5f, Tol = 0.05f, Time = 10.f;
    int32 Count = 5;
    TArray<FChoiceOpt> Options;
};

struct FMission
{
    FString Title;
    FString Place;
    TArray<FStep> Steps;
    TSet<FString> Speakers;
};

struct FEnding
{
    int32 Min = 0;
    FString Title;
    FString Text;
};

// Campanha completa carregada de Content/Data/campaign.json.
struct FCampaign
{
    TArray<FMission> Missions;
    TArray<FEnding> Endings;
    bool Load();
};
