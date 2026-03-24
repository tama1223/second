import unreal

# 현재 레벨은 이미 /Game/Arena로 생성됨 - 액터들 배치

# 2. 바닥 - 큰 Cylinder를 아레나 바닥으로 사용
cylinder_asset = unreal.EditorAssetLibrary.load_asset('/Engine/BasicShapes/Cylinder')
if cylinder_asset:
    level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)

    floor = unreal.EditorLevelLibrary.spawn_actor_from_object(
        cylinder_asset,
        unreal.Vector(0, 0, -50),
        unreal.Rotator(0, 0, 0)
    )
    floor.set_actor_label("Arena_Floor")
    floor.set_actor_scale3d(unreal.Vector(40, 40, 1))
    print("아레나 바닥 생성됨")
else:
    print("ERROR: Cylinder 에셋을 찾을 수 없음")

# 3. 디렉셔널 라이트
dir_light = unreal.EditorLevelLibrary.spawn_actor_from_class(
    unreal.DirectionalLight,
    unreal.Vector(0, 0, 500),
    unreal.Rotator(-45, -45, 0)
)
dir_light.set_actor_label("Arena_DirectionalLight")
print("디렉셔널 라이트 추가됨")

# 4. 스카이라이트
sky_light = unreal.EditorLevelLibrary.spawn_actor_from_class(
    unreal.SkyLight,
    unreal.Vector(0, 0, 300),
    unreal.Rotator(0, 0, 0)
)
sky_light.set_actor_label("Arena_SkyLight")
print("스카이라이트 추가됨")

# 5. NavMeshBoundsVolume
nav_mesh_vol = unreal.EditorLevelLibrary.spawn_actor_from_class(
    unreal.NavMeshBoundsVolume,
    unreal.Vector(0, 0, 200),
    unreal.Rotator(0, 0, 0)
)
nav_mesh_vol.set_actor_label("Arena_NavMeshBounds")
nav_mesh_vol.set_actor_scale3d(unreal.Vector(80, 80, 10))
print("NavMeshBoundsVolume 추가됨")

# 6. PlayerStart 2개 (양쪽)
ps1 = unreal.EditorLevelLibrary.spawn_actor_from_class(
    unreal.PlayerStart,
    unreal.Vector(-800, 0, 100),
    unreal.Rotator(0, 0, 0)
)
ps1.set_actor_label("PlayerStart_0")
print("PlayerStart 0 배치됨")

ps2 = unreal.EditorLevelLibrary.spawn_actor_from_class(
    unreal.PlayerStart,
    unreal.Vector(800, 0, 100),
    unreal.Rotator(0, 180, 0)
)
ps2.set_actor_label("PlayerStart_1")
print("PlayerStart 1 배치됨")

# 7. World Settings - GameMode Override
world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
world_settings = world.get_world_settings()
try:
    game_mode_class = unreal.load_class(None, "/Script/second.HRBGameMode")
    if game_mode_class:
        world_settings.set_editor_property("default_game_mode", game_mode_class)
        print("GameMode Override: HRBGameMode 설정됨")
    else:
        print("WARNING: HRBGameMode 클래스를 찾을 수 없음")
except Exception as e:
    print(f"WARNING: GameMode 설정 실패 - {e}")
    print("에디터 World Settings에서 수동으로 HRBGameMode를 설정하세요")

# 저장
unreal.EditorLevelLibrary.save_current_level()
print("\n=== 아레나 맵 설정 완료 ===")
