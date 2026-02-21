#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "texture_coin.h"
#include "texture_door.h"
#include "texture_flag.h"
#include "texture_fuel.h"
#include "texture_jetpack_meter.h"
#include "texture_jetpack_particle.h"
#include "texture_key.h"
#include "texture_map_tile.h"
#include "texture_none.h"
#include "texture_player_base.h"
#include "texture_player_base_invert.h"
#include "texture_player_falling.h"
#include "texture_player_falling_invert.h"
#include "texture_spikes.h"
#include "texture_player.h"
#include "texture_player_invert.h"

#include "font_minecraft.h"

#include "sound_door_open.h"
#include "sound_pickup_coin.h"
#include "sound_pickup_key.h"
#include "sound_pickup_fuel.h"
#include "sound_step.h"
#include "sound_jetpack.h"
#include "sound_win.h"
#include "sound_dead.h"


#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 320

#define GRAVITY 1
#define PLAYER_SPEED 1
#define JETPACK_SPEED 1.7
#define TILES_NUM 600
#define TILE_WIDTH 16

#define GAME_SCALE 3
#define GAME_START_TICK 30
#define FUEL_DECREASE_RATE 5
#define FUEL_MAX 3000

#define LEVEL_MAX 2

// #define DEBUG_PLAYER_COLLISIONS

// basic entity
typedef struct {
    Texture2D *img;
    Rectangle rect; // Base Rectangle
    short type_id;
    short frame;    // animation frame
} ent;

// moving entity
typedef struct {
    ent base;
    Rectangle right_collision_rect;
    Rectangle left_collision_rect;
    Rectangle down_collision_rect;
    Rectangle top_collision_rect;
    Rectangle on_ground_collision_rect;
    bool inverted;
    bool on_ground;
    short walking_time;
    bool alive;
} m_ent;

// particle
typedef struct {
    ent base;
    int lifetime;
    bool alive;
} jetpack_particle;

typedef struct {
    Vector2 pos;
    int lifetime;
    bool alive;
} coin_add_text;

int player_fuel = FUEL_MAX/2;
int player_flying_time = 0;

short coins = 0;
char coin_text[100];
char win_text[100];
coin_add_text coin_add_texts[10];

bool key_collected = false;
int player_frametime = 0;
int player_frame;

bool game_over = false;
bool game_win = false;

m_ent ply;

Font minecraft_font;

ent tiles[TILES_NUM];
jetpack_particle jetpack_particles[30];
short next_jetpack_particle_count = 0;

Color FUEL_COLOR_RED = {250,50,50,255};
Color FUEL_COLOR_YELLOW = {250,250,50,255};
Color FUEL_COLOR_GREEN = {50,250,50,255};
Color background_color = {100,160,255,255}; // sky blue

int game_tick = 0;
Camera2D camera;

Texture2D player_texture                    ;
Texture2D player_invert_texture             ;
Texture2D player_falling_texture            ;
Texture2D player_falling_invert_texture     ;
Texture2D map_tile_texture                  ;
Texture2D jetpack_particle_texture          ;
Texture2D jetpack_meter_texture             ;
Texture2D coin_texture                      ;
Texture2D door_texture                      ;
Texture2D key_texture                       ;
Texture2D spikes_texture                    ;
Texture2D flag_texture                      ;
Texture2D fuel_texture                      ;
Texture2D none_texture                      ;

Texture2D player_texture_anim               ;
Texture2D player_texture_anim_invert        ;

Sound pickup_coin_sound                     ;
Sound pickup_key_sound                     ;
Sound pickup_fuel_sound                     ;
Sound win_sound                             ;
Sound jetpack_sound                         ;
Sound door_open_sound                       ;
Sound step_sound                            ;
Sound dead_sound                            ;

#define ENT_WALL    1
#define ENT_COIN    2
#define ENT_KEY     3
#define ENT_DOOR    4
#define ENT_SPIKES  5
#define ENT_FLAG    9
#define ENT_FUEL    6
#define _ 0

int current_level = 1;

int level_template[] = { // 40 x 15
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,1,
1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,1,
1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,1,
1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,1,
1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,1,
1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,1,
1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,1,
1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,1,
1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,1,
1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,1,
1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,1,
1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,1,
1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};

