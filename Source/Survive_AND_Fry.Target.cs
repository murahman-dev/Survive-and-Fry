using UnrealBuildTool;
using System.Collections.Generic;

public class Survive_AND_FryTarget : TargetRules
{
	public Survive_AND_FryTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "Survive_AND_Fry" } );
	}
}
