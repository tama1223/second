---
name: 타이머 람다 TWeakObjectPtr 사용
description: SetTimer 람다에서 UObject/AActor this를 직접 캡처하지 말고 TWeakObjectPtr로 안전하게 캡처할 것
type: feedback
---

`GetWorldTimerManager().SetTimer(..., [this](){...}, ...)` 패턴에서 `this` 직접 캡처 금지.

**Why:** 타이머 실행 전에 액터가 이미 소멸(Destroy)된 경우 dangling pointer로 크래시 발생 가능.

**How to apply:**
```cpp
TWeakObjectPtr<AHRBHeroCharacter> WeakThis(this);
GetWorldTimerManager().SetTimer(Handle, [WeakThis]()
{
    if (WeakThis.IsValid()) { WeakThis->Destroy(); }
}, 1.0f, false);
```
