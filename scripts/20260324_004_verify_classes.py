import unreal

# Re-trigger compile
unreal.SystemLibrary.execute_console_command(None, "LiveCoding.Compile")

import time
time.sleep(10)

# Try to load AI controller class
try:
    cls = unreal.load_class(None, "/Script/second.HRBAIController")
    print(f"HRBAIController: {cls}")
except Exception as e:
    print(f"HRBAIController not loaded: {e}")

# Check existing class as baseline
try:
    cls2 = unreal.load_class(None, "/Script/second.HRBGameMode")
    print(f"HRBGameMode (baseline): {cls2}")
except Exception as e:
    print(f"HRBGameMode baseline failed: {e}")
