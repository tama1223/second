import unreal

bt_asset = unreal.load_asset("/Game/HeroRoundBattle/AI/BT_HRBHero")
bb_asset = unreal.load_asset("/Game/HeroRoundBattle/AI/BB_HRBHero")

# Try all possible property names for BB link on BT
for prop_name in ["BlackboardAsset", "blackboard", "Blackboard", "BlackboardData"]:
    try:
        bt_asset.set_editor_property(prop_name, bb_asset)
        print(f"SUCCESS: Set BT.{prop_name} = BB")
        break
    except Exception as e:
        print(f"  {prop_name}: {e}")

# Save BT
try:
    unreal.EditorAssetLibrary.save_asset("/Game/HeroRoundBattle/AI/BT_HRBHero")
    print("BT saved")
except Exception as e:
    print(f"BT save failed: {e}")

# Try to configure BB keys
print("\n=== Configuring BB keys ===")
try:
    keys = bb_asset.get_editor_property("Keys")
    print(f"Current keys (via 'Keys'): {keys}")
except:
    try:
        keys = bb_asset.get_editor_property("keys")
        print(f"Current keys (via 'keys'): {keys}")
    except Exception as e:
        print(f"Cannot read keys: {e}")

# Try to get parent keys
try:
    pkeys = bb_asset.get_editor_property("ParentKeys")
    print(f"ParentKeys: {pkeys}")
except:
    pass

# Try direct approach with new_object
print("\n=== Attempting key creation ===")
try:
    # Check if key types are available
    float_type = unreal.new_object(type=unreal.load_class(None, "/Script/AIModule.BlackboardKeyType_Float"))
    print(f"Float key type created: {float_type}")
except Exception as e:
    print(f"Float key type: {e}")

try:
    obj_type = unreal.new_object(type=unreal.load_class(None, "/Script/AIModule.BlackboardKeyType_Object"))
    print(f"Object key type created: {obj_type}")
except Exception as e:
    print(f"Object key type: {e}")
