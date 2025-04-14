#include "raylib.h"
#include <stdio.h>
#include <string.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 320

#define GRAVITY 1
#define PLAYER_SPEED 1
#define JETPACK_SPEED 1.7
#define TILES_NUM 600
#define TILE_WIDTH 16

#define GAME_SCALE 3

//#define DEBUG_PLAYER_COLLISIONS

#define _ 0

// TYPES >>
typedef struct {
    Texture2D *img;
    Rectangle rect; // Base Rectangle
    int type_id;
} ent; //entity

typedef struct {
    ent base;
    Rectangle right_collision_rect; // Left Collision Detection
    Rectangle left_collision_rect; // Right Collision Detection
    Rectangle down_collision_rect; // Bottom Collision Detection
    Rectangle top_collision_rect; // Bottom Collision Detection
    Rectangle on_ground_collision_rect;
    Texture2D *img_invert;
    Texture2D *falling_img;
    Texture2D *falling_img_invert;
    bool inverted;
    bool on_ground;
    int walking_time;
    bool alive;
} m_ent; //moving entity
         
typedef struct {
    ent base;
    int lifetime;
    bool alive;
} jetpack_particle;

float player_fuel = 99.9f;
int coins = 0;
bool key_collected = false;

m_ent ply;
ent counter_coin;

Font minecraft_font;

ent tiles[TILES_NUM];
jetpack_particle jetpack_particles[30];
int next_jetpack_particle_count = 0;

Color background_color = {100,160,255,255}; //sky blue
	
Texture2D player_texture                ;
Texture2D player_invert_texture         ;
Texture2D player_falling_texture        ;
Texture2D player_falling_invert_texture ;
Texture2D map_tile_texture              ;
Texture2D jetpack_particle_texture      ;
Texture2D jetpack_meter_texture         ;
Texture2D coin_texture                  ;
Texture2D door_texture                  ;
Texture2D key_texture                   ;
Texture2D spikes_texture                ;
Texture2D flag_texture                  ;
Texture2D none_texture                  ;





// TYPES <<
#define ENT_COIN 2
#define ENT_KEY 3
#define ENT_DOOR 4
#define ENT_SPIKES 5
#define ENT_FLAG 9

// Map Tile Ids:
// 1 == Wall
// 2 == Coin
// 3 == Key
// 4 == Door
// 7 == Elevator
// 9 == Flag
int level[] = { // 40 x 15
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
1,_,_,_,_,_,2,2,2,2,_,1,1,1,_,2,2,2,2,_,_,_,_,_,3,1,2,2,2,_,_,_,_,_,_,_,_,_,_,_,
1,_,_,_,_,_,_,_,_,_,_,1,1,1,_,2,2,2,2,_,_,_,1,1,1,1,2,2,2,_,_,_,_,_,_,9,_,_,_,_,
1,1,1,1,1,1,1,1,1,1,_,1,1,1,_,2,2,2,2,_,_,1,1,_,_,_,2,2,2,_,_,1,1,1,1,1,1,_,_,_,
_,1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,1,1,_,_,_,_,_,_,_,_,1,1,1,_,_,_,_,_,_,_,
_,1,_,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,_,_,_,1,1,1,1,1,1,1,1,1,_,_,_,_,_,_,_,
_,1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,1,_,_,1,1,1,1,_,_,_,_,_,_,_,_,_,_,_,_,_,
_,1,1,1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,4,_,1,1,1,1,1,_,_,_,_,_,_,_,_,_,_,_,_,_,
_,_,_,1,_,_,_,_,_,_,_,_,_,_,_,1,1,1,_,1,1,1,1,1,1,1,1,_,_,_,_,_,_,_,_,_,_,_,_,_,
_,_,_,1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
_,_,_,1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
_,_,_,1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
_,_,_,1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
_,_,_,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
};

void player_jump()
{

}

void register_input()
{
    if (!ply.alive) return;

    if (IsKeyDown(KEY_RIGHT)) {
        ply.base.rect.x += 
            (ply.walking_time > 15)
            ? PLAYER_SPEED
            : PLAYER_SPEED - (PLAYER_SPEED/2);

        ply.inverted = false;
        ply.walking_time++;
    }
    else if (IsKeyDown(KEY_LEFT)) {
        ply.base.rect.x -= 
            (ply.walking_time > 15) 
            ? PLAYER_SPEED 
            : PLAYER_SPEED - (PLAYER_SPEED/2);

        ply.inverted = true;
        ply.walking_time++;
    }
    else {
        ply.walking_time = 0;
    }

    if (IsKeyDown(KEY_UP)) {
        if (player_fuel > 0) {
            ply.base.rect.y -= JETPACK_SPEED;
            player_fuel -= 1.0f;
            next_jetpack_particle_count++;
            ply.on_ground = false;
        }
    } else {
        if (player_fuel < 99.9f && ply.on_ground) {
            player_fuel += 0.80f;
        }
    }
}

