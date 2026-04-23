import unreal

# PIE 상태가 아니면 에디터 월드로 확인
world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
print(f"월드: {world.get_name()}")

# World Settings GameMode 확인
ws = world.get_world_settings()
gm_class = ws.get_editor_property("default_game_mode")
print(f"GameMode: {gm_class.get_name() if gm_class else 'None'}")

# 에셋 상태 확인
eal = unreal.EditorAssetLibrary

pawn_data = eal.load_asset("/Game/HeroRoundBattle/Data/DA_HRBPawnData")
exp_data  = eal.load_asset("/Game/HeroRoundBattle/Data/DA_HRBExperience_Default")
bp_gm     = eal.load_asset("/Game/HeroRoundBattle/Blueprints/BP_HRBGameMode")

print(f"\n=== 데이터 에셋 확인 ===")
if pawn_data:
    pawn_class = pawn_data.get_editor_property("pawn_class")
    cam_mode   = pawn_data.get_editor_property("default_camera_mode")
    print(f"DA_HRBPawnData.PawnClass        = {pawn_class.get_name() if pawn_class else 'None ← 문제!'}")
    print(f"DA_HRBPawnData.DefaultCameraMode= {cam_mode.get_name() if cam_mode else 'None ← 문제!'}")
else:
    print("DA_HRBPawnData 로드 실패 ← 문제!")

if exp_data:
    pd = exp_data.get_editor_property("default_pawn_data")
    print(f"DA_HRBExperience.DefaultPawnData= {pd.get_name() if pd else 'None ← 문제!'}")
else:
    print("DA_HRBExperience_Default 로드 실패 ← 문제!")

if bp_gm:
    bp_cls = unreal.load_class(None, "/Game/HeroRoundBattle/Blueprints/BP_HRBGameMode.BP_HRBGameMode_C")
    if bp_cls:
        cdo = unreal.get_default_object(bp_cls)
        exp = cdo.get_editor_property("default_experience")
        print(f"BP_HRBGameMode.DefaultExperience= {exp.get_name() if exp else 'None ← 문제!'}")

        # PlayerControllerClass, HUDClass 확인
        pc_class = cdo.get_editor_property("player_controller_class")
        hud_class = cdo.get_editor_property("hud_class")
        hero_class = cdo.get_editor_property("hero_character_class")
        print(f"BP_HRBGameMode.PlayerControllerClass = {pc_class.get_name() if pc_class else 'None'}")
        print(f"BP_HRBGameMode.HUDClass              = {hud_class.get_name() if hud_class else 'None'}")
        print(f"BP_HRBGameMode.HeroCharacterClass    = {hero_class.get_name() if hero_class else 'None'}")
else:
    print("BP_HRBGameMode 로드 실패 ← 문제!")
