#include "raylib.h"
#include <stdio.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 320

#define GRAVITY 1
#define PLAYER_SPEED 1
#define JETPACK_SPEED 1.75f
#define TILES_NUM 600

#define GAME_SCALE 3

//#define DEBUG_PLAYER_COLLISIONS

#define _ 0

// TYPES >>
typedef struct {
    Texture2D *img;
    Rectangle rect; // Base Rectangle
} ent;
typedef struct {
    ent base;
    Rectangle right_collision_rect; // Left Collision Detection
    Rectangle left_collision_rect; // Right Collision Detection
    Rectangle down_collision_rect; // Bottom Collision Detection
    Rectangle top_collision_rect; // Bottom Collision Detection
    Texture2D *inverted_img;
    bool inverted;
} m_ent;

float player_fuel = 99.9f;

// TYPES <<

// Map Tile Ids:
// 1 == Wall
// 2 == Coin
// 3 == Key
// 4 == Door
// 7 == Elevator
int world[] = { // 40 x 15
_,_,_,_,_,_,_,_,_,_,_,_,_,1,1,1,1,1,1,1,1,1,1,1,1,1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
_,_,_,_,_,_,_,_,_,_,_,_,_,1,_,_,_,_,_,_,_,_,_,_,3,1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
_,_,_,_,_,_,_,_,_,_,_,_,_,1,_,_,_,_,_,_,_,_,1,1,1,1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
_,1,1,1,1,1,1,1,1,_,_,1,1,1,_,2,2,2,2,_,_,1,1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
_,1,_,_,_,_,_,_,_,_,_,_,_,_,_,2,2,2,2,_,1,1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
_,1,_,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,_,_,_,1,1,1,1,1,1,1,1,1,_,_,_,_,_,_,_,
_,1,_,_,7,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,1,_,_,1,1,1,1,_,_,_,_,_,_,_,_,_,_,_,_,_,
_,1,1,1,7,1,1,1,1,1,1,_,_,_,_,_,_,_,_,_,4,_,1,1,1,1,1,_,_,_,_,_,_,_,_,_,_,_,_,_,
_,_,_,1,7,_,_,_,_,_,1,1,_,1,_,1,_,1,_,1,1,1,1,1,1,1,1,_,_,_,_,_,_,_,_,_,_,_,_,_,
_,_,_,1,7,_,_,_,_,_,1,_,_,_,_,_,_,_,_,1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
_,_,_,1,7,_,_,_,_,_,1,_,_,_,_,_,_,_,_,1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
_,_,_,1,7,_,_,_,_,_,1,_,_,_,_,_,_,_,_,1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
_,_,_,1,7,_,_,_,_,_,_,_,_,_,_,_,_,_,_,1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
_,_,_,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
};

void player_jump()
{

}

void register_input(m_ent *ply)
{
    if (IsKeyDown(KEY_RIGHT)) {
        ply->base.rect.x += PLAYER_SPEED;
        ply->inverted = false;
    }

    if (IsKeyDown(KEY_LEFT)) {
        ply->base.rect.x -= PLAYER_SPEED;
        ply->inverted = true;
    }

    if (IsKeyDown(KEY_UP)) {
        if (player_fuel > 0) {
            ply->base.rect.y -= JETPACK_SPEED;
            player_fuel -= 1.0f;
        }
    } else {
        if (player_fuel < 99.9f) {
            player_fuel += 0.25f;
        }
    }
}

void create_world(ent *tiles, Texture2D *map_tile_texture)
{
    int x = 0;
    int y = 1;
    int spacing = 16;
    for(int i = 0 ; i < TILES_NUM ; i++)
    {
        x++;

        tiles[i].img = map_tile_texture;

        // Wall
        if (world[x + (y * 40)] == 1)
        {
            tiles[i].rect.x = x * spacing;
            tiles[i].rect.y = y * spacing;
            tiles[i].rect.width = 16;
            tiles[i].rect.height = 16;
        }
        // Anything else is not visible
        else {
            tiles[i].rect.x = -999*spacing;
            tiles[i].rect.y = -999*spacing;
        }

        if (x >= 40)
        {
            x = 0;
            y++;
        }
    }
}

void draw_ent(Texture2D *texture, Rectangle *rect)
{
    DrawTextureEx(
            *texture, 
            (Vector2) {rect->x, rect->y},
            0.0f,
            1.0f,
            WHITE);
}

void draw_world(ent *tiles)
{
    for (int i = 0 ; i < TILES_NUM ; i++) 
    {
        draw_ent(tiles[i].img, &tiles[i].rect);
    }
}

