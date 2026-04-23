import unreal

eal = unreal.EditorAssetLibrary
print("=== CDO 수정 ===")

# 1. HRBHealthBarComponent.WidgetClass 연결
bp_hp_cls = unreal.load_class(None,
    "/Game/HeroRoundBattle/UI/BP_HRBHealthBarWidget.BP_HRBHealthBarWidget_C")
hbc_cls = unreal.load_class(None, "/Script/second.HRBHealthBarComponent")

if bp_hp_cls and hbc_cls:
    cdo = unreal.get_default_object(hbc_cls)
    cdo.set_editor_property("widget_class", bp_hp_cls)
    print(f"HRBHealthBarComponent.WidgetClass = BP_HRBHealthBarWidget ✓")
elif not bp_hp_cls:
    print("WARNING: BP_HRBHealthBarWidget_C 로드 실패 — BP 컴파일 됐는지 확인")
else:
    print("WARNING: HRBHealthBarComponent 로드 실패")

# 2. HRBEnemyHeroCharacter.AIControllerClass = BP_HRBEnemyAIController
bp_ai_cls = unreal.load_class(None,
    "/Game/HeroRoundBattle/AI/BP_HRBEnemyAIController.BP_HRBEnemyAIController_C")
enemy_cls = unreal.load_class(None, "/Script/second.HRBEnemyHeroCharacter")

if bp_ai_cls and enemy_cls:
    cdo = unreal.get_default_object(enemy_cls)
    cdo.set_editor_property("ai_controller_class", bp_ai_cls)
    print(f"HRBEnemyHeroCharacter.AIControllerClass = BP_HRBEnemyAIController ✓")
elif not bp_ai_cls:
    print("WARNING: BP_HRBEnemyAIController_C 로드 실패 — BP 컴파일 됐는지 확인")

# 3. 결과 확인
print("\n=== 확인 ===")
if hbc_cls:
    cdo = unreal.get_default_object(hbc_cls)
    wc = cdo.get_editor_property("widget_class")
    print(f"HRBHealthBarComponent.WidgetClass  = {wc.get_name() if wc else 'None ← 실패'}")

if enemy_cls:
    cdo = unreal.get_default_object(enemy_cls)
    ai = cdo.get_editor_property("ai_controller_class")
    print(f"HRBEnemyHeroCharacter.AIControllerClass = {ai.get_name() if ai else 'None ← 실패'}")
