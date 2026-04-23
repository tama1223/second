import unreal
import time

unreal.SystemLibrary.execute_console_command(None, "LiveCoding.Compile")
print("Compiling camera + selection fix...")
time.sleep(10)

cls1 = unreal.load_class(None, "/Script/second.HRBPlayerController")
cls2 = unreal.load_class(None, "/Script/second.HRBHUD")
print(f"HRBPlayerController: {'OK' if cls1 else 'FAIL'}")
print(f"HRBHUD: {'OK' if cls2 else 'FAIL'}")
print("Done! Restart PIE to test camera and input.")
