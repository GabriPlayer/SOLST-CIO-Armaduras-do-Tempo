using UnrealBuildTool;

public class SolsticioEditorTarget : TargetRules
{
    public SolsticioEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("Solsticio");
    }
}
