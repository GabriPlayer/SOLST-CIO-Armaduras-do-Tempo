using UnrealBuildTool;

public class SolsticioEditorTarget : TargetRules
{
    public SolsticioEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V7;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("Solsticio");
    }
}

