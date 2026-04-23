import unreal

bt_asset = unreal.load_asset("/Game/HeroRoundBattle/AI/BT_HRBHero")
bb_asset = unreal.load_asset("/Game/HeroRoundBattle/AI/BB_HRBHero")

# Inspect BT properties
print("=== BT properties ===")
for prop in dir(bt_asset):
    if not prop.startswith('_'):
        try:
            val = getattr(bt_asset, prop)
            if not callable(val):
                print(f"  {prop} = {val}")
        except:
            pass

# Inspect BB properties
print("\n=== BB properties ===")
for prop in dir(bb_asset):
    if not prop.startswith('_') and not callable(getattr(bb_asset, prop, None)):
        try:
            val = getattr(bb_asset, prop)
            print(f"  {prop} = {val}")
        except:
            pass
