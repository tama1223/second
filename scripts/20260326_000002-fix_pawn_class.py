import unreal

eal = unreal.EditorAssetLibrary
pawn_data_path = "/Game/HeroRoundBattle/Data/DA_HRBPawnData"

pawn_data = eal.load_asset(pawn_data_path)
if not pawn_data:
    print("ERROR: DA_HRBPawnData 로드 실패")
else:
    # SArenaCharacter 클래스 로드
    arena_char_cls = unreal.load_class(None, "/Script/second.SArenaCharacter")
    if arena_char_cls:
        pawn_data.set_editor_property("pawn_class", arena_char_cls)
        eal.save_asset(pawn_data_path)
        print(f"DA_HRBPawnData.PawnClass = SArenaCharacter ✓")
    else:
        print("ERROR: SArenaCharacter 클래스를 찾을 수 없음")
        print("  컴파일이 완료되었는지 확인하세요")
