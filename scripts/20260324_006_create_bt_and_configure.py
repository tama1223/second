import unreal

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()

# ============================================================
# Step 1: Load or verify BB asset
# ============================================================
bb_asset = unreal.load_asset("/Game/HeroRoundBattle/AI/BB_HRBHero")
if bb_asset:
    print(f"BB asset loaded: {bb_asset.get_path_name()}")
else:
    print("ERROR: BB asset not found")

# ============================================================
# Step 2: Create BT asset
# ============================================================
bt_asset = unreal.load_asset("/Game/HeroRoundBattle/AI/BT_HRBHero")
if not bt_asset:
    bt_factory = unreal.BehaviorTreeFactory()
    bt_asset = asset_tools.create_asset(
        "BT_HRBHero",
        "/Game/HeroRoundBattle/AI",
        unreal.BehaviorTree,
        bt_factory
    )

if bt_asset:
    print(f"BT asset: {bt_asset.get_path_name()}")
    # Link blackboard
    if bb_asset:
        bt_asset.set_editor_property("blackboard_asset", bb_asset)
        unreal.EditorAssetLibrary.save_asset(bt_asset.get_path_name())
        print("BT linked to BB and saved")
else:
    print("ERROR: Failed to create/load BT asset")

# ============================================================
# Step 3: Try to add BB keys using property approach
# ============================================================
if bb_asset:
    # Get current keys
    current_keys = bb_asset.get_editor_property("keys")
    print(f"Current BB keys count: {len(current_keys)}")

    # Try to create key types via load_class
    try:
        obj_key_type_cls = unreal.load_class(None, "/Script/AIModule.BlackboardKeyType_Object")
        float_key_type_cls = unreal.load_class(None, "/Script/AIModule.BlackboardKeyType_Float")
        print(f"Key type classes: Object={obj_key_type_cls}, Float={float_key_type_cls}")

        # Create key instances
        obj_key = unreal.new_object(obj_key_type_cls)
        float_key1 = unreal.new_object(float_key_type_cls)
        float_key2 = unreal.new_object(float_key_type_cls)

        # Create entries
        entries = []

        entry1 = unreal.BlackboardEntry()
        entry1.set_editor_property("entry_name", "TargetActor")
        entry1.set_editor_property("key_type", obj_key)
        entries.append(entry1)

        entry2 = unreal.BlackboardEntry()
        entry2.set_editor_property("entry_name", "SelfHPPercent")
        entry2.set_editor_property("key_type", float_key1)
        entries.append(entry2)

        entry3 = unreal.BlackboardEntry()
        entry3.set_editor_property("entry_name", "TargetDistance")
        entry3.set_editor_property("key_type", float_key2)
        entries.append(entry3)

        bb_asset.set_editor_property("keys", entries)
        unreal.EditorAssetLibrary.save_asset(bb_asset.get_path_name())
        print("BB keys configured and saved!")
    except Exception as e:
        print(f"Could not configure BB keys via Python: {e}")
        print("BB keys need to be manually added in editor: TargetActor(Object), SelfHPPercent(Float), TargetDistance(Float)")
        print("Fallback AI is active and works without BT configuration")

print("\n=== Done ===")
