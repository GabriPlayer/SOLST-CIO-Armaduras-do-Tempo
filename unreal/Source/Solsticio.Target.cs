using UnrealBuildTool;

public class SolsticioTarget : TargetRules
{
    public SolsticioTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V7;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("Solsticio");
    }
}

