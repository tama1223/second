import unreal

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
eal = unreal.EditorAssetLibrary

print("=== Hero Blueprint 생성 및 설정 ===")

# ── 1. BP_HRBHeroCharacter (플레이어 영웅 BP) ──────────────
bp_hero_path = "/Game/HeroRoundBattle/Characters/BP_HRBHeroCharacter"
if not eal.does_asset_exist(bp_hero_path):
    factory = unreal.BlueprintFactory()
    factory.set_editor_property("parent_class",
        unreal.load_class(None, "/Script/second.HRBHeroCharacter"))
    bp_hero = asset_tools.create_asset("BP_HRBHeroCharacter",
        "/Game/HeroRoundBattle/Characters", None, factory)
    eal.save_asset(bp_hero_path)
    print("BP_HRBHeroCharacter 생성")
else:
    print("BP_HRBHeroCharacter 이미 존재")

# BP_HRBHeroCharacter CDO에 HealthBarComp.WidgetClass 설정
bp_hero_cls = unreal.load_class(None,
    f"{bp_hero_path}.BP_HRBHeroCharacter_C")
bp_hp_cls = unreal.load_class(None,
    "/Game/HeroRoundBattle/UI/BP_HRBHealthBarWidget.BP_HRBHealthBarWidget_C")
if bp_hero_cls and bp_hp_cls:
    cdo = unreal.get_default_object(bp_hero_cls)
    comps = cdo.get_components_by_class(unreal.WidgetComponent)
    for comp in comps:
        if 'HealthBar' in comp.get_name():
            comp.set_editor_property("widget_class", bp_hp_cls)
            print(f"  BP_HRBHeroCharacter.HealthBarComp.WidgetClass = BP_HRBHealthBarWidget ✓")
    eal.save_asset(bp_hero_path)
elif not bp_hero_cls:
    print("  WARNING: BP_HRBHeroCharacter_C 로드 실패")

# ── 2. BP_HRBEnemyHeroCharacter (적 영웅 BP) ──────────────
bp_enemy_path = "/Game/HeroRoundBattle/Characters/BP_HRBEnemyHeroCharacter"
if not eal.does_asset_exist(bp_enemy_path):
    factory = unreal.BlueprintFactory()
    factory.set_editor_property("parent_class",
        unreal.load_class(None, "/Script/second.HRBEnemyHeroCharacter"))
    bp_enemy = asset_tools.create_asset("BP_HRBEnemyHeroCharacter",
        "/Game/HeroRoundBattle/Characters", None, factory)
    eal.save_asset(bp_enemy_path)
    print("BP_HRBEnemyHeroCharacter 생성")
else:
    print("BP_HRBEnemyHeroCharacter 이미 존재")

# BP_HRBEnemyHeroCharacter CDO에 AIControllerClass + HealthBarComp 설정
bp_enemy_cls = unreal.load_class(None,
    f"{bp_enemy_path}.BP_HRBEnemyHeroCharacter_C")
bp_ai_cls = unreal.load_class(None,
    "/Game/HeroRoundBattle/AI/BP_HRBEnemyAIController.BP_HRBEnemyAIController_C")
if bp_enemy_cls:
    cdo = unreal.get_default_object(bp_enemy_cls)
    if bp_ai_cls:
        cdo.set_editor_property("ai_controller_class", bp_ai_cls)
        print("  BP_HRBEnemyHeroCharacter.AIControllerClass = BP_HRBEnemyAIController ✓")
    if bp_hp_cls:
        comps = cdo.get_components_by_class(unreal.WidgetComponent)
        for comp in comps:
            if 'HealthBar' in comp.get_name():
                comp.set_editor_property("widget_class", bp_hp_cls)
                print("  BP_HRBEnemyHeroCharacter.HealthBarComp.WidgetClass = BP_HRBHealthBarWidget ✓")
    eal.save_asset(bp_enemy_path)

# ── 3. BP_HRBGameMode에서 스폰 클래스 업데이트 ────────────
bp_gm_path = "/Game/HeroRoundBattle/Blueprints/BP_HRBGameMode"
bp_gm_cls = unreal.load_class(None,
    f"{bp_gm_path}.BP_HRBGameMode_C")
if bp_gm_cls and bp_hero_cls and bp_enemy_cls:
    cdo = unreal.get_default_object(bp_gm_cls)
    # HeroCharacterClass → BP_HRBHeroCharacter
    try:
        cdo.set_editor_property("hero_character_class", bp_hero_cls)
        print("BP_HRBGameMode.HeroCharacterClass = BP_HRBHeroCharacter ✓")
    except Exception as e:
        print(f"WARNING hero_character_class: {e}")
    # EnemyHeroCharacterClass → BP_HRBEnemyHeroCharacter
    try:
        cdo.set_editor_property("enemy_hero_character_class", bp_enemy_cls)
        print("BP_HRBGameMode.EnemyHeroCharacterClass = BP_HRBEnemyHeroCharacter ✓")
    except Exception as e:
        print(f"WARNING enemy_hero_character_class: {e}")
    eal.save_asset(bp_gm_path)

unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).save_current_level()
print("\n=== 완료 ===")
print("PIE 재실행 전 에디터에서 Compile 버튼 눌러주세요.")
