using UnrealBuildTool;
using System.Collections.Generic;

public class secondServerTarget : TargetRules
{
	public secondServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;
		ExtraModuleNames.Add("second");
	}
}
