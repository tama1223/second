import unreal

editor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

# 기존 NavMeshBoundsVolume 확인
all_actors = editor_subsystem.get_all_level_actors()
existing = [a for a in all_actors if 'NavMesh' in a.get_class().get_name()]

if existing:
    print(f"NavMeshBoundsVolume 이미 존재: {existing[0].get_name()}")
else:
    # NavMeshBoundsVolume 스폰
    nav_volume = editor_subsystem.spawn_actor_from_class(
        unreal.NavMeshBoundsVolume,
        unreal.Vector(0, 0, 200)
    )

    if nav_volume:
        nav_volume.set_actor_label("NavMeshBoundsVolume")
        # 아레나 전체를 커버하는 크기 (5000x5000x1000)
        brush_comp = nav_volume.get_component_by_class(unreal.BrushComponent)
        nav_volume.set_actor_scale3d(unreal.Vector(50, 50, 10))
        print("NavMeshBoundsVolume 배치 완료 (5000x5000x1000)")
    else:
        print("ERROR: NavMeshBoundsVolume 스폰 실패")

# 레벨 저장
unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).save_current_level()
print("레벨 저장 완료")
print("에디터에서 P 키를 누르면 NavMesh(초록색)가 생성된 것을 확인할 수 있습니다.")
