import unreal

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
eal = unreal.EditorAssetLibrary

print("=== Blueprint 생성 ===")

# ── 1. BP_HRBEnemyAIController ──────────────────────────
bp_ai_path = "/Game/HeroRoundBattle/AI/BP_HRBEnemyAIController"
if not eal.does_asset_exist(bp_ai_path):
    factory = unreal.BlueprintFactory()
    factory.set_editor_property("parent_class",
        unreal.load_class(None, "/Script/second.HRBEnemyAIController"))
    bp_ai = asset_tools.create_asset("BP_HRBEnemyAIController",
        "/Game/HeroRoundBattle/AI", None, factory)
    eal.save_asset(bp_ai_path)
    print("BP_HRBEnemyAIController 생성 완료")
else:
    print("BP_HRBEnemyAIController 이미 존재")

# ── 2. BP_HRBHealthBarWidget ────────────────────────────
bp_hp_path = "/Game/HeroRoundBattle/UI/BP_HRBHealthBarWidget"
if not eal.does_asset_exist(bp_hp_path):
    factory = unreal.WidgetBlueprintFactory()
    widget_cls = unreal.load_class(None, "/Script/second.HRBHealthBarWidget")
    if widget_cls:
        factory.set_editor_property("parent_class", widget_cls)
    bp_hp = asset_tools.create_asset("BP_HRBHealthBarWidget",
        "/Game/HeroRoundBattle/UI", None, factory)
    eal.save_asset(bp_hp_path)
    print("BP_HRBHealthBarWidget 생성 완료")
else:
    print("BP_HRBHealthBarWidget 이미 존재")

# ── 3. HRBEnemyHeroCharacter CDO AIControllerClass 설정 ──
bp_ai_cls = unreal.load_class(None,
    "/Game/HeroRoundBattle/AI/BP_HRBEnemyAIController.BP_HRBEnemyAIController_C")
enemy_cls = unreal.load_class(None, "/Script/second.HRBEnemyHeroCharacter")
if bp_ai_cls and enemy_cls:
    cdo = unreal.get_default_object(enemy_cls)
    try:
        cdo.set_editor_property("ai_controller_class", bp_ai_cls)
        print("HRBEnemyHeroCharacter.AIControllerClass = BP_HRBEnemyAIController ✓")
    except Exception as e:
        print(f"WARNING: {e}")

# ── 4. HRBHealthBarComponent WidgetClass 설정 ────────────
bp_hp_cls = unreal.load_class(None,
    "/Game/HeroRoundBattle/UI/BP_HRBHealthBarWidget.BP_HRBHealthBarWidget_C")
hbc_cls = unreal.load_class(None, "/Script/second.HRBHealthBarComponent")
if bp_hp_cls and hbc_cls:
    cdo = unreal.get_default_object(hbc_cls)
    try:
        cdo.set_editor_property("widget_class", bp_hp_cls)
        print("HRBHealthBarComponent.WidgetClass = BP_HRBHealthBarWidget ✓")
    except Exception as e:
        print(f"WARNING: {e}")

unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).save_current_level()

print("\n=== 자동화 완료 ===")
print("\n수동으로 해야 할 것 (에디터에서):")
print("1. BB_HRBEnemy 열기 → 'New Key' → Object 타입, 이름: TargetActor, Base Class: Actor")
print("2. BT_HRBEnemy 열기 → BlackboardAsset = BB_HRBEnemy 설정")
print("   → Selector 노드 추가 → 하위에 Sequence → 하위에 FindTarget, MoveToTarget Task 추가")
print("3. BP_HRBEnemyAIController 열기 → BehaviorTree = BT_HRBEnemy 설정")
print("4. BP_HRBHealthBarWidget 열기 → ProgressBar 위젯 추가 (변수명 반드시 'HPBar')")