int level_0[] = { // 40 x 15
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,_,_,_,_,_,2,2,2,2,_,1,_,_,_,2,2,2,2,_,_,_,_,_,3,1,2,2,2,_,_,_,_,_,_,_,_,_,_,1,
1,_,_,_,_,_,_,_,_,_,_,1,6,_,_,2,2,2,2,_,_,_,1,1,1,1,2,2,2,_,_,5,_,_,5,_,_,5,_,1,
1,1,1,1,1,1,1,1,1,1,_,1,1,1,_,2,2,2,2,_,_,1,1,_,_,_,2,2,2,_,_,1,1,1,1,1,1,1,_,1,
1,1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,1,1,_,_,_,_,_,_,_,_,1,1,_,_,_,_,_,_,_,1,
1,1,_,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,_,_,_,1,1,1,1,1,1,1,6,_,_,1,5,5,5,1,1,
1,1,_,_,_,_,_,_,_,_,6,_,_,_,_,_,_,_,_,_,1,_,_,1,1,1,1,_,_,_,_,_,_,_,1,1,1,1,1,1,
1,1,1,1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,4,_,1,1,1,1,1,3,5,1,1,1,_,_,4,4,4,_,_,1,
1,2,2,_,_,_,_,_,_,_,_,_,_,_,_,1,1,1,_,1,1,1,1,1,1,1,1,1,1,1,_,_,_,1,1,1,1,1,_,1,
1,_,_,_,_,2,2,1,1,2,2,2,_,2,2,2,2,_,_,1,3,_,_,_,_,_,_,_,_,_,_,_,1,1,_,_,_,_,_,1,
1,_,1,_,_,2,2,2,2,2,2,2,_,2,2,2,2,_,_,1,1,1,5,_,_,6,_,_,1,1,1,1,1,1,_,1,1,1,1,1,
1,3,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,1,9,_,1,5,5,5,5,5,6,_,_,_,_,_,_,_,2,2,2,1,
1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,1,1,_,1,1,1,1,1,1,2,_,_,_,_,_,_,_,2,2,2,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,_,_,_,_,_,_,_,_,_,5,5,5,_,_,5,5,2,2,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};

int level_1[] = { // 40 x 15
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,_,_,_,1,_,2,2,_,1,1,_,_,_,_,_,1,1,_,_,1,1,1,1,1,1,_,_,2,2,2,2,2,2,2,_,_,_,_,1,
1,_,_,_,1,_,_,_,_,4,4,_,_,_,_,_,1,1,_,_,4,_,_,_,1,1,_,_,2,2,2,2,2,2,2,_,_,_,_,1,
1,1,1,_,1,_,_,_,1,1,1,1,_,_,_,_,1,1,_,1,1,1,1,_,1,1,_,_,2,2,2,2,2,2,2,_,_,_,_,1,
1,3,_,_,1,_,1,1,1,_,_,_,_,_,_,_,1,1,_,_,1,6,1,_,1,1,_,_,2,2,2,2,2,2,2,_,1,1,1,1,
1,2,_,2,1,_,2,2,1,_,_,_,_,_,_,_,1,1,_,_,4,_,1,_,1,1,_,_,_,_,_,_,_,_,_,_,1,_,_,1,
1,2,_,2,1,_,_,_,1,_,1,_,_,_,_,_,_,1,1,1,1,_,1,_,1,1,6,6,6,6,6,6,_,_,_,_,1,_,9,1,
1,2,_,2,1,1,1,_,1,_,1,_,_,_,_,_,_,1,6,_,1,_,1,_,1,1,1,1,1,1,1,1,1,_,_,1,1,_,1,1,
1,2,_,2,1,_,_,_,1,_,1,1,1,1,1,1,1,1,1,_,1,_,1,_,1,1,_,3,4,3,4,3,4,_,_,_,_,_,1,1,
1,_,_,_,_,_,_,_,1,_,1,1,1,1,1,1,1,1,_,_,4,_,1,_,1,1,_,1,1,1,1,1,1,1,_,1,1,1,1,1,
1,1,1,_,1,1,1,_,1,_,_,_,2,2,2,2,2,_,_,_,1,1,1,_,1,1,_,1,2,2,2,_,_,_,_,_,_,_,_,1,
1,6,_,_,1,3,_,_,1,_,_,_,2,3,3,3,2,_,_,1,1,1,1,_,_,_,_,1,2,2,2,_,_,_,_,_,_,_,_,1,
1,2,2,2,1,_,_,_,1,_,_,_,_,_,_,_,_,_,1,1,1,1,1,_,_,_,_,1,2,2,2,_,_,_,_,_,_,_,_,1,
1,5,5,2,1,5,5,5,1,_,_,_,_,_,_,_,_,1,1,1,1,1,1,5,5,5,5,1,2,2,2,5,5,5,5,5,_,_,_,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};

