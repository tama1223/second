import unreal

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
eal = unreal.EditorAssetLibrary

print("=== Step 4-5 에디터 에셋 세팅 시작 ===")

# ── 1. Blackboard 에셋 생성 ──────────────────────────────
bb_path = "/Game/HeroRoundBattle/AI/BB_HRBEnemy"
if not eal.does_asset_exist(bb_path):
    bb = asset_tools.create_asset("BB_HRBEnemy", "/Game/HeroRoundBattle/AI",
        unreal.BlackboardData, None)
    print(f"BB_HRBEnemy 생성 완료")
else:
    bb = eal.load_asset(bb_path)
    print(f"BB_HRBEnemy 기존 로드")

# Blackboard에 TargetActor 키 추가
if bb:
    keys = bb.get_editor_property("keys")
    existing_keys = [k.entry_name for k in keys] if keys else []
    if "TargetActor" not in existing_keys:
        new_key = unreal.BlackboardKeyType_Object()
        entry = unreal.BlackboardEntry()
        entry.set_editor_property("entry_name", "TargetActor")
        entry.set_editor_property("key_type", new_key)
        keys.append(entry)
        bb.set_editor_property("keys", keys)
        eal.save_asset(bb_path)
        print("  TargetActor 키 추가 완료")
    else:
        print("  TargetActor 키 이미 존재")

# ── 2. Behavior Tree 에셋 생성 ──────────────────────────
bt_path = "/Game/HeroRoundBattle/AI/BT_HRBEnemy"
if not eal.does_asset_exist(bt_path):
    bt = asset_tools.create_asset("BT_HRBEnemy", "/Game/HeroRoundBattle/AI",
        unreal.BehaviorTree, None)
    print(f"BT_HRBEnemy 생성 완료")
else:
    bt = eal.load_asset(bt_path)
    print(f"BT_HRBEnemy 기존 로드")

# BT에 Blackboard 연결
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
    print(f"BP_HRBEnemyAIController 생성 완료")
else:
    bp_ai = eal.load_asset(bp_ai_path)
    print(f"BP_HRBEnemyAIController 기존 로드")

# BP_HRBEnemyAIController CDO에 BT 연결
if bp_ai and bt:
    bp_ai_cls = unreal.load_class(None,
        f"{bp_ai_path}.BP_HRBEnemyAIController_C")
    if bp_ai_cls:
        cdo = unreal.get_default_object(bp_ai_cls)
        cdo.set_editor_property("behavior_tree", bt)
        eal.save_asset(bp_ai_path)
        print("  BP_HRBEnemyAIController.BehaviorTree 연결 완료")

# ── 4. HRBEnemyHeroCharacter CDO → AIControllerClass 업데이트 ──
enemy_cls = unreal.load_class(None, "/Script/second.HRBEnemyHeroCharacter")
if enemy_cls and bp_ai:
    bp_ai_cls = unreal.load_class(None,
        f"{bp_ai_path}.BP_HRBEnemyAIController_C")
    if bp_ai_cls:
        cdo = unreal.get_default_object(enemy_cls)
        cdo.set_editor_property("ai_controller_class", bp_ai_cls)
        print("  HRBEnemyHeroCharacter.AIControllerClass = BP_HRBEnemyAIController")

# ── 5. HP바 위젯 블루프린트 생성 ────────────────────────
bp_hp_path = "/Game/HeroRoundBattle/UI/BP_HRBHealthBarWidget"
if not eal.does_asset_exist(bp_hp_path):
    factory = unreal.WidgetBlueprintFactory()
    widget_cls = unreal.load_class(None,
        "/Script/second.HRBHealthBarWidget")
    if widget_cls:
        factory.set_editor_property("parent_class", widget_cls)
    bp_hp = asset_tools.create_asset("BP_HRBHealthBarWidget",
        "/Game/HeroRoundBattle/UI", None, factory)
    print(f"BP_HRBHealthBarWidget 생성 완료")
else:
    bp_hp = eal.load_asset(bp_hp_path)
    print(f"BP_HRBHealthBarWidget 기존 로드")

# HRBHealthBarComponent CDO에 WidgetClass 연결
hbc_cls = unreal.load_class(None, "/Script/second.HRBHealthBarComponent")
if hbc_cls and bp_hp:
    bp_hp_cls = unreal.load_class(None,
        f"{bp_hp_path}.BP_HRBHealthBarWidget_C")
    if bp_hp_cls:
        cdo = unreal.get_default_object(hbc_cls)
        cdo.set_editor_property("widget_class", bp_hp_cls)
        print("  HRBHealthBarComponent.WidgetClass = BP_HRBHealthBarWidget")

print("\n=== 완료 ===")
print("남은 수동 작업:")
print("  1. BT_HRBEnemy 에디터에서 열어 Selector → [FindTarget, MoveToTarget] 노드 추가")
print("  2. BP_HRBHealthBarWidget 에디터에서 열어 HPBar ProgressBar 위젯 추가 (이름: HPBar)")
