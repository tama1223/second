import unreal

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
eal = unreal.EditorAssetLibrary

# ── 1. HRBPawnData 생성 ──────────────────────────────────────────
pawn_data_path = "/Game/HeroRoundBattle/Data/DA_HRBPawnData"
pawn_data_cls = unreal.load_class(None, "/Script/second.HRBPawnData")

if not eal.does_asset_exist(pawn_data_path):
    pawn_data = asset_tools.create_asset(
        "DA_HRBPawnData",
        "/Game/HeroRoundBattle/Data",
        pawn_data_cls,
        None
    )
    print(f"HRBPawnData 생성 완료")
else:
    pawn_data = eal.load_asset(pawn_data_path)
    print(f"HRBPawnData 기존 로드")

# DefaultCameraMode 설정
camera_mode_cls = unreal.load_class(None, "/Script/second.HRBCameraMode_TopDown")
if pawn_data and camera_mode_cls:
    pawn_data.set_editor_property("default_camera_mode", camera_mode_cls)
    eal.save_asset(pawn_data_path)
    print("  DefaultCameraMode = HRBCameraMode_TopDown ✓")

# ── 2. HRBExperienceDefinition 생성 ──────────────────────────────
exp_path = "/Game/HeroRoundBattle/Data/DA_HRBExperience_Default"
exp_cls = unreal.load_class(None, "/Script/second.HRBExperienceDefinition")

if not eal.does_asset_exist(exp_path):
    exp_data = asset_tools.create_asset(
        "DA_HRBExperience_Default",
        "/Game/HeroRoundBattle/Data",
        exp_cls,
        None
    )
    print(f"HRBExperienceDefinition 생성 완료")
else:
    exp_data = eal.load_asset(exp_path)
    print(f"HRBExperienceDefinition 기존 로드")

# DefaultPawnData 연결
if exp_data and pawn_data:
    exp_data.set_editor_property("default_pawn_data", pawn_data)
    eal.save_asset(exp_path)
    print("  DefaultPawnData = DA_HRBPawnData ✓")

# ── 3. World Settings GameMode 설정 ─────────────────────────────
world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
world_settings = world.get_world_settings()
hrb_gamemode_cls = unreal.load_class(None, "/Script/second.HRBGameMode")
if hrb_gamemode_cls:
    world_settings.set_editor_property("game_mode_override", hrb_gamemode_cls)
    print("World Settings GameMode = HRBGameMode ✓")

print("\n=== 완료 ===")
print(f"남은 수동 작업: HRBGameMode 디테일에서 DefaultExperience = DA_HRBExperience_Default 설정")
