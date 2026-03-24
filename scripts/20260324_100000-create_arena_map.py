import unreal

# 1. 새 빈 레벨 생성
editor_level = unreal.EditorLevelLibrary
world = unreal.EditorLevelLibrary.get_editor_world()

# 새 레벨 생성 및 저장
unreal.EditorLevelLibrary.new_level("/Game/Arena")
print("새 레벨 생성됨: /Game/Arena")

# 2. 바닥 - 큰 Cylinder를 아레나 바닥으로 사용
cylinder_asset = unreal.EditorAssetLibrary.load_asset('/Engine/BasicShapes/Cylinder')
if cylinder_asset:
    floor = unreal.EditorLevelLibrary.spawn_actor_from_object(
        cylinder_asset,
        unreal.Vector(0, 0, -50),
        unreal.Rotator(0, 0, 0)
    )
    floor.set_actor_label("Arena_Floor")
    floor.set_actor_scale3d(unreal.Vector(40, 40, 1))  # 반지름 약 2000

    # 머티리얼 설정 - 어두운 회색
    mesh_comp = floor.get_component_by_class(unreal.StaticMeshComponent)
    if mesh_comp:
        mesh_comp.set_collision_enabled(unreal.CollisionEnabled.QUERY_AND_PHYSICS)
        mesh_comp.set_collision_response_to_channel(unreal.CollisionChannel.PAWN, unreal.CollisionResponse.ECR_BLOCK)
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
light_comp = dir_light.get_component_by_class(unreal.DirectionalLightComponent)
if light_comp:
    light_comp.set_intensity(3.0)
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
nav_mesh_vol.set_actor_scale3d(unreal.Vector(80, 80, 10))  # 아레나 전체 커버
print("NavMeshBoundsVolume 추가됨")

# 6. PlayerStart 2개 (양쪽)
ps1 = unreal.EditorLevelLibrary.spawn_actor_from_class(
    unreal.PlayerStart,
    unreal.Vector(-800, 0, 100),
    unreal.Rotator(0, 0, 0)
)
ps1.set_actor_label("PlayerStart_0")
print("PlayerStart 0 배치됨 (-800, 0, 100)")

ps2 = unreal.EditorLevelLibrary.spawn_actor_from_class(
    unreal.PlayerStart,
    unreal.Vector(800, 0, 100),
    unreal.Rotator(0, 180, 0)
)
ps2.set_actor_label("PlayerStart_1")
print("PlayerStart 1 배치됨 (800, 0, 100)")

# 7. World Settings - GameMode Override 설정
world_settings = unreal.EditorLevelLibrary.get_editor_world().get_world_settings()
game_mode_class = unreal.load_class(None, "/Script/second.HRBGameMode")
if game_mode_class:
    world_settings.set_editor_property("default_game_mode", game_mode_class)
    print("GameMode Override 설정됨: HRBGameMode")
else:
    print("WARNING: HRBGameMode 클래스를 찾을 수 없음 - 빌드 후 수동 설정 필요")

# 레벨 저장
unreal.EditorLevelLibrary.save_current_level()
print("레벨 저장 완료: /Game/Arena")

print("\n=== 아레나 맵 생성 완료 ===")