int* levels[] = {
    level_0,
    level_1
};

// Load the level into tiles
void load_level(int leveldata[])
{
    int x = 0;
    int y = 0;
    int spacing = TILE_WIDTH;

    for (int i = 0; i < TILES_NUM; i++)
    {
        ent *tile = &tiles[i];
        int type = leveldata[x + (y * 40)];

        tile->img = (type == 0) ? &none_texture :
                    (type == 1) ? &map_tile_texture :
                    (type == 2) ? &coin_texture :
                    (type == 3) ? &key_texture :
                    (type == 4) ? &door_texture :
                    (type == 5) ? &spikes_texture :
                    (type == 6) ? &fuel_texture :
                    (type == 9) ? &flag_texture : &none_texture;

        if (type == 0)
        {
            tile->rect.x = tile->rect.y = -999;
        }
        else
        {
            tile->rect.x = x * spacing;
            tile->rect.y = y * spacing;
        }

        tile->rect.width = tile->rect.height = TILE_WIDTH;
        tile->type_id = type;

        if (++x >= 40)
        {
            x = 0;
            y++;
        }
    }
}



void restart_level()
{
    load_level(levels[current_level]);
    ply.alive = true;
    ply.base.rect.x = 20;
    ply.base.rect.y = 20;
    player_fuel = FUEL_MAX;
    coins = 0;
    key_collected = false;
}

void next_level()
{
    current_level++;
    restart_level();
}

void register_input()
{
    if (IsKeyDown(KEY_ESCAPE)) { CloseWindow(); }

    bool key_left = IsKeyDown(KEY_LEFT);
    bool key_right = IsKeyDown(KEY_RIGHT);
    bool key_up = IsKeyDown(KEY_UP);


    if (game_win)
    {
        if (IsKeyDown(KEY_ENTER) && (current_level+1) < LEVEL_MAX)
        {
            game_win = false;
            game_over = false;
            next_level();
        }
    }

    if (!ply.alive)
    {
        if (IsKeyDown(KEY_R)) restart_level();

        return;
    }

    // Handle movement based on key presses
    if (key_right || key_left)
    {
        if (key_right)
        {
            ply.base.rect.x += PLAYER_SPEED;
            ply.inverted = false;
        }
        if (key_left)
        {
            ply.base.rect.x -= PLAYER_SPEED;
            ply.inverted = true;
        }

        if (ply.on_ground && ply.walking_time % 25 == 0) PlaySound(step_sound);

        ply.walking_time++;
    }
    else
    {
        ply.walking_time = 0; // Reset walking time if neither key is pressed
    }

    // Handle upward movement (jetpack)
    if (key_up && player_fuel > 0)
    {
        ply.base.rect.y -= JETPACK_SPEED;
        player_fuel -= FUEL_DECREASE_RATE;
        next_jetpack_particle_count++;
        ply.on_ground = false;
        player_flying_time++;
        if ( player_flying_time == 1 || game_tick % 8 == 0) PlaySound(jetpack_sound);
    }
}

void draw_text(const char* text, int x, int y, int size, Color color)
{
    DrawTextEx(minecraft_font, text, (Vector2) {x, y}, size, 4, color);
}

void draw_text_big(const char* text, int x, int y)
{
    DrawTextEx(minecraft_font, text, (Vector2) {x, y}, 16, 4, WHITE);
}

void draw_ent(ent *entity)
{
    DrawTextureRec(*entity->img,(Rectangle) {TILE_WIDTH*entity->frame,0,TILE_WIDTH,TILE_WIDTH}, (Vector2) {entity->rect.x, entity->rect.y}, WHITE);
}

void create_coin_pickup_text(int x, int y)
{
    for( int i = 0 ; i < 10 ; i++)
    {
        coin_add_text *ctext = &coin_add_texts[i];
        if (!ctext->alive)
        {
            ctext->alive = true;
            ctext->lifetime = 0;
            ctext->pos.x = x;
            ctext->pos.y = y;
            return;
        }
    }
}

