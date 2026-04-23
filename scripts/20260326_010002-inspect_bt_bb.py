import unreal

eal = unreal.EditorAssetLibrary

bt = eal.load_asset("/Game/HeroRoundBattle/AI/BT_HRBEnemy")
bb = eal.load_asset("/Game/HeroRoundBattle/AI/BB_HRBEnemy")

print("BT 프로퍼티:")
for p in sorted(dir(bt)):
    if not p.startswith('_'):
        print(f"  {p}")

print("\nBB 프로퍼티:")
for p in sorted(dir(bb)):
    if not p.startswith('_'):
        print(f"  {p}")
