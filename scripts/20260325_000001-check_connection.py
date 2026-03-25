import unreal

print(f"연결 성공: {unreal.SystemLibrary.get_project_directory()}")

# 현재 레벨 확인
world = unreal.EditorLevelLibrary.get_editor_world()
print(f"현재 레벨: {world.get_name()}")

# 등록된 C++ 클래스 확인
for cls_name in ["HRBGameMode", "HRBPawnData", "HRBExperienceDefinition", "HRBCameraMode_TopDown"]:
    cls = unreal.load_class(None, f"/Script/second.{cls_name}")
    status = "✓ 발견" if cls else "✗ 없음"
    print(f"  {cls_name}: {status}")