// Load the level into tiles
void create_world()
{
    int x = 0;
    int y = 0;
    int spacing = TILE_WIDTH;
    for(int i = 0 ; i < TILES_NUM ; i++)
    {
        ent *tile = &tiles[i];

        int type = level[x + (y * 40)];

        tile->img = &none_texture;
        tile->rect.x = x * spacing;
        tile->rect.y = y * spacing;
        tile->rect.width = TILE_WIDTH;
        tile->rect.height = TILE_WIDTH;
        tile->type_id = type;

        if (type == 0) {
            tile->rect.x = -999;
            tile->rect.y = -999;
        }
        if (type == 1) tile->img = &map_tile_texture;
        if (type == 2) tile->img = &coin_texture;
        if (type == 3) tile->img = &key_texture;
        if (type == 4) tile->img = &door_texture;
        if (type == 5) tile->img = &spikes_texture;
        if (type == 9) tile->img = &flag_texture;

        if (x >= 40)
        {
            x = 0;
            y++;
        }
        else
        {
            x++;
        }
    }
}

void initialize_jetpack()
{
    for (int i = 0 ; i < 30 ; i++)
    {
        jetpack_particle *jp = &jetpack_particles[i];

        jp->base.rect.x = -999;
        jp->base.rect.y = -999;
        jp->base.rect.width = 16;
        jp->base.rect.height = 16;
        jp->base.img = &jetpack_particle_texture;
    }
}


void draw_player_kill_texture(Texture2D *texture, Rectangle *rect, float rotation)
{
    DrawTextureEx(
            *texture, 
            (Vector2) {rect->x, rect->y + 25},
            rotation,
            1.0f,
            WHITE);
}
void draw_texture_scaled(Texture2D *texture, Rectangle *rect, float scale)
{
    DrawTextureEx(
            *texture, 
            (Vector2) {rect->x, rect->y},
            0.0f,
            scale,
            WHITE);
}
void draw_texture(Texture2D *texture, Rectangle *rect)
{
    DrawTextureEx(
            *texture, 
            (Vector2) {rect->x, rect->y},
            0.0f,
            1.0f,
            WHITE);
}
void draw_jetpack_particles()
{
    for (int i = 0 ; i < 30 ; i++)
    {
        jetpack_particle *jp = &jetpack_particles[i];
        draw_texture(jp->base.img,&jp->base.rect);
    }
}

void create_jetpack_particle(jetpack_particle *jp)
{
    jp->alive = true;
    jp->lifetime = 0;
    jp->base.rect.x = ply.base.rect.x + ( (ply.inverted) ? 4 : -4  );
    jp->base.rect.y = ply.base.rect.y + 6;
}

void destroy_jetpack_particle(jetpack_particle *jp)
{
    jp->alive = false;
    jp->base.rect.x = -999;
    jp->base.rect.y = -999;
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

            if (jp->lifetime > 20) {
                destroy_jetpack_particle(jp);
            }
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
        draw_texture(tiles[i].img, &tiles[i].rect);
    }
}

void draw_jetpack_meter()
{
    Rectangle jetpack_meter_rect = {15, 15, 16, 48};
    draw_texture_scaled(&jetpack_meter_texture, &jetpack_meter_rect, GAME_SCALE);

    // fuel color
    Color fuel_clr = (Color) {50,250,50,255};
    if (player_fuel < 30) {
        fuel_clr = (Color) {250,50,50,255};
    } else if (player_fuel < 60) {
        fuel_clr = (Color) {250,250,50,255};
    }

    int fuel_meter_height = 95;

    // fuel meter
    DrawRectangle(
            30,
            146  - (player_fuel/100) * fuel_meter_height,
            18,
            (player_fuel/100) * fuel_meter_height, fuel_clr);
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
    entity->rect.x = -999;
}

void touch_flag(ent *flag)
{
    ply.base.rect.x = 1000;
}

void open_door(ent *door)
{
}

