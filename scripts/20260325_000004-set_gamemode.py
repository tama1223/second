import unreal

world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
world_settings = world.get_world_settings()

# 프로퍼티 이름 탐색
props = [p for p in dir(world_settings) if 'game' in p.lower() or 'mode' in p.lower()]
print("WorldSettings game/mode 관련 프로퍼티:")
for p in props[:20]:
    print(f"  {p}")
