@echo off

gcc src\main.c -o mygame -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm

pause