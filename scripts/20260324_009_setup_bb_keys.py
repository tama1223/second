import unreal

bb_asset = unreal.load_asset("/Game/HeroRoundBattle/AI/BB_HRBHero")

# Create key type instances
obj_key_cls = unreal.load_class(None, "/Script/AIModule.BlackboardKeyType_Object")
float_key_cls = unreal.load_class(None, "/Script/AIModule.BlackboardKeyType_Float")

# Create entries
entry_target = unreal.BlackboardEntry()
entry_target.set_editor_property("entry_name", "TargetActor")
kt_obj = unreal.new_object(type=obj_key_cls, outer=bb_asset)
entry_target.set_editor_property("key_type", kt_obj)

entry_hp = unreal.BlackboardEntry()
entry_hp.set_editor_property("entry_name", "SelfHPPercent")
kt_float1 = unreal.new_object(type=float_key_cls, outer=bb_asset)
entry_hp.set_editor_property("key_type", kt_float1)

entry_dist = unreal.BlackboardEntry()
entry_dist.set_editor_property("entry_name", "TargetDistance")
kt_float2 = unreal.new_object(type=float_key_cls, outer=bb_asset)
entry_dist.set_editor_property("key_type", kt_float2)

# Set all keys
bb_asset.set_editor_property("Keys", [entry_target, entry_hp, entry_dist])

# Save
unreal.EditorAssetLibrary.save_asset(bb_asset.get_path_name())

# Verify
keys = bb_asset.get_editor_property("Keys")
print(f"BB keys count: {len(keys)}")
for k in keys:
    name = k.get_editor_property("entry_name")
    kt = k.get_editor_property("key_type")
    print(f"  Key: {name} -> {kt}")

print("BB configuration complete!")

# For BT BlackboardAsset link: we'll handle it in C++ AIController
# The AIController will call UseBlackboard + RunBehaviorTree
print("Note: BT<->BB link will be handled in C++ AIController code")
