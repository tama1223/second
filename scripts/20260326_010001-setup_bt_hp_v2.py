import unreal

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
eal = unreal.EditorAssetLibrary

print("=== Step 4-5 에디터 에셋 세팅 ===")

# ── 1. Blackboard 확인 (이미 생성됨) ─────────────────────
bb_path = "/Game/HeroRoundBattle/AI/BB_HRBEnemy"
bb = eal.load_asset(bb_path)
print(f"BB_HRBEnemy 로드: {bb is not None}")

# Blackboard 키 프로퍼티 이름 탐색
if bb:
    props = [p for p in dir(bb) if 'key' in p.lower() or 'parent' in p.lower()]
    print(f"BB 프로퍼티: {props[:10]}")

# ── 2. Behavior Tree 생성 ─────────────────────────────────
bt_path = "/Game/HeroRoundBattle/AI/BT_HRBEnemy"
if not eal.does_asset_exist(bt_path):
    bt = asset_tools.create_asset("BT_HRBEnemy", "/Game/HeroRoundBattle/AI",
        unreal.BehaviorTree, None)
    print("BT_HRBEnemy 생성 완료")
else:
    bt = eal.load_asset(bt_path)
    print("BT_HRBEnemy 로드")

if bt and bb:
    bt.set_editor_property("blackboard_asset", bb)
    eal.save_asset(bt_path)
    print("  BT ↔ BB 연결 완료")

# ── 3. BP_HRBEnemyAIController 생성 ──────────────────────
bp_ai_path = "/Game/HeroRoundBattle/AI/BP_HRBEnemyAIController"
if not eal.does_asset_exist(bp_ai_path):
    factory = unreal.BlueprintFactory()
    factory.set_editor_property("parent_class",
        unreal.load_class(None, "/Script/second.HRBEnemyAIController"))
    bp_ai = asset_tools.create_asset("BP_HRBEnemyAIController",
        "/Game/HeroRoundBattle/AI", None, factory)
    print("BP_HRBEnemyAIController 생성 완료")
else:
    bp_ai = eal.load_asset(bp_ai_path)
    print("BP_HRBEnemyAIController 로드")

# BP CDO에 BT 연결
if bt:
    bp_ai_cls = unreal.load_class(None,
        f"{bp_ai_path}.BP_HRBEnemyAIController_C")
    if bp_ai_cls:
        cdo = unreal.get_default_object(bp_ai_cls)
        try:
            cdo.set_editor_property("behavior_tree", bt)
            eal.save_asset(bp_ai_path)
            print("  BP_HRBEnemyAIController.BehaviorTree 연결 완료")
        except Exception as e:
            print(f"  WARNING: BT 연결 실패: {e}")

# ── 4. HRBEnemyHeroCharacter AIControllerClass 업데이트 ──
enemy_cls = unreal.load_class(None, "/Script/second.HRBEnemyHeroCharacter")
if enemy_cls:
    bp_ai_cls = unreal.load_class(None,
        f"{bp_ai_path}.BP_HRBEnemyAIController_C")
    if bp_ai_cls:
        cdo = unreal.get_default_object(enemy_cls)
        try:
            cdo.set_editor_property("ai_controller_class", bp_ai_cls)
            print("  HRBEnemyHeroCharacter.AIControllerClass 연결 완료")
        except Exception as e:
            print(f"  WARNING: AIControllerClass 연결 실패: {e}")

# ── 5. HP바 위젯 블루프린트 ────────────────────────────
bp_hp_path = "/Game/HeroRoundBattle/UI/BP_HRBHealthBarWidget"
if not eal.does_asset_exist(bp_hp_path):
    factory = unreal.WidgetBlueprintFactory()
    widget_cls = unreal.load_class(None, "/Script/second.HRBHealthBarWidget")
    if widget_cls:
        factory.set_editor_property("parent_class", widget_cls)
    bp_hp = asset_tools.create_asset("BP_HRBHealthBarWidget",
        "/Game/HeroRoundBattle/UI", None, factory)
    print("BP_HRBHealthBarWidget 생성 완료")
else:
    bp_hp = eal.load_asset(bp_hp_path)
    print("BP_HRBHealthBarWidget 로드")

# HRBHealthBarComponent WidgetClass 연결
hbc_cls = unreal.load_class(None, "/Script/second.HRBHealthBarComponent")
if hbc_cls:
    bp_hp_cls = unreal.load_class(None,
        f"{bp_hp_path}.BP_HRBHealthBarWidget_C")
    if bp_hp_cls:
        cdo = unreal.get_default_object(hbc_cls)
        try:
            cdo.set_editor_property("widget_class", bp_hp_cls)
            print("  HRBHealthBarComponent.WidgetClass 연결 완료")
        except Exception as e:
            print(f"  WARNING: WidgetClass 연결 실패: {e}")

# ── 6. 레벨 저장 ─────────────────────────────────────────
unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).save_current_level()
print("\n=== 완료 ===")
print("남은 수동 작업:")
print("  1. BT_HRBEnemy 에디터에서 열어 Selector → [FindTarget, MoveToTarget] 노드 추가")
print("  2. BP_HRBHealthBarWidget에 HPBar ProgressBar 추가 (이름 반드시 'HPBar')")