void draw_jetpack_particle(jetpack_particle *jp)
{
    if (jp->alive) draw_ent(&jp->base);
}
void draw_jetpack_particles()
{
    for (int i = 0 ; i < 30 ; i++)
    {
        draw_jetpack_particle(&jetpack_particles[i]);
    }
}
void create_jetpack_particle(jetpack_particle *jp)
{
    jp->alive = true;
    jp->lifetime = 0;
    jp->base.img = &jetpack_particle_texture;
    jp->base.rect.x = ply.base.rect.x + ((ply.inverted) ? 4 : -4  );
    jp->base.rect.y = ply.base.rect.y + 6;
}
void destroy_jetpack_particle(jetpack_particle *jp)
{
    jp->alive = false;
    jp->base.rect.x = -999;
}
void update_jetpack_particles()
{
    bool spawn_next_particle = false;

    if (next_jetpack_particle_count > 1)
    {
        next_jetpack_particle_count = 0;
        spawn_next_particle = true;
    }

    for (int i = 0 ; i < 30 ; i++)
    {
        jetpack_particle *jp = &jetpack_particles[i];
        if (jp->alive)
        {
            int r = GetRandomValue(-1,1);
            int r2 = GetRandomValue(0,1);

            jp->base.rect.x += r;
            jp->base.rect.y += r2 * ((40 - jp->lifetime)/15);
            jp->lifetime++;

            if (jp->lifetime > 20) destroy_jetpack_particle(jp);
        }
        else
        {
            if (spawn_next_particle)
            {
                spawn_next_particle = false;
                create_jetpack_particle(jp);
            }
        }
    }
}
void draw_world()
{
    for (int i = 0 ; i < TILES_NUM ; i++)
    {
        draw_ent(&tiles[i]);
    }
}
void draw_jetpack_meter()
{
    DrawTextureEx(jetpack_meter_texture, (Vector2) {15,15}, 0.0f, GAME_SCALE, WHITE);

    // fuel color
    Color *fuel_clr = &FUEL_COLOR_GREEN;
    if (player_fuel < FUEL_MAX/4)
        fuel_clr = &FUEL_COLOR_RED;
    else if (player_fuel < FUEL_MAX/2)
        fuel_clr = &FUEL_COLOR_YELLOW;

    int fuel_meter_height = 95;

    // fuel meter
    DrawRectangle(27, 124  - ((double)player_fuel/FUEL_MAX) * fuel_meter_height, 18, ((double)player_fuel/FUEL_MAX) * fuel_meter_height, *fuel_clr);
}
void apply_gravity(m_ent *entity)
{
    entity->base.rect.y += GRAVITY;
}
void update_m_ent(m_ent *e)
{
    e->right_collision_rect.x = e->base.rect.x + 13;
    e->right_collision_rect.y = e->base.rect.y + 3;

    e->left_collision_rect.x = e->base.rect.x + -1;
    e->left_collision_rect.y = e->base.rect.y + 3;

    e->down_collision_rect.x = e->base.rect.x + 1;
    e->down_collision_rect.y = e->base.rect.y + 13;

    e->on_ground_collision_rect.x = e->base.rect.x + 1;
    e->on_ground_collision_rect.y = e->base.rect.y + 15;

    e->top_collision_rect.x = e->base.rect.x + 1;
    e->top_collision_rect.y = e->base.rect.y + 1;
}
void destroy_ent(ent *entity)
{
    entity->rect.x = -999;
}
void touch_flag(ent *flag)
{
    game_win = true;
    game_over = true;
    ply.base.rect.x = 1000;
    PlaySound(win_sound);
}
void touch_fuel(ent *fuel)
{
    fuel->rect.x = -999;
    player_fuel = FUEL_MAX;
    PlaySound(pickup_fuel_sound);
}
void pickup_coin(ent *coin)
{
    coins++;
    create_coin_pickup_text((int)coin->rect.x, (int)coin->rect.y);
    PlaySound(pickup_coin_sound);
}
void pickup_key(ent *key)
{
    key->rect.x = -999;
    PlaySound(pickup_key_sound);
}
void kill_player()
{
    ply.alive = false;
    PlaySound(dead_sound);
}
void handle_ent_collision(short *ent_ids[4])
{
    for (int i = 0 ; i < 4 ; i++)
    {
        ent *tile = &tiles[*ent_ids[i]];

        switch (tile->type_id)
        {
            case ENT_COIN:
                pickup_coin(tile);
                destroy_ent(tile);
            break;
            case ENT_KEY:
                if (!key_collected)
                {
                    key_collected = true;
                    pickup_key(tile);
                    destroy_ent(tile);
                }
            break;
            case ENT_DOOR:
                if (key_collected)
                {
                    destroy_ent(tile);
                    key_collected = false;
                    PlaySound(door_open_sound);
                }
            break;
            case ENT_FLAG:
                touch_flag(tile);
            break;
            case ENT_FUEL:
                touch_fuel(tile);
            break;
            case ENT_SPIKES:
                kill_player();
            break;
        }

    }
}