void pickup_coin(ent *coin)
{
    coins++;
}

void pickup_key(ent *key)
{
    key->rect.x = -999;
    key->rect.y = -999;   
}

void kill_player()
{
    ply.alive = false;
}

void handle_ent_collision(int *ent_ids[4])
{
    for (int i = 0 ; i < 4 ; i++)
    {
        ent *tile = &tiles[*ent_ids[i]];
        if      (tile->type_id == ENT_COIN)
        {
            pickup_coin(tile);
            destroy_ent(tile);
        }
        else if (tile->type_id == ENT_KEY)
        {
            key_collected = true;
            pickup_key(tile);
            destroy_ent(tile);
        } 
        else if (tile->type_id == ENT_DOOR)
        {
            if (key_collected)
            {
                open_door(tile);
                destroy_ent(tile);
            }
        }
        else if (tile->type_id == ENT_FLAG)
        {
            touch_flag(tile);
        }
        else if (tile->type_id == ENT_SPIKES)
        {
            kill_player();
        }
    }
}

void apply_m_ent_collision(m_ent *e)
{
    bool colliding_with_floor = false;
    bool colliding_with_left = false;
    bool colliding_with_right = false;
    bool colliding_with_top = false;
    bool colliding_with_on_ground = false;

    int floor_collision_tile = -1;
    int right_collision_tile = -1;
    int left_collision_tile = -1;
    int top_collision_tile = -1;

     for (int i = 0 ; i < TILES_NUM ; i++) {
        if (!colliding_with_floor) {
            colliding_with_floor = CheckCollisionRecs(e->down_collision_rect, tiles[i].rect);
            floor_collision_tile = i;
        }
        if (!colliding_with_left) {
            colliding_with_left = CheckCollisionRecs(e->left_collision_rect, tiles[i].rect);
            if (colliding_with_left) left_collision_tile = i;
        }
        if (!colliding_with_right) {
            colliding_with_right = CheckCollisionRecs(e->right_collision_rect, tiles[i].rect);
            if (colliding_with_right) right_collision_tile = i;
        }
        if (!colliding_with_top) {
            colliding_with_top = CheckCollisionRecs(e->top_collision_rect, tiles[i].rect);
            if (colliding_with_top) top_collision_tile = i;
        }
        if (!colliding_with_on_ground) {
            colliding_with_on_ground = CheckCollisionRecs(
                    e->on_ground_collision_rect,
                    tiles[i].rect
            );
        }
    }

    int *colliding_ent_ids[4] = {
        &left_collision_tile,
        &right_collision_tile,
        &top_collision_tile,
        &floor_collision_tile
    };

    handle_ent_collision(colliding_ent_ids);

    // If on ground, start falling when no on_ground detector rect collision
    if (e->on_ground)
        e->on_ground = colliding_with_on_ground;   

    if (!colliding_with_floor && !e->on_ground)
    {
        // Falling ->
        //
        apply_gravity(e);
    } else    
    {
        // Not Falling ->
        //
        e->on_ground = true;

        while (colliding_with_floor) {
            e->base.rect.y -= 0.01;
            update_m_ent(e);

            colliding_with_floor = CheckCollisionRecs(
                    e->down_collision_rect,
                    tiles[floor_collision_tile].rect
            );
        }
    }
 
    if (e->walking_time > 15) {
        e->base.rect.x += colliding_with_left ? PLAYER_SPEED : 0;
        e->base.rect.x -= colliding_with_right ? PLAYER_SPEED : 0;
    } else {
        e->base.rect.x += colliding_with_left ? PLAYER_SPEED  : 0;
        e->base.rect.x -= colliding_with_right ? PLAYER_SPEED  : 0;
    }
    e->base.rect.y += colliding_with_top ? PLAYER_SPEED -0.2 : 0;

}
// Load a texture from a file and replace MAGENTA with BLANK
Texture2D load_texture(const char *path)
{
    Image img = LoadImage(path);
    ImageColorReplace(&img, MAGENTA, BLANK);
    Texture2D tex =  LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}

void draw_coin_counter()
{
        char coin_text[100];
        strcpy(coin_text,"");
        sprintf(coin_text,"x%d",coins);
        DrawTextEx(minecraft_font,coin_text, (Vector2) {50.0f,280.0f}, 16, 4,WHITE);
}

