@echo off

xxd -i res\coin.png                 > include\texture_coin.h
xxd -i res\door.png                 > include\texture_door.h
xxd -i res\flag.png                 > include\texture_flag.h
xxd -i res\fuel.png                 > include\texture_fuel.h
xxd -i res\jetpack_particle.png     > include\texture_jetpack_particle.h
xxd -i res\key.png                  > include\texture_key.h
xxd -i res\jetpack_meter.png        > include\texture_jetpack_meter.h
xxd -i res\map_tile.png             > include\texture_map_tile.h
xxd -i res\none.png                 > include\texture_none.h
xxd -i res\player_base.png          > include\texture_player_base.h
xxd -i res\player_base_invert.png   > include\texture_player_base_invert.h
xxd -i res\player_falling.png       > include\texture_player_falling.h
xxd -i res\player_falling_invert.png> include\texture_player_falling_invert.h
xxd -i res\spikes.png               > include\texture_spikes.h

xxd -i res\player.png               > include\texture_player.h
xxd -i res\player_invert.png        > include\texture_player_invert.h

xxd -i res\Minecraft.ttf            > include\font_minecraft.h

xxd -i res\sfxr_door_open.wav       > include\sound_door_open.h
xxd -i res\sfxr_jetpack.wav         > include\sound_jetpack.h
xxd -i res\sfxr_pickup_coin.wav     > include\sound_pickup_coin.h
xxd -i res\sfxr_pickup_fuel.wav     > include\sound_pickup_fuel.h
xxd -i res\sfxr_step.wav            > include\sound_step.h
xxd -i res\sfxr_win.wav             > include\sound_win.h
xxd -i res\sfxr_dead.wav            > include\sound_dead.h