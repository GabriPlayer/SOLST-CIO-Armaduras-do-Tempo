#pragma once

#include "CoreMinimal.h"
#include "SolsticioTypes.generated.h"

USTRUCT(BlueprintType)
struct FDialogueLine
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Speaker;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Text;
};

USTRUCT(BlueprintType)
struct FMissionInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Title;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Place;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString LevelName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Minutes = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FDialogueLine> Intro;
};
