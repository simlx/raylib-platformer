@echo off
gcc src/main.c -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows -Os -s -flto -o mygame.exe
