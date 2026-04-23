import unreal

# 1. 현재 레벨 확인
current_level = unreal.EditorLevelLibrary.get_editor_world().get_name()
print(f"현재 레벨: {current_level}")

# 2. 아레나 바닥 생성 (큰 평면)
floor_location = unreal.Vector(0, 0, 0)
floor_rotation = unreal.Rotator(0, 0, 0)

cube_asset = unreal.EditorAssetLibrary.load_asset('/Engine/BasicShapes/Cube')
floor_actor = unreal.EditorLevelLibrary.spawn_actor_from_object(
    cube_asset,
    floor_location,
    floor_rotation
)
floor_actor.set_actor_label("Arena_Floor")
floor_actor.set_actor_scale3d(unreal.Vector(50, 50, 0.5))  # 5000x5000 넓이, 50 높이
floor_actor.root_component.set_mobility(unreal.ComponentMobility.STATIC)
print("아레나 바닥 생성 완료")

# 3. 바닥 머티리얼 설정 (기본 그리드 머티리얼)
mesh_comp = floor_actor.get_component_by_class(unreal.StaticMeshComponent)
if mesh_comp:
    grid_mat = unreal.EditorAssetLibrary.load_asset('/Engine/BasicShapes/BasicShapeMaterial')
    if grid_mat:
        mesh_comp.set_material(0, grid_mat)
        print("바닥 머티리얼 설정 완료")

# 4. 경기장 경계 표시용 원형 링 (얇은 실린더 4개로 벽 표현)
# 간단하게 4면 벽으로 사각형 아레나 구성
wall_positions = [
    (unreal.Vector(2500, 0, 150), unreal.Vector(0.5, 50, 3)),    # +X 벽
    (unreal.Vector(-2500, 0, 150), unreal.Vector(0.5, 50, 3)),   # -X 벽
    (unreal.Vector(0, 2500, 150), unreal.Vector(50, 0.5, 3)),    # +Y 벽
    (unreal.Vector(0, -2500, 150), unreal.Vector(50, 0.5, 3)),   # -Y 벽
]

for i, (pos, scale) in enumerate(wall_positions):
    wall = unreal.EditorLevelLibrary.spawn_actor_from_object(
        cube_asset,
        pos,
        unreal.Rotator(0, 0, 0)
    )
    wall.set_actor_label(f"Arena_Wall_{i}")
    wall.set_actor_scale3d(scale)
    wall.root_component.set_mobility(unreal.ComponentMobility.STATIC)

print("아레나 벽 4면 생성 완료")

# 5. PlayerStart 배치
player_start_class = unreal.EditorAssetLibrary.load_blueprint_class('/Engine/BasicShapes/Cube')  # fallback
# PlayerStart는 직접 스폰
editor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

# 기존 PlayerStart 확인
all_actors = editor_subsystem.get_all_level_actors()
existing_ps = [a for a in all_actors if a.get_class().get_name() == 'PlayerStart']

if not existing_ps:
    ps = editor_subsystem.spawn_actor_from_class(
        unreal.PlayerStart,
        unreal.Vector(0, -1500, 100)
    )
    ps.set_actor_label("PlayerStart_Arena")
    print("PlayerStart 배치 완료 (0, -1500, 100)")
else:
    print(f"기존 PlayerStart {len(existing_ps)}개 발견 - 스킵")

# 6. 조명 추가 (DirectionalLight)
existing_lights = [a for a in all_actors if 'DirectionalLight' in a.get_class().get_name()]
if not existing_lights:
    light = editor_subsystem.spawn_actor_from_class(
        unreal.DirectionalLight,
        unreal.Vector(0, 0, 3000)
    )
    light.set_actor_label("Arena_Sun")
    light_comp = light.get_component_by_class(unreal.DirectionalLightComponent)
    if light_comp:
        light_comp.set_intensity(3.0)
    print("DirectionalLight 생성 완료")
else:
    print("기존 DirectionalLight 발견 - 스킵")

# 7. GameMode Override 설정
world = unreal.EditorLevelLibrary.get_editor_world()
world_settings = world.get_world_settings()

# SArenaGameMode 클래스 찾기
gamemode_class = unreal.EditorAssetLibrary.load_blueprint_class('/Game/SArenaGameMode') if unreal.EditorAssetLibrary.does_asset_exist('/Game/SArenaGameMode') else None

if gamemode_class:
    world_settings.set_editor_property('game_mode_override', gamemode_class)
    print(f"GameMode Override 설정: {gamemode_class.get_name()}")
else:
    # C++ 클래스 직접 설정 시도
    try:
        gm_class = unreal.find_class("SArenaGameMode")
        if gm_class:
            world_settings.set_editor_property('game_mode_override', gm_class)
            print("GameMode Override 설정: SArenaGameMode (C++)")
        else:
            print("WARNING: SArenaGameMode 클래스를 찾을 수 없음 - 에디터에서 수동 설정 필요")
            print("  World Settings > GameMode Override > SArenaGameMode")
    except Exception as e:
        print(f"WARNING: GameMode 설정 실패: {e}")
        print("  World Settings > GameMode Override > SArenaGameMode 수동 설정 필요")

# 8. 레벨 저장
# unreal.EditorLevelLibrary.save_current_level()
print("\n=== 아레나 레벨 세팅 완료 ===")
print("- 바닥: 5000x5000, 중앙 원점")
print("- 벽: 4면 경계")
print("- PlayerStart: (0, -1500, 100)")
print("- DirectionalLight: 위에서 비추는 태양광")
print("- GameMode: SArenaGameMode (확인 필요)")
print("\nPIE(Play) 버튼을 눌러 탑다운 카메라를 테스트하세요!")
