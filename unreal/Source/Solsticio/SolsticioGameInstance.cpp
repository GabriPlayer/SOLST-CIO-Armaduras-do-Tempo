#include "SolsticioGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

void USolsticioGameInstance::Init()
{
    Super::Init();
    LoadMissions();
}

void USolsticioGameInstance::LoadMissions()
{
    const FString Path = FPaths::ProjectContentDir() / TEXT("Data/missions.json");
    FString Raw;
    if (!FFileHelper::LoadFileToString(Raw, *Path))
    {
        UE_LOG(LogTemp, Error, TEXT("Nao foi possivel ler %s"), *Path);
        return;
    }

    TSharedPtr<FJsonObject> Root;
    if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Raw), Root) || !Root.IsValid()) return;

    for (const TSharedPtr<FJsonValue>& V : Root->GetArrayField(TEXT("missions")))
    {
        const TSharedPtr<FJsonObject> O = V->AsObject();
        FMissionInfo M;
        M.Title = O->GetStringField(TEXT("title"));
        M.Place = O->GetStringField(TEXT("place"));
        M.LevelName = O->GetStringField(TEXT("level"));
        M.Minutes = O->GetIntegerField(TEXT("minutes"));
        for (const TSharedPtr<FJsonValue>& L : O->GetArrayField(TEXT("intro")))
        {
            const TSharedPtr<FJsonObject> LO = L->AsObject();
            FDialogueLine Line;
            Line.Speaker = LO->GetStringField(TEXT("who"));
            Line.Text = LO->GetStringField(TEXT("text"));
            M.Intro.Add(Line);
        }
        Missions.Add(M);
    }
}

bool USolsticioGameInstance::GetCurrentMission(FMissionInfo& Out) const
{
    if (!Missions.IsValidIndex(CurrentMission)) return false;
    Out = Missions[CurrentMission];
    return true;
}

void USolsticioGameInstance::StartCurrentMission()
{
    FMissionInfo M;
    if (GetCurrentMission(M)) UGameplayStatics::OpenLevel(this, FName(*M.LevelName));
}

void USolsticioGameInstance::CompleteMission()
{
    CurrentMission++;
    StartCurrentMission();
}

FString USolsticioGameInstance::GetEndingTitle() const
{
    if (Bond >= 8) return TEXT("Final: Duas Vozes");
    if (Bond >= 4) return TEXT("Final: Ecos");
    return TEXT("Final: Silencio");
}