void apply_m_ent_collision(m_ent *e)
{
    bool colliding_with_on_ground = false;

    short floor_collision_tile = -1;
    short right_collision_tile = -1;
    short left_collision_tile = -1;
    short top_collision_tile = -1;

    for (int i = 0 ; i < TILES_NUM ; i++)
    {
        Rectangle *tile_rect = &tiles[i].rect;
        if (floor_collision_tile == -1 && CheckCollisionRecs(e->down_collision_rect, *tile_rect))   floor_collision_tile = i;
        if (left_collision_tile == -1 && CheckCollisionRecs(e->left_collision_rect, *tile_rect))    left_collision_tile = i;
        if (right_collision_tile == -1 && CheckCollisionRecs(e->right_collision_rect, *tile_rect))  right_collision_tile = i;
        if (top_collision_tile == -1 && CheckCollisionRecs(e->top_collision_rect, *tile_rect))      top_collision_tile = i;
        // not on ground when the tile is a coin
        if (!colliding_with_on_ground) colliding_with_on_ground = CheckCollisionRecs(e->on_ground_collision_rect, *tile_rect) && tiles[i].type_id != ENT_COIN;     }

    bool colliding_with_floor   = (floor_collision_tile > -1);
    bool colliding_with_left    = (left_collision_tile  > -1);
    bool colliding_with_right   = (right_collision_tile > -1);
    bool colliding_with_top     = (top_collision_tile   > -1);

    short *colliding_ent_ids[4] = {
        &left_collision_tile,
        &right_collision_tile,
        &top_collision_tile,
        &floor_collision_tile,
    };

    handle_ent_collision(colliding_ent_ids);

    // Can only start to fall, when was on ground
    if (e->on_ground) e->on_ground = colliding_with_on_ground;

    if (!colliding_with_floor && !e->on_ground)
    {
        // Falling ->
        apply_gravity(e);
    }
    else
    {
        // Not Falling ->
        e->on_ground = true;

        while (colliding_with_floor)
        {
            e->base.rect.y -= 0.01;
            update_m_ent(e);
            colliding_with_floor = CheckCollisionRecs(e->down_collision_rect,tiles[floor_collision_tile].rect);
        }
    }

    e->base.rect.x += colliding_with_left ? PLAYER_SPEED : 0;
    e->base.rect.x -= colliding_with_right ? PLAYER_SPEED : 0;
    e->base.rect.y += colliding_with_top ? PLAYER_SPEED -0.2 : 0;
}

Sound load_sound(const unsigned char *file_data, unsigned int data_size)
{
    Wave wav = LoadWaveFromMemory(".wav", file_data, data_size);
    Sound snd = LoadSoundFromWave(wav);
    UnloadWave(wav);
    return snd;
}

// Load a texture from memory and replace MAGENTA with BLANK
Texture2D load_texture(const unsigned char *file_data, unsigned int data_size)
{
    Image img = LoadImageFromMemory(".png",file_data,data_size);
    ImageColorReplace(&img, MAGENTA, BLANK);
    Texture2D tex =  LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}

void draw_lose()
{
}

void draw_win()
{
    if ((current_level+1) < LEVEL_MAX)
    {
        strcpy(win_text,"");
        sprintf(win_text,"LEVEL %d COMPLETED",current_level+1);
        draw_text_big(win_text, 220, 120);
        draw_text_big("PRESS ENTER TO CONTINUE", 220, 230);
    }
    else
    {
        draw_text_big("ALL LEVELS COMPLETED", 220, 120);
        draw_text_big("YOU WIN!", 300, 230);
    }
}

void draw_restart()
{
    draw_text_big("YOU DIED", 220,120);
    draw_text_big("PRESS R TO RESTART!", 230, 140);
}

