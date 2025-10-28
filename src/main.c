#include "includes.c"

static Arena mainArena;

int main(int _argc, char** _argv)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(1280, 720, "SurvivorBuilder");
    SetTargetFPS(60);

    Game* game = ArenaPush(&mainArena, sizeof(Game));
    GameInit(game);

    while (WindowShouldClose() == false)
    {
        const f32 deltaTime = GetFrameTime();
        GameUpdate(game, deltaTime);
        GameRender(game);
    }

    CloseWindow();
    return 0;
}