void draw_jetpack_meter()
{
    DrawRectangle(5, 100, 20, 100, (Color) {255,255,255,255});
    DrawRectangle(5 + 3, 105, 14, 90, (Color) {50,50,50,255});

    DrawRectangle(5 + 4, 106, 12, 88, (Color) {50,50,50,255});
    // fuel
    DrawRectangle(5 + 4,
            195 - (player_fuel/100) * 88,
            12,
            (player_fuel/100) * 88, (Color) {250,50,50,255});
}

void apply_gravity(m_ent *entity, bool neg)
{
    entity->base.rect.y += (neg) ? -GRAVITY/2 : GRAVITY;
}

void apply_m_ent_collision(m_ent *e, ent *tiles)
{
    bool colliding_with_floor = false;
    bool colliding_with_left = false;
    bool colliding_with_right = false;
    bool colliding_with_top = false;

     for (int i = 0 ; i < TILES_NUM ; i++) {
        if (!colliding_with_floor) {
            colliding_with_floor = CheckCollisionRecs(e->down_collision_rect, tiles[i].rect);
        }
        if (!colliding_with_left) {
            colliding_with_left = CheckCollisionRecs(e->left_collision_rect, tiles[i].rect);
        }
        if (!colliding_with_right) {
            colliding_with_right = CheckCollisionRecs(e->right_collision_rect, tiles[i].rect);
        }
        if (!colliding_with_top) {
            colliding_with_top = CheckCollisionRecs(e->top_collision_rect, tiles[i].rect);
        }
    }

    if (colliding_with_floor) {
        colliding_with_top = false;
    }

    apply_gravity(e, colliding_with_floor);
    e->base.rect.x += colliding_with_left ? PLAYER_SPEED : 0;
    e->base.rect.x -= colliding_with_right ? PLAYER_SPEED : 0;
    e->base.rect.y += colliding_with_top ? JETPACK_SPEED*0.5 : 0;
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

void update_m_ent(m_ent *e, ent *tiles)
{
    e->right_collision_rect.x = e->base.rect.x + 12;
    e->right_collision_rect.y = e->base.rect.y + 3;

    e->left_collision_rect.x = e->base.rect.x + 0;
    e->left_collision_rect.y = e->base.rect.y + 3;

    e->down_collision_rect.x = e->base.rect.x + 1;
    e->down_collision_rect.y = e->base.rect.y + 14;

    e->top_collision_rect.x = e->base.rect.x + 1;
    e->top_collision_rect.y = e->base.rect.y + 1;



}

int main(void)
{
    // INIT >>
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "mygame");
	SetTargetFPS(60);
    // INIT <<
	
    Texture2D player_texture = load_texture("res/player_base.png");
    Texture2D player_invert_texture = load_texture("res/player_base_invert.png");
    Texture2D map_tile_texture = load_texture("res/map_tile.png");

    m_ent ply = { 
        (ent) {&player_texture, {20.0f,20.0f,16,16}},
        {20.0f,20.0f,2,10},
        {20.0f,20.0f,2,10},
        {20.0f,20.0f,12,2},
        {20.0f,20.0f,12,2},
        &player_invert_texture,
        false
    };

    ent tiles[TILES_NUM];

    Color background_color = {115,155,255,255}; //sky blue

    create_world(tiles, &map_tile_texture);
    
    Camera2D camera = { 0 };
    camera.target = (Vector2){ply.base.rect.x,ply.base.rect.y};
    camera.offset = (Vector2){SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f};
    camera.rotation = 0.0f;
    camera.zoom = GAME_SCALE;

	while (!WindowShouldClose())
	{
        // INPUT >>
        register_input(&ply);
        // INPUT <<
        //
        //
        // GAME LOGIC >>

        camera.target = (Vector2){ply.base.rect.x,ply.base.rect.y};

        update_m_ent(&ply, tiles);
        apply_m_ent_collision(&ply, tiles);

        // GAME LOGIC <<
        //
        //
        // RENDER >>
        BeginDrawing();
        BeginMode2D(camera);
        ClearBackground(background_color);

        if (!ply.inverted)
            draw_ent(ply.base.img, &ply.base.rect);
        else
            draw_ent(ply.inverted_img, &ply.base.rect);

        draw_world(tiles);
 
        #ifdef DEBUG_PLAYER_COLLISIONS   
            Color debug_clr = (Color) {255,0,255,200};
            DrawRectangleRec(ply.left_collision_rect, debug_clr);
            DrawRectangleRec(ply.right_collision_rect, debug_clr);
            DrawRectangleRec(ply.down_collision_rect, debug_clr);
            DrawRectangleRec(ply.top_collision_rect, debug_clr);
        #endif

        EndMode2D();

        draw_jetpack_meter();


        EndDrawing();
        // RENDER <<
	}

    UnloadTexture(player_texture);
    UnloadTexture(player_invert_texture);
    UnloadTexture(map_tile_texture);

    CloseWindow();
	return 0;
}
