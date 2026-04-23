import unreal
import time

# Trigger Live Coding compile
unreal.SystemLibrary.execute_console_command(None, "LiveCoding.Compile")
print("Live Coding compile triggered, waiting...")

time.sleep(10)

# Verify all classes
classes_to_check = [
    "HRBAIController",
    "BTService_HRBUpdatePerception",
    "BTTask_HRBSelectTarget",
    "BTTask_HRBExecuteCommand",
    "HRBGameMode",
]

print("\n=== Class Verification ===")
all_ok = True
for cls_name in classes_to_check:
    try:
        cls = unreal.load_class(None, f"/Script/second.{cls_name}")
        if cls:
            print(f"  OK: {cls_name}")
        else:
            print(f"  FAIL: {cls_name} - not found")
            all_ok = False
    except Exception as e:
        print(f"  FAIL: {cls_name} - {e}")
        all_ok = False

# Verify assets
print("\n=== Asset Verification ===")
bt = unreal.load_asset("/Game/HeroRoundBattle/AI/BT_HRBHero")
bb = unreal.load_asset("/Game/HeroRoundBattle/AI/BB_HRBHero")
print(f"  BT asset: {'OK' if bt else 'MISSING'}")
print(f"  BB asset: {'OK' if bb else 'MISSING'}")

if bb:
    keys = bb.get_editor_property("Keys")
    print(f"  BB keys: {len(keys)}")
    for k in keys:
        print(f"    - {k.get_editor_property('entry_name')}")

# Check GameMode AI flag
print("\n=== GameMode Check ===")
try:
    gm_cls = unreal.load_class(None, "/Script/second.HRBGameMode")
    gm_cdo = unreal.get_default_object(gm_cls)
    use_ai = gm_cdo.get_editor_property("bUseAIOpponent")
    print(f"  bUseAIOpponent = {use_ai}")
except Exception as e:
    print(f"  GameMode check: {e}")

if all_ok:
    print("\n=== ALL CHECKS PASSED ===")
else:
    print("\n=== SOME CHECKS FAILED - Fallback AI will be used ===")
