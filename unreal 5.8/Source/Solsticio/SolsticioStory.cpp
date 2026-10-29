#include "SolsticioStory.h"
#include "Dom/JsonObject.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

static FLineData ParseLine(const TSharedPtr<FJsonObject>& O)
{
    FLineData L;
    L.Who = O->GetStringField(TEXT("who"));
    L.Text = O->GetStringField(TEXT("text"));
    return L;
}

static void ParseStrings(const TSharedPtr<FJsonObject>& O, const TCHAR* Field, TArray<FString>& Out)
{
    const TArray<TSharedPtr<FJsonValue>>* Arr;
    if (O->TryGetArrayField(Field, Arr))
        for (const TSharedPtr<FJsonValue>& V : *Arr) Out.Add(V->AsString());
}

bool FCampaign::Load()
{
    FString Json;
    if (!FFileHelper::LoadFileToString(Json, *(FPaths::ProjectContentDir() / TEXT("Data/campaign.json"))))
        return false;
    TSharedPtr<FJsonObject> Root;
    if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Json), Root) || !Root.IsValid())
        return false;

    for (const TSharedPtr<FJsonValue>& MV : Root->GetArrayField(TEXT("missions")))
    {
        const TSharedPtr<FJsonObject> MO = MV->AsObject();
        FMission M;
        M.Title = MO->GetStringField(TEXT("title"));
        M.Place = MO->GetStringField(TEXT("place"));
        for (const TSharedPtr<FJsonValue>& SV : MO->GetArrayField(TEXT("steps")))
        {
            const TSharedPtr<FJsonObject> SO = SV->AsObject();
            const FString Type = SO->GetStringField(TEXT("type"));
            FStep S;
            if (Type == TEXT("line"))
            {
                const FLineData L = ParseLine(SO);
                S.Type = EStep::Line; S.Who = L.Who; S.Text = L.Text;
                M.Speakers.Add(L.Who);
            }
            else if (Type == TEXT("choice"))
            {
                S.Type = EStep::Choice;
                S.Text = SO->GetStringField(TEXT("prompt"));
                for (const TSharedPtr<FJsonValue>& OV : SO->GetArrayField(TEXT("options")))
                {
                    const TSharedPtr<FJsonObject> OO = OV->AsObject();
                    FChoiceOpt Opt;
                    Opt.Text = OO->GetStringField(TEXT("text"));
                    Opt.Bond = (int32)OO->GetNumberField(TEXT("bond"));
                    for (const TSharedPtr<FJsonValue>& LV : OO->GetArrayField(TEXT("lines")))
                    {
                        Opt.Lines.Add(ParseLine(LV->AsObject()));
                        M.Speakers.Add(Opt.Lines.Last().Who);
                    }
                    S.Options.Add(Opt);
                }
            }
            else
            {
                S.Type = Type == TEXT("sequence") ? EStep::Sequence : Type == TEXT("timing") ? EStep::Timing
                    : Type == TEXT("slider") ? EStep::Slider : EStep::Targets;
                S.Text = SO->GetStringField(TEXT("prompt"));
                ParseStrings(SO, TEXT("items"), S.Items);
                ParseStrings(SO, TEXT("ordered"), S.Ordered);
                S.Width = (float)SO->GetNumberField(TEXT("width"));
                S.Speed = (float)SO->GetNumberField(TEXT("speed"));
                S.Target = (float)SO->GetNumberField(TEXT("target"));
                S.Tol = (float)SO->GetNumberField(TEXT("tol"));
                S.Count = (int32)SO->GetNumberField(TEXT("count"));
                S.Time = (float)SO->GetNumberField(TEXT("time"));
            }
            M.Steps.Add(S);
        }
        Missions.Add(M);
    }
    for (const TSharedPtr<FJsonValue>& EV : Root->GetArrayField(TEXT("endings")))
    {
        const TSharedPtr<FJsonObject> EO = EV->AsObject();
        FEnding E;
        E.Min = (int32)EO->GetNumberField(TEXT("min"));
        E.Title = EO->GetStringField(TEXT("title"));
        E.Text = EO->GetStringField(TEXT("text"));
        Endings.Add(E);
    }
    return Missions.Num() > 0;
}
