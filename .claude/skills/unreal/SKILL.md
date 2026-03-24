---
name: unreal
description: 언리얼 엔진 에디터에서 Python 코드를 원격으로 실행하여 액터 생성, 에셋 조작, 레벨 수정 등의 작업을 수행합니다. 사용자가 언리얼 엔진, 메타휴먼, 레벨 디자인, 액터 조작, Python 스크립트 실행을 언급하거나 unreal-remote-execution 작업을 요청할 때 사용합니다.
allowed-tools: Read, Write, Bash, Glob, Grep, Find
model: opus
---

# 언리얼 엔진 원격 제어 스킬

언리얼 엔진 에디터에 Python 코드를 원격으로 전송하여 실행하는 skill입니다. `unreal-remote-execution` npm 패키지를 사용합니다.

## 실행 지침

사용자의 언리얼 엔진 작업 요청을 받으면 아래 단계를 순서대로 따릅니다.

### 1단계: 요청 분석

사용자의 언리얼 엔진 작업 요청을 이해합니다.

### 2단계: Python 코드 생성

- `import unreal`로 시작
- 주요 모듈:
  - `unreal.EditorLevelLibrary` — 레벨 내 액터 조작
  - `unreal.EditorAssetLibrary` — 에셋 로드 및 관리
  - `unreal.EditorActorSubsystem` — 액터 검색 및 선택
  - `unreal.SystemLibrary` — 시스템 정보 조회
- 결과를 `print()`로 출력
- 필요시 `unreal.log()` 또는 `unreal.log_warning()` 사용

### 3단계: 스크립트 저장

- 경로: `scripts/YYYYMMDD_HHMMSS-{명령_요약}.py`
- 타임스탬프는 현재 시간 기준
- 명령_요약은 snake_case (예: `create_cube`, `list_actors`)

### 4단계: 스크립트 실행

```bash
node .claude/skills/unreal/execute_unreal.js scripts/{파일명}.py
```

`execute_unreal.js`는 이 skill 폴더 내에 포함되어 있습니다.

### 5단계: 결과 처리

- 성공: 생성/수정된 내용 요약
- 실패: 오류 메시지와 해결 방법 제시

## 예제

### 예제 1: 큐브 액터 생성

요청: "언리얼에서 원점에 큐브를 생성해줘"

```python
import unreal

actor_location = unreal.Vector(0, 0, 0)
actor_rotation = unreal.Rotator(0, 0, 0)

cube_asset = unreal.EditorAssetLibrary.load_asset('/Engine/BasicShapes/Cube')
actor = unreal.EditorLevelLibrary.spawn_actor_from_object(
    cube_asset,
    actor_location,
    actor_rotation
)

print(f"큐브 액터 생성됨: {actor.get_name()}")
```

### 예제 2: 레벨의 모든 액터 조회

요청: "현재 레벨의 모든 액터 목록을 보여줘"

```python
import unreal

editor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
actors = editor_subsystem.get_all_level_actors()

print(f"총 {len(actors)}개의 액터 발견:")
for actor in actors:
    actor_name = actor.get_name()
    actor_class = actor.get_class().get_name()
    print(f"  - {actor_name} (클래스: {actor_class})")
```

### 예제 3: 메타휴먼 컴포넌트 분석

요청: "메타휴먼의 FaceSync 컴포넌트가 있는지 확인해줘"

```python
import unreal

editor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
all_actors = editor_subsystem.get_all_level_actors()

for actor in all_actors:
    actor_name = actor.get_name()

    if 'metahuman' in actor_name.lower() or 'bp_' in actor_name.lower():
        components = actor.get_components_by_class(unreal.ActorComponent)

        print(f"\n=== {actor_name} 컴포넌트 ===")
        for component in components:
            comp_name = component.get_name()
            comp_class = component.get_class().get_name()
            print(f"  - {comp_name} ({comp_class})")

            if 'facesync' in comp_class.lower():
                print(f"    ✓ FaceSync 컴포넌트 발견!")
```

## 모범 사례

### Python 코드 작성 시

1. 항상 결과를 `print()`로 출력하여 피드백 제공
2. 중요한 작업에는 try-except 블록 사용
3. 복잡한 로직에는 한국어 주석 추가

### 스크립트 실행 시

1. 한 번에 하나의 스크립트만 실행
2. 복잡한 작업은 timeout 파라미터 증가 (기본값: 120000ms)
3. `scripts/` 폴더의 기존 스크립트 재사용 가능

## 문제 해결

### "Cannot find module 'unreal-remote-execution'" 오류

해결: `npm install -g unreal-remote-execution`

### "Timed out: Could not find a node" 오류

- 언리얼 엔진 에디터가 실행 중인지 확인
- Project Settings > Plugins > Python > "Enable Remote Execution?" 체크 확인
- 에디터 재시작

### Python 실행 오류

- 언리얼 엔진의 Output Log 패널에서 상세 오류 확인
- Python Editor Script Plugin 활성화 확인

## 주의사항

- 모든 Python 스크립트는 `scripts/` 폴더에 저장
- 파일명 형식 준수: `YYYYMMDD_HHMMSS-{명령_요약}.py`
- 실행 명령: `node .claude/skills/unreal/execute_unreal.js scripts/{파일명}.py`
- 언리얼 에디터가 실행 중이어야 함

## 참고 자료

- [Unreal Engine Python API 문서](https://docs.unrealengine.com/en-US/PythonAPI/)
- [unreal-remote-execution GitHub](https://github.com/nils-soderman/unreal-remote-execution)
- [Python Editor Script Plugin 문서](https://docs.unrealengine.com/en-US/scripting-the-editor-using-python-in-unreal-engine/)
