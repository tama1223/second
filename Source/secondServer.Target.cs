using UnrealBuildTool;
using System.Collections.Generic;

public class secondServerTarget : TargetRules
{
	public secondServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		ExtraModuleNames.Add("second");
	}
}
