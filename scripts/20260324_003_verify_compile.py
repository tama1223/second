import unreal
import time

# Wait for Live Coding to finish
time.sleep(5)

# Verify AI classes exist after compilation
try:
    ai_class = unreal.find_class("HRBAIController")
    if ai_class:
        print(f"SUCCESS: HRBAIController class found: {ai_class}")
    else:
        print("WARNING: HRBAIController class not found - compilation may still be in progress")
except Exception as e:
    print(f"Check failed: {e}")

# Also check BT task classes
for class_name in ["BTTask_HRBSelectTarget", "BTTask_HRBExecuteCommand", "BTService_HRBUpdatePerception"]:
    try:
        cls = unreal.find_class(class_name)
        print(f"  {class_name}: {'FOUND' if cls else 'NOT FOUND'}")
    except:
        print(f"  {class_name}: NOT FOUND")
