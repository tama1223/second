import unreal

# ============================================================
# Step 1: Create Blackboard Data Asset
# ============================================================
asset_tools = unreal.AssetToolsHelpers.get_asset_tools()

# Create BB asset
bb_factory = unreal.BlackboardDataFactory()
bb_asset = asset_tools.create_asset(
    "BB_HRBHero",
    "/Game/HeroRoundBattle/AI",
    unreal.BlackboardData,
    bb_factory
)

if not bb_asset:
    print("ERROR: Failed to create Blackboard asset")
else:
    print(f"Blackboard created: {bb_asset.get_path_name()}")

    # Add keys via Python
    # TargetActor - Object key
    target_key = unreal.BlackboardKeyType_Object()
    target_entry = unreal.BlackboardEntry()
    target_entry.set_editor_property("entry_name", "TargetActor")
    target_entry.set_editor_property("key_type", target_key)

    # SelfHPPercent - Float key
    hp_key = unreal.BlackboardKeyType_Float()
    hp_entry = unreal.BlackboardEntry()
    hp_entry.set_editor_property("entry_name", "SelfHPPercent")
    hp_entry.set_editor_property("key_type", hp_key)

    # TargetDistance - Float key
    dist_key = unreal.BlackboardKeyType_Float()
    dist_entry = unreal.BlackboardEntry()
    dist_entry.set_editor_property("entry_name", "TargetDistance")
    dist_entry.set_editor_property("key_type", dist_key)

    # Set keys on blackboard
    bb_asset.set_editor_property("keys", [target_entry, hp_entry, dist_entry])

    unreal.EditorAssetLibrary.save_asset(bb_asset.get_path_name())
    print("Blackboard saved with 3 keys: TargetActor, SelfHPPercent, TargetDistance")

# ============================================================
# Step 2: Create Behavior Tree Asset
# ============================================================
bt_factory = unreal.BehaviorTreeFactory()
bt_asset = asset_tools.create_asset(
    "BT_HRBHero",
    "/Game/HeroRoundBattle/AI",
    unreal.BehaviorTree,
    bt_factory
)

if not bt_asset:
    print("ERROR: Failed to create BehaviorTree asset")
else:
    print(f"BehaviorTree created: {bt_asset.get_path_name()}")

    # Link blackboard to behavior tree
    bt_asset.set_editor_property("blackboard_asset", bb_asset)

    unreal.EditorAssetLibrary.save_asset(bt_asset.get_path_name())
    print("BehaviorTree saved and linked to Blackboard")

print("\n=== Asset creation complete ===")
print("NOTE: BT nodes must be configured in the editor.")
print("The fallback AI is active until BT is fully configured.")
