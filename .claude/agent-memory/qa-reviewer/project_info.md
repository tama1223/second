---
name: 프로젝트 기본 정보
description: second/ UE5 프로젝트 구조와 HRB 클래스 목록
type: project
---

second/는 독립 UE5 5.4 프로젝트. Lyra 직접 상속 금지, HRB 접두사 자체 클래스 사용.

주요 클래스 위치:
- Camera: Source/second/Camera/ (HRBCameraMode, HRBCameraMode_TopDown, HRBCameraComponent)
- Character: Source/second/Character/ (HRBPawnData)
- GameMode: Source/second/GameMode/ (HRBExperienceDefinition, HRBExperienceManagerComponent, HRBGameMode)
- Game: Source/second/Game/ (SArenaCharacter, SArenaGameMode - Step1 레거시)
- Hero: Source/second/Hero/ (HRBHeroCharacter)
- Player: Source/second/Player/ (HRBPlayerController, HRBSelectionHUD)

**Why:** LyraGame 모듈 직접 의존성 없이 독립 프로젝트로 유지
**How to apply:** 코드 리뷰 시 Lyra 직접 include/상속 없는지 확인
