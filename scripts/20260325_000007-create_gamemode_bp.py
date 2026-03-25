import unreal

# BP_HRBGameMode 블루프린트 생성 (C++ HRBGameMode 서브클래스)
bp_path = "/Game/HeroRoundBattle/Blueprints/BP_HRBGameMode"
eal = unreal.EditorAssetLibrary

if not eal.does_asset_exist(bp_path):
    factory = unreal.BlueprintFactory()
    factory.set_editor_property("parent_class", unreal.load_class(None, "/Script/second.HRBGameMode"))
    bp = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        "BP_HRBGameMode",
        "/Game/HeroRoundBattle/Blueprints",
        None,
        factory
    )
    print(f"BP_HRBGameMode 생성 완료: {bp_path}")
else:
    bp = eal.load_asset(bp_path)
    print(f"BP_HRBGameMode 기존 로드")

# BP의 CDO에 DefaultExperience 설정
if bp:
    bp_cls = unreal.load_class(None, f"{bp_path}.BP_HRBGameMode_C")
    if bp_cls:
        cdo = unreal.get_default_object(bp_cls)
        exp_asset = eal.load_asset("/Game/HeroRoundBattle/Data/DA_HRBExperience_Default")
        if exp_asset:
            cdo.set_editor_property("default_experience", exp_asset)
            eal.save_asset(bp_path)
            print("  DefaultExperience = DA_HRBExperience_Default ✓")
        else:
            print("  ERROR: Experience 에셋 로드 실패")
    else:
        print(f"  BP 클래스 로드 실패 (컴파일 필요할 수 있음)")

# World Settings GameMode를 BP로 변경
world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
world_settings = world.get_world_settings()
bp_gamemode_cls = unreal.load_class(None, f"{bp_path}.BP_HRBGameMode_C")
if bp_gamemode_cls:
    world_settings.set_editor_property("default_game_mode", bp_gamemode_cls)
    unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).save_current_level()
    print("World Settings GameMode = BP_HRBGameMode ✓")
    print("레벨 저장 완료")
else:
    print("World Settings GameMode 수동 설정 필요: BP_HRBGameMode")
