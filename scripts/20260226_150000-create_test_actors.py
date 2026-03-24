import unreal

# 이동/회전 테스트용 액터 생성 스크립트

# 기본 도형 에셋 경로
shapes = {
    "Cube": "/Engine/BasicShapes/Cube",
    "Sphere": "/Engine/BasicShapes/Sphere",
    "Cylinder": "/Engine/BasicShapes/Cylinder",
    "Cone": "/Engine/BasicShapes/Cone",
}

# 각 액터의 배치 위치 (서로 간격을 두어 구분 용이)
positions = {
    "Cube": unreal.Vector(0, 0, 100),
    "Sphere": unreal.Vector(300, 0, 100),
    "Cylinder": unreal.Vector(0, 300, 100),
    "Cone": unreal.Vector(300, 300, 100),
}

created_actors = []

for name, asset_path in shapes.items():
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if asset is None:
        print(f"[실패] {name} 에셋 로드 실패: {asset_path}")
        continue

    location = positions[name]
    rotation = unreal.Rotator(0, 0, 0)

    actor = unreal.EditorLevelLibrary.spawn_actor_from_object(asset, location, rotation)
    if actor:
        # 액터 라벨 설정 (에디터에서 식별 용이)
        label = f"Test_{name}"
        actor.set_actor_label(label)
        created_actors.append(actor)
        loc = actor.get_actor_location()
        print(f"[성공] {label} 생성 - 위치: ({loc.x}, {loc.y}, {loc.z})")
    else:
        print(f"[실패] {name} 액터 생성 실패")

print(f"\n총 {len(created_actors)}개의 테스트 액터 생성 완료")
print("\n배치 구조:")
print("  Test_Cube (0,0)       Test_Sphere (300,0)")
print("  Test_Cylinder (0,300) Test_Cone (300,300)")
print("\n이동/회전 테스트 준비 완료!")