void draw_game(Camera2D *camera, int game_tick)
{
    BeginDrawing();
    ClearBackground(background_color);

    BeginMode2D(*camera);

    if (ply.alive)
    {
        if (!ply.inverted)
            draw_texture(
                    (ply.on_ground)
                    ? ply.base.img
                    : ply.falling_img,
                    &ply.base.rect
            );
        else
            draw_texture( 
                    (ply.on_ground) 
                    ? ply.img_invert 
                    : ply.falling_img_invert,
                    &ply.base.rect
            );
    } else {
        // draw player ded
        draw_player_kill_texture(ply.falling_img,&ply.base.rect,-90.0f);
    }
    
    draw_world();
    draw_jetpack_particles();

    #ifdef DEBUG_PLAYER_COLLISIONS   
        Color debug_clr = (Color) {255,0,255,100};
        DrawRectangleRec(ply.left_collision_rect, debug_clr);
        DrawRectangleRec(ply.right_collision_rect, debug_clr);
        DrawRectangleRec(ply.down_collision_rect, debug_clr);
        DrawRectangleRec(ply.top_collision_rect, debug_clr);
        DrawRectangleRec(ply.on_ground_collision_rect, debug_clr);
    #endif

    EndMode2D();

    draw_jetpack_meter();
    draw_texture_scaled(counter_coin.img, &counter_coin.rect,3.0f);
    draw_coin_counter();

    if (game_tick < 30)
    {
        ClearBackground(BLUE);
    }
    EndDrawing();
}


int main(void)
{
    // INIT
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "mygame");
	SetTargetFPS(60);
	
    player_texture                = load_texture("res/player_base.png");
    player_invert_texture         = load_texture("res/player_base_invert.png");
    player_falling_texture        = load_texture("res/player_falling.png");
    player_falling_invert_texture = load_texture("res/player_falling_invert.png");
    map_tile_texture              = load_texture("res/map_tile.png");
    jetpack_particle_texture      = load_texture("res/jetpack_particle.png");
    jetpack_meter_texture         = load_texture("res/jetpack_meter.png");
    coin_texture                  = load_texture("res/coin.png");
    door_texture                  = load_texture("res/door.png");
    key_texture                   = load_texture("res/key.png");
    spikes_texture                = load_texture("res/spikes.png");
    flag_texture                  = load_texture("res/flag.png");
    none_texture                  = load_texture("res/none.png");

    minecraft_font = (Font) LoadFont("res/Minecraft.ttf");

    ply = (m_ent) { 
        (ent) {&player_texture, {20.0f,20.0f,TILE_WIDTH,TILE_WIDTH}},
        {20.0f,20.0f,2,10}, // right
        {20.0f,20.0f,2,10}, // left
        {20.0f,20.0f,12,2}, // down
        {20.0f,20.0f,12,2}, // top
        {20.0f,20.0f,12,2},  // on_ground
        &player_invert_texture,
        &player_falling_texture,
        &player_falling_invert_texture,
        false,
        false,
        0,
        true
    };

    create_world(&map_tile_texture);
    initialize_jetpack(&jetpack_particle_texture);
    counter_coin = (ent) {&coin_texture, {10,260,16,16}};
    
    Camera2D camera = { 0 };
    camera.target = (Vector2){ply.base.rect.x,ply.base.rect.y};
    camera.offset = (Vector2){SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
    camera.rotation = 0.0f;
    camera.zoom = GAME_SCALE;

    int game_tick = 0;

	while (!WindowShouldClose())
	{
        game_tick++;

        // INPUT
        register_input();

        // LOGIC
        camera.target = (Vector2){ply.base.rect.x,ply.base.rect.y};

        // Wait for the rendering to load before starting the game
        if (game_tick > 30)
        {
            // Update Everything
            update_m_ent(&ply);
            update_jetpack_particles();
            apply_m_ent_collision(&ply);
        }
        // RENDER
        draw_game(&camera, game_tick);
	}

    UnloadTexture(player_texture);
    UnloadTexture(player_invert_texture);
    UnloadTexture(player_falling_texture);
    UnloadTexture(player_falling_invert_texture);
    UnloadTexture(map_tile_texture);
    UnloadTexture(jetpack_particle_texture);
    UnloadTexture(jetpack_meter_texture);
    UnloadTexture(coin_texture);
    UnloadTexture(door_texture);
    UnloadTexture(key_texture);
    UnloadTexture(spikes_texture);
    UnloadTexture(flag_texture);
    UnloadTexture(none_texture);

    CloseWindow();
	return 0;
}
