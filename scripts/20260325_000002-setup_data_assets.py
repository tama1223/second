import unreal

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
editor_asset_lib = unreal.EditorAssetLibrary

# ── 1. HRBPawnData 생성 ──────────────────────────────────────────
pawn_data_path = "/Game/HeroRoundBattle/Data/DA_HRBPawnData"

if not editor_asset_lib.does_asset_exist(pawn_data_path):
    pawn_data_cls = unreal.load_class(None, "/Script/second.HRBPawnData")
    factory = unreal.DataAssetFactory()
    factory.set_editor_property("asset_class", pawn_data_cls)
    pawn_data = asset_tools.create_asset("DA_HRBPawnData", "/Game/HeroRoundBattle/Data", None, factory)
    print(f"HRBPawnData 생성: {pawn_data_path}")
else:
    pawn_data = editor_asset_lib.load_asset(pawn_data_path)
    print(f"HRBPawnData 이미 존재: {pawn_data_path}")

# DefaultCameraMode 설정
camera_mode_cls = unreal.load_class(None, "/Script/second.HRBCameraMode_TopDown")
if pawn_data and camera_mode_cls:
    pawn_data.set_editor_property("default_camera_mode", camera_mode_cls)
    editor_asset_lib.save_asset(pawn_data_path)
    print("  DefaultCameraMode = HRBCameraMode_TopDown ✓")
else:
    print(f"  WARNING: camera_mode_cls={camera_mode_cls}")

# ── 2. HRBExperienceDefinition 생성 ──────────────────────────────
exp_path = "/Game/HeroRoundBattle/Data/DA_HRBExperience_Default"

if not editor_asset_lib.does_asset_exist(exp_path):
    exp_cls = unreal.load_class(None, "/Script/second.HRBExperienceDefinition")
    factory = unreal.DataAssetFactory()
    factory.set_editor_property("asset_class", exp_cls)
    exp_data = asset_tools.create_asset("DA_HRBExperience_Default", "/Game/HeroRoundBattle/Data", None, factory)
    print(f"HRBExperienceDefinition 생성: {exp_path}")
else:
    exp_data = editor_asset_lib.load_asset(exp_path)
    print(f"HRBExperienceDefinition 이미 존재: {exp_path}")

# DefaultPawnData 연결
if exp_data and pawn_data:
    exp_data.set_editor_property("default_pawn_data", pawn_data)
    editor_asset_lib.save_asset(exp_path)
    print("  DefaultPawnData = DA_HRBPawnData ✓")

# ── 3. World Settings → GameMode + DefaultExperience 설정 ────────
world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
world_settings = world.get_world_settings()

# GameMode Override
hrb_gamemode_cls = unreal.load_class(None, "/Script/second.HRBGameMode")
if hrb_gamemode_cls:
    world_settings.set_editor_property("game_mode_override", hrb_gamemode_cls)
    print("World Settings GameMode = HRBGameMode ✓")

# HRBGameMode DefaultExperience 설정 (CDO 수정은 안 되므로 World Settings 통해 처리)
# → DefaultExperience는 에디터에서 GameMode 블루프린트 or CDO 통해 설정 필요
# 대신 현재 레벨의 HRBGameMode에 soft reference로 experience path 출력
print(f"\n수동 설정 필요:")
print(f"  World Settings > GameMode Override > HRBGameMode 확인 후")
print(f"  HRBGameMode Details > DefaultExperience = {exp_path}")

print("\n=== 데이터 에셋 세팅 완료 ===")
print(f"  DA_HRBPawnData: {pawn_data_path}")
print(f"  DA_HRBExperience_Default: {exp_path}")
