import unreal

# Trigger Live Coding compile via console command
unreal.SystemLibrary.execute_console_command(None, "LiveCoding.Compile")
print("Live Coding compile command sent!")
