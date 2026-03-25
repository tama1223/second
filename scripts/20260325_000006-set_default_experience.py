import unreal

# HRBGameMode CDO에서 DefaultExperience 설정 시도
gamemode_cls = unreal.load_class(None, "/Script/second.HRBGameMode")
if gamemode_cls:
    cdo = unreal.get_default_object(gamemode_cls)
    props = [p for p in dir(cdo) if 'experience' in p.lower() or 'default' in p.lower()]
    print("HRBGameMode 프로퍼티:")
    for p in props[:15]:
        print(f"  {p}")

    exp_asset = unreal.EditorAssetLibrary.load_asset("/Game/HeroRoundBattle/Data/DA_HRBExperience_Default")
    if exp_asset and hasattr(cdo, 'default_experience'):
        cdo.set_editor_property("default_experience", exp_asset)
        print("DefaultExperience 설정 완료 ✓")
    else:
        print(f"exp_asset={exp_asset}")