void draw_coin_counter()
{
    DrawTextureEx(coin_texture, (Vector2) {10,260}, 0.0f, GAME_SCALE, WHITE);
    strcpy(coin_text,"");
    sprintf(coin_text,"x%d",coins);
    draw_text_big(coin_text, 50, 280);
}

void draw_spinning_key()
{
    DrawTexturePro
        (
            key_texture,
            (Rectangle) {0,0,key_texture.width,key_texture.height},
            (Rectangle) {35,240,key_texture.width*2,key_texture.height*2},
            (Vector2) {key_texture.width,key_texture.height} ,
            game_tick,
            WHITE
        );
}
void update_coin_pickup_text()
{
    for( int i = 0 ; i < 10 ; i++)
    {
        coin_add_text *ctext = &coin_add_texts[i];
        if (ctext->alive) 
        {
            ctext->lifetime++;
            if (ctext->lifetime % 2 == 0) ctext->pos.y-=1;
            if (ctext->lifetime > 30) ctext->alive = false;
        }
    }
}

void draw_coin_pickup_text()
{
    for( int i = 0 ; i < 10 ; i++)
    {
        coin_add_text *ctext = &coin_add_texts[i];
        if (ctext->alive)
        {
            draw_text("+1", ctext->pos.x - 10, ctext->pos.y, 8, YELLOW);
        }
    }
}

void update_player()
{
    // decide current texture
    if (!ply.alive)
        ply.base.img = &player_falling_texture;
    else if (!ply.inverted)
        if (ply.on_ground)
            if (ply.walking_time > 0)
                ply.base.img = &player_texture_anim;
            else
                ply.base.img = &player_texture;
        else
            ply.base.img = &player_falling_texture;
    else
        if (ply.on_ground)
            if (ply.walking_time > 0)
                ply.base.img = &player_texture_anim_invert;
            else
                ply.base.img = &player_invert_texture;
        else
            ply.base.img = &player_falling_invert_texture;

    player_frametime+=12;
    if (player_frametime > 298) player_frametime = 1;
    player_frame = player_frametime / 100;
    ply.base.frame = player_frame;
}

void draw_player_kill(Texture2D *texture, Rectangle *rect, float rotation)
{
    DrawTextureEx(*texture, (Vector2) {rect->x,rect->y+25}, rotation, 1.0f, WHITE);
}

void draw_player()
{
    if (ply.alive)
        draw_ent(&ply.base);
    else
        draw_player_kill(ply.base.img,&ply.base.rect,-90.0f);
}

void draw_game(Camera2D *camera, int game_tick)
{
    BeginDrawing();
        ClearBackground(background_color);

        BeginMode2D(*camera);

            draw_world();
            draw_player();
            draw_jetpack_particles();
            draw_coin_pickup_text();

            #ifdef DEBUG_PLAYER_COLLISIONS
                Color debug_clr = (Color) {255,0,255,100};
                DrawRectangleRec(ply.left_collision_rect, debug_clr);
                DrawRectangleRec(ply.right_collision_rect, debug_clr);
                DrawRectangleRec(ply.down_collision_rect, debug_clr);
                DrawRectangleRec(ply.top_collision_rect, debug_clr);
                DrawRectangleRec(ply.on_ground_collision_rect, debug_clr);
            #endif

        EndMode2D();

        if (!game_over)
        {
            draw_jetpack_meter();
            draw_coin_counter();
        }

        if (key_collected)                  draw_spinning_key();
        if (game_over && game_win)          draw_win();
        else if (!ply.alive)                draw_restart();
        if (game_tick < GAME_START_TICK)    ClearBackground(BLUE);

    EndDrawing();
}

void update_game()
{
    // Wait for the rendering to load before starting the game
    if (game_tick > GAME_START_TICK)
    {
        camera.target = (Vector2) {ply.base.rect.x,ply.base.rect.y};
        update_player();
        update_m_ent(&ply);
        update_jetpack_particles();
        apply_m_ent_collision(&ply);
        update_coin_pickup_text();
    }
}

