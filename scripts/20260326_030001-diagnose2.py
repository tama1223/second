import unreal

eal = unreal.EditorAssetLibrary
print("=== 상세 진단 ===\n")

# 1. BP_HRBGameMode 설정 확인
bp_gm_cls = unreal.load_class(None,
    "/Game/HeroRoundBattle/Blueprints/BP_HRBGameMode.BP_HRBGameMode_C")
if bp_gm_cls:
    cdo = unreal.get_default_object(bp_gm_cls)
    try:
        hc = cdo.get_editor_property("hero_character_class")
        print(f"BP_HRBGameMode.HeroCharacterClass       = {hc.get_name() if hc else 'None ←'}")
    except: print("HeroCharacterClass: 조회 실패")
    try:
        ec = cdo.get_editor_property("enemy_hero_character_class")
        print(f"BP_HRBGameMode.EnemyHeroCharacterClass  = {ec.get_name() if ec else 'None ←'}")
    except: print("EnemyHeroCharacterClass: 조회 실패")
    try:
        exp = cdo.get_editor_property("default_experience")
        print(f"BP_HRBGameMode.DefaultExperience        = {exp.get_name() if exp else 'None ←'}")
    except: print("DefaultExperience: 조회 실패")
else:
    print("BP_HRBGameMode_C 로드 실패!")

print()

# 2. BP_HRBEnemyHeroCharacter 설정 확인
bp_enemy_cls = unreal.load_class(None,
    "/Game/HeroRoundBattle/Characters/BP_HRBEnemyHeroCharacter.BP_HRBEnemyHeroCharacter_C")
if bp_enemy_cls:
    cdo = unreal.get_default_object(bp_enemy_cls)
    ai = cdo.get_editor_property("ai_controller_class")
    print(f"BP_HRBEnemyHeroCharacter.AIControllerClass = {ai.get_name() if ai else 'None ←'}")
    auto_ai = cdo.get_editor_property("auto_possess_ai")
    print(f"BP_HRBEnemyHeroCharacter.AutoPossessAI     = {auto_ai}")
else:
    print("BP_HRBEnemyHeroCharacter_C 로드 실패!")

print()

# 3. BP_HRBEnemyAIController BT 확인
bp_ai_cls = unreal.load_class(None,
    "/Game/HeroRoundBattle/AI/BP_HRBEnemyAIController.BP_HRBEnemyAIController_C")
if bp_ai_cls:
    cdo = unreal.get_default_object(bp_ai_cls)
    try:
        bt = cdo.get_editor_property("behavior_tree")
        print(f"BP_HRBEnemyAIController.BehaviorTree       = {bt.get_name() if bt else 'None ← 여기 문제!'}")
    except Exception as e:
        print(f"BehaviorTree 조회 실패: {e}")
else:
    print("BP_HRBEnemyAIController_C 로드 실패!")

print()

# 4. BT Blackboard 연결
bt = eal.load_asset("/Game/HeroRoundBattle/AI/BT_HRBEnemy")
if bt:
    bb = bt.get_blackboard_asset()
    print(f"BT_HRBEnemy.BlackboardAsset               = {bb.get_name() if bb else 'None ←'}")
else:
    print("BT_HRBEnemy 로드 실패!")

print()

# 5. BP_HRBHeroCharacter HealthBarComp 확인
bp_hero_cls = unreal.load_class(None,
    "/Game/HeroRoundBattle/Characters/BP_HRBHeroCharacter.BP_HRBHeroCharacter_C")
if bp_hero_cls:
    cdo = unreal.get_default_object(bp_hero_cls)
    comps = cdo.get_components_by_class(unreal.WidgetComponent)
    for c in comps:
        wc = c.get_editor_property("widget_class")
        print(f"BP_HRBHeroCharacter.{c.get_name()}.WidgetClass = {wc.get_name() if wc else 'None ←'}")
    if not comps:
        print("BP_HRBHeroCharacter: WidgetComponent 없음 ←")
else:
    print("BP_HRBHeroCharacter_C 로드 실패!")
