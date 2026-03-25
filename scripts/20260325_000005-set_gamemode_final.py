import unreal

world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
world_settings = world.get_world_settings()

hrb_gamemode_cls = unreal.load_class(None, "/Script/second.HRBGameMode")
if hrb_gamemode_cls:
    world_settings.set_editor_property("default_game_mode", hrb_gamemode_cls)
    print(f"World Settings GameMode = HRBGameMode ✓")
else:
    print("ERROR: HRBGameMode 클래스를 찾을 수 없음")

# 레벨 저장
unreal.EditorLevelLibrary.save_current_level()
print("레벨 저장 완료")

print("\n=== 전체 세팅 완료 ===")
print("DA_HRBPawnData         → DefaultCameraMode = HRBCameraMode_TopDown")
print("DA_HRBExperience_Default → DefaultPawnData = DA_HRBPawnData")
print("Arena 레벨             → GameMode = HRBGameMode")
print("\n남은 수동 작업:")
print("  World Settings에서 HRBGameMode 선택 후")
print("  DefaultExperience 프로퍼티에 DA_HRBExperience_Default 연결")
