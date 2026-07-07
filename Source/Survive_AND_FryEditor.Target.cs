using UnrealBuildTool;
using System.Collections.Generic;

public class Survive_AND_FryEditorTarget : TargetRules
{
	public Survive_AND_FryEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		ExtraModuleNames.AddRange( new string[] { "Survive_AND_Fry" } );
	}
}