void load_resources()
{
    // load textures
    player_texture                = load_texture(res_player_base_png,           res_player_base_png_len);
    player_invert_texture         = load_texture(res_player_base_invert_png,    res_player_base_invert_png_len);
    player_falling_texture        = load_texture(res_player_falling_png,        res_player_falling_png_len);
    player_falling_invert_texture = load_texture(res_player_falling_invert_png, res_player_falling_invert_png_len);
    map_tile_texture              = load_texture(res_map_tile_png,              res_map_tile_png_len);
    jetpack_particle_texture      = load_texture(res_jetpack_particle_png,      res_jetpack_particle_png_len);
    jetpack_meter_texture         = load_texture(res_jetpack_meter_png,         res_jetpack_meter_png_len);
    coin_texture                  = load_texture(res_coin_png,                  res_coin_png_len);
    door_texture                  = load_texture(res_door_png,                  res_door_png_len);
    key_texture                   = load_texture(res_key_png,                   res_key_png_len);
    spikes_texture                = load_texture(res_spikes_png,                res_spikes_png_len);
    flag_texture                  = load_texture(res_flag_png,                  res_flag_png_len);
    fuel_texture                  = load_texture(res_fuel_png,                  res_fuel_png_len);
    none_texture                  = load_texture(res_none_png,                  res_none_png_len);
    player_texture_anim           = load_texture(res_player_png,                res_player_png_len);
    player_texture_anim_invert    = load_texture(res_player_invert_png,         res_player_invert_png_len);

    // load font
    minecraft_font = (Font) LoadFontFromMemory(".ttf", res_Minecraft_ttf, res_Minecraft_ttf_len, 32, NULL, 0);

    // load sounds
    step_sound                    = load_sound(res_sfxr_step_wav,               res_sfxr_step_wav_len);
    jetpack_sound                 = load_sound(res_sfxr_jetpack_wav,            res_sfxr_jetpack_wav_len);
    door_open_sound               = load_sound(res_sfxr_door_open_wav,          res_sfxr_door_open_wav_len);
    win_sound                     = load_sound(res_sfxr_win_wav,                res_sfxr_win_wav_len);
    pickup_coin_sound             = load_sound(res_sfxr_pickup_coin_wav,        res_sfxr_pickup_coin_wav_len);
    pickup_key_sound              = load_sound(res_sfxr_pickup_key_wav,        res_sfxr_pickup_key_wav_len);
    pickup_fuel_sound             = load_sound(res_sfxr_pickup_fuel_wav,        res_sfxr_pickup_fuel_wav_len);
    dead_sound                    = load_sound(res_sfxr_dead_wav,               res_sfxr_dead_wav_len);
}

void unload_resources()
{
    UnloadTexture(player_texture);
    UnloadTexture(player_invert_texture);
    UnloadTexture(player_falling_texture);
    UnloadTexture(player_falling_invert_texture);
    UnloadTexture(player_texture_anim);
    UnloadTexture(player_texture_anim_invert);
    UnloadTexture(map_tile_texture);
    UnloadTexture(jetpack_particle_texture);
    UnloadTexture(jetpack_meter_texture);
    UnloadTexture(coin_texture);
    UnloadTexture(door_texture);
    UnloadTexture(key_texture);
    UnloadTexture(spikes_texture);
    UnloadTexture(flag_texture);
    UnloadTexture(none_texture);

    UnloadSound(pickup_coin_sound);
    UnloadSound(win_sound);
    UnloadSound(door_open_sound);
    UnloadSound(step_sound);
    UnloadSound(jetpack_sound);
    UnloadSound(dead_sound);
}

void setup_camera(Camera2D *cam)
{
    *cam = (Camera2D) { 0 };
    cam->target = (Vector2) {ply.base.rect.x,ply.base.rect.y};
    cam->offset = (Vector2) {SCREEN_WIDTH/2,SCREEN_HEIGHT/2};
    cam->rotation = 0.0f;
    cam->zoom = GAME_SCALE;
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Jetpack Jumper");
    InitAudioDevice();

    SetTargetFPS(60);

    // Resources are loaded from memory
    load_resources();

    ply = (m_ent) {
        (ent) {&player_texture, {20.0f,20.0f,TILE_WIDTH,TILE_WIDTH}},
        {20.0f,20.0f,2,10}, // right
        {20.0f,20.0f,2,10}, // left
        {20.0f,20.0f,12,2}, // down
        {20.0f,20.0f,12,2}, // top
        {20.0f,20.0f,12,2}, // on_ground
        false,  // invert
        false,  // on ground
        0,      // walking time
        true    // alive
    };

    current_level = -1;
    next_level();

    setup_camera(&camera);

    while (!WindowShouldClose())
    {
        game_tick++;

        register_input();
        update_game();
        draw_game(&camera, game_tick);
    }

    unload_resources();

    CloseWindow();
    return 0;
}
