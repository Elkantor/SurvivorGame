#include "includes.c"

static Arena mainArena;

int main(int _argc, char** _argv)
{
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT);
    InitWindow(1280, 720, "SurvivorBuilder");
    SetTargetFPS(60);

    Game* game = ArenaPush(&mainArena, sizeof(Game));
    GameInit(game);

    while (WindowShouldClose() == false)
    {
        const f32 deltaTime = 1.f/60.f;
        GameUpdate(game, deltaTime);
        GameRender(game);
    }

    ArenaPop(&mainArena, game);
    CloseWindow();
    return 0;
}