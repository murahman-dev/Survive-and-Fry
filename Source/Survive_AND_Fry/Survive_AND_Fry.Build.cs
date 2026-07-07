using UnrealBuildTool;

public class Survive_AND_Fry : ModuleRules
{
    public Survive_AND_Fry(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Core engine modules and Niagara for particle effects (antidote, chopping, serving feedback).
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Niagara" });

        // UMG for HUD widgets (progress bar, task description, game over/win screens).
        PrivateDependencyModuleNames.AddRange(new string[] { "UMG" });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}