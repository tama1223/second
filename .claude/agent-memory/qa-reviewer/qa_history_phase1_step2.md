---
name: Phase1 Step1+Step2 통합 QA 이력
description: 2026-03-25 수행한 Phase1 Step1+Step2 통합 QA 결과
type: project
---

QA 수행일: 2026-03-25

## 통과 항목
- Lyra 클래스 직접 상속/include 없음 (주석에만 참조 언급)
- HRB 접두사 네이밍 규칙 준수
- UPROPERTY 매크로 UObject 포인터에 모두 사용
- Experience 로딩 흐름 (InitGameState → SetCurrentExperience → OnExperienceLoaded 콜백 → SpawnHeroes+RestartPlayer) 정상
- SArenaCharacter.AutoPossessPlayer = Disabled 유지
- HRBHeroCharacter.AutoPossessPlayer/AI = Disabled (Possess 안 함)
- Enhanced Input 바인딩 완전함 (좌클릭, 1/2/3키)
- 드래그 선택 HUD 사각형 그리기 로직 정상
- HRBGameMode.SpawnHeroes()에서 RegisterHero() 호출 확인

## 수정된 버그
- PlayerTick에서 bIsDragging 조건 → IsInputKeyDown(LeftMouseButton)으로 변경
  - 이유: bIsDragging은 DrawHUD에서만 true 설정되는데 PlayerTick이 그보다 먼저 실행되어 DragCurrentScreen 갱신 누락
  - 파일: Source/second/Player/HRBPlayerController.cpp

## 경고 항목 (수정 불필요)
- DefaultDeferredDecalMaterial 경로: BeginPlay에서 LoadObject 실패 시 데칼만 비표시, 크래시 없음
- SArenaGameMode vs HRBGameMode 공존: 에디터에서 HRBGameMode를 World Settings에 설정해야 Step2 동작

**Why:** 드래그 선택 기능은 PlayerTick 위치 갱신이 필수
**How to apply:** 향후 입력 상태 기반 조건은 bXxx 플래그 대신 IsInputKeyDown 사용 권장
