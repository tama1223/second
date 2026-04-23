import unreal
import time

unreal.SystemLibrary.execute_console_command(None, "LiveCoding.Compile")
print("Compiling visual updates...")
time.sleep(8)

# Quick verify
cls = unreal.load_class(None, "/Script/second.HRBHeroCharacter")
print(f"HRBHeroCharacter: {'OK' if cls else 'FAIL'}")
cls2 = unreal.load_class(None, "/Script/second.HRBHUD")
print(f"HRBHUD: {'OK' if cls2 else 'FAIL'}")
print("Done! Restart PIE to see changes.")
