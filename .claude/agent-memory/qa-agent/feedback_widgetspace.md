---
name: WidgetSpace World 설정
description: HRBHealthBarComponent에서 WidgetSpace를 World로 설정해야 함 — Screen으로 잘못 구현되는 실수 발생
type: feedback
---

UHRBHealthBarComponent 생성자에서 반드시 `SetWidgetSpace(EWidgetSpace::World)` 를 사용할 것.

**Why:** Step 4~5 worker-agent 구현 시 `EWidgetSpace::Screen`으로 잘못 설정됨. 체크리스트에 World Space 여부가 명시되어 있었음.

**How to apply:** HP바 컴포넌트 코드 리뷰 시 SetWidgetSpace 호출을 반드시 확인.
