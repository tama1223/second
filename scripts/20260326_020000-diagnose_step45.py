import unreal

editor_sub = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
actors = editor_sub.get_all_level_actors()

print("=== Step 4-5 진단 ===\n")

# 영웅 캐릭터 확인
heroes = [a for a in actors if 'HRBHeroCharacter' in a.get_class().get_name()
          or 'HRBEnemyHeroCharacter' in a.get_class().get_name()]
print(f"레벨 내 영웅 액터: {len(heroes)}개 (PIE 중에만 보임)")

# CDO 확인 ─────────────────────────────────────────────
print("\n=== CDO 확인 ===")

# HRBEnemyHeroCharacter AIControllerClass
enemy_cls = unreal.load_class(None, "/Script/second.HRBEnemyHeroCharacter")
if enemy_cls:
    cdo = unreal.get_default_object(enemy_cls)
    ai_cls = cdo.get_editor_property("ai_controller_class")
    print(f"HRBEnemyHeroCharacter.AIControllerClass = {ai_cls.get_name() if ai_cls else 'None ← 문제!'}")
    auto_ai = cdo.get_editor_property("auto_possess_ai")
    print(f"HRBEnemyHeroCharacter.AutoPossessAI     = {auto_ai}")

# HRBHealthBarComponent WidgetClass
hbc_cls = unreal.load_class(None, "/Script/second.HRBHealthBarComponent")
if hbc_cls:
    cdo = unreal.get_default_object(hbc_cls)
    wc = cdo.get_editor_property("widget_class")
    print(f"HRBHealthBarComponent.WidgetClass       = {wc.get_name() if wc else 'None ← 문제!'}")
    ws = cdo.get_editor_property("widget_space")
    print(f"HRBHealthBarComponent.WidgetSpace       = {ws}")

# BP_HRBEnemyAIController BehaviorTree
bp_ai_cls = unreal.load_class(None,
    "/Game/HeroRoundBattle/AI/BP_HRBEnemyAIController.BP_HRBEnemyAIController_C")
if bp_ai_cls:
    cdo = unreal.get_default_object(bp_ai_cls)
    try:
        bt = cdo.get_editor_property("behavior_tree")
        print(f"BP_HRBEnemyAIController.BehaviorTree    = {bt.get_name() if bt else 'None ← 문제!'}")
    except Exception as e:
        print(f"BP_HRBEnemyAIController.BehaviorTree    = 조회 실패: {e}")
else:
    print("BP_HRBEnemyAIController 클래스 로드 실패 ← 문제!")

# BT_HRBEnemy Blackboard 연결 확인
bt_asset = unreal.EditorAssetLibrary.load_asset("/Game/HeroRoundBattle/AI/BT_HRBEnemy")
if bt_asset:
    bb = bt_asset.get_blackboard_asset()
    print(f"BT_HRBEnemy.BlackboardAsset             = {bb.get_name() if bb else 'None ← 문제!'}")

# HRBHeroCharacter HealthBarComponent 있는지
hero_cls = unreal.load_class(None, "/Script/second.HRBHeroCharacter")
if hero_cls:
    cdo = unreal.get_default_object(hero_cls)
    comps = cdo.get_components_by_class(unreal.ActorComponent)
    comp_names = [c.get_class().get_name() for c in comps]
    has_hpbar = any('HealthBar' in n for n in comp_names)
    print(f"HRBHeroCharacter 컴포넌트: {comp_names}")
    print(f"HRBHeroCharacter.HealthBarComp          = {'있음' if has_hpbar else 'None ← 문제!'}")

# NavMesh 확인
nav_actors = [a for a in actors if 'NavMesh' in a.get_class().get_name()]
print(f"\nNavMeshBoundsVolume: {len(nav_actors)}개 {'✓' if nav_actors else '← 없음!'}")

# GameMode 확인
world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
ws = world.get_world_settings()
gm = ws.get_editor_property("default_game_mode")
print(f"GameMode: {gm.get_name() if gm else 'None'}")
