import unreal

print("언리얼 에디터 연결 성공!")
print(f"프로젝트: {unreal.SystemLibrary.get_project_directory()}")
print(f"엔진 버전: {unreal.SystemLibrary.get_engine_version()}")
