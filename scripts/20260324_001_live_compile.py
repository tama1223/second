import unreal
import subprocess
import sys

# Trigger Live Coding compile from within the editor
unreal.LiveCoding.enable_for_this_session()
unreal.LiveCoding.compile()
print("Live Coding compile triggered!")
