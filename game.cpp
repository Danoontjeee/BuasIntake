#include "game.h"
#include "surface.h"
#include "template.h"
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include "iostream"
#define tileSize 64
#include "cmath"
#include <mutex>


std::once_flag onceFlag;


namespace Tmpl8
{
    
    
    Surface tiles("assets/tilemapReal.png");
    Sprite rose(new Surface("assets/rose.tga"), 3);
    Sprite slime(new Surface("assets/slime.tga"), 2);
    Sprite winText(new Surface("assets/wintext.png"), 1);
    Sprite explText(new Surface("assets/explanationtext.png"), 1);
    Sprite startText(new Surface("assets/startuptext.png"), 1);
    Sprite startTxt(new Surface("assets/starttext.png"), 1);
    Sprite loseText(new Surface("assets/losetext.png"), 1);
    Sprite roseCollect(new Surface("assets/rosecollected.tga"), 4);
    int startX = 150, startY = 170;
    int px = startX, py = startY;
    int lives = 3;
    int gameState = 0;


    void Game::Init() {}
    void Game::Shutdown() {}

    char level1[5][30] = {
        "aaXbaXbaXbcXbaXbcXbaXbaXbaXca",
        "daXad ad daXad daXbdZad ad da",
        "daXad ad bbXad faXeaXad ad bb",
        "bbXad ad ad ad ad ad ad ebYeb",
        "ebYebYebYebYebYebYebYebYdbYdb" };

    char level2[5][30] = {
        "aaXbaXbaXbaXbcXbaXbaXbaXbaXca",
        "daXad ad ad bbXad cbLebYad da",
        "fcXeaXacYad ad ad ad dbYbdZda",
        "daXad ad ad fbXad ad dbYad da",
        "faXbaXbaXbaXecXbaXbaXbaXbaXab" };
    char level3[5][30] = {
        "aaXbaXbaXbcXbaXbaXbaXbaXbaXca",
        "daXad ad daXad ad ad ad ad da",
        "daXad ad bbXad fbXad fbXad da",
        "bbXad ad ad ad daXad daXbdZda",
        "ebYebYebYebYccXecXbaXecXbaXab"
    };
    char map[5][30] = {};
    

    void DrawTile(int tx, int ty, Surface* screen, int x, int y)
    {
        Pixel* src = tiles.GetBuffer() + tx * 64 + (ty * 64) * tiles.GetWidth();
        Pixel* dst = screen->GetBuffer() + x + y * screen->GetWidth();

        for (int i = 0; i < tileSize; i++, src += tiles.GetWidth(), dst += screen->GetWidth())
            for (int j = 0; j < tileSize; j++)
                dst[j] = src[j];
    }
    //checks collision -> X
    bool checkPos(int x, int y) {
        int tx = x / tileSize, ty = y / tileSize;
        return map[ty][tx * 3 + 2] != 'X';
    }
    //checks teleport -> L
    bool checkTeleport(int x, int y){
        int tx = x / tileSize, ty = y / tileSize;
        return map[ty][tx * 3 + 2 ] == 'L';
    }
    //checks death -> Y
    bool checkDeath(int x, int y) {
        int tx = x / tileSize, ty = y / tileSize;
        return map[ty][tx * 3 + 2] != 'Y';
    }
    //checks finish -> Z
    bool checkFinish(int x, int y) {
        int tx = x / tileSize, ty = y / tileSize;
        return map[ty][tx * 3 + 2] == 'Z';
    }
    
    //checks distance 2 coordinates
    int distance(int x1, int y1, int x2, int y2) {
        return (sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)));
    }
    
    bool gameStart = true;
    bool roseCollected = false;
    bool firstTime = true;
    bool gameOn = false;
    int roseFrame = 0;
    int roseX = 0;
    int roseY = 0;
    int collected = 0;
    int nx = px, ny = py;
    

    void Game::Tick(float deltaTime)
    {
        
        screen->Clear((145 << 16) + (240 << 8) + (250));
        int xOfset = screen->GetWidth() / 2 - 320, yOfset = screen->GetHeight() / 2 - 160;

        // main menu
        if (gameState == 0) {
            gameStart = true;
            collected = 0;
            screen->Bar(screen->GetWidth() / 2 - 266, screen->GetHeight() / 3 - 138, screen->GetWidth() / 2 + 266, screen->GetHeight() / 3 + 138, (155 << 16) + (116 << 8) + 0);
            screen->Bar(screen->GetWidth() / 2 - 256, screen->GetHeight() / 3 - 128, screen->GetWidth() / 2 + 256, screen->GetHeight() / 3 + 128, (235 << 16) + (166 << 8) + 85);
            startText.Draw(screen, screen->GetWidth() / 2 - 256, screen->GetHeight() / 3 - 128);

            screen->Bar(screen->GetWidth() / 2 - 138, screen->GetHeight() / 4 * 3 - 42, screen->GetWidth() / 2 + 138, screen->GetHeight() / 4 * 3 + 42, (155 << 16) + (116 << 8) + 0);
            screen->Bar(screen->GetWidth() / 2 - 128, screen->GetHeight() / 4 * 3 - 32, screen->GetWidth() / 2 + 128, screen->GetHeight() / 4 * 3 + 32, (235 << 16) + (166 << 8) + 85);
            startTxt.Draw(screen, screen->GetWidth() / 2 - 128, screen->GetHeight() / 4 * 3 - 32);
            Sleep(150);
            if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
                gameState = 1;
                gameOn = true;
            }
        }
        // first level
        if (gameState == 1) {
            if (gameStart) {
                Sleep(200);
                px = startX;
                py = startY;
                roseCollected = false;
                roseX = 4 * tileSize + xOfset;
                roseY = 1 * tileSize + yOfset;
                gameStart = false;
                gameOn = true;
                lives = 3;
                for (int y = 0; y < 5; y++) {
                    for (int x = 0; x < 30; x++) {
                        map[y][x] = level1[y][x];
                    }
                }
            }

            for (int y = 0; y < 5; y++)
                for (int x = 0; x < 10; x++)
                {
                    int tx = map[y][x * 3] - 'a', ty = map[y][x * 3 + 1] - 'a';
                    DrawTile(tx, ty, screen, x * tileSize + xOfset, y * tileSize + yOfset);
                }

            screen->Bar(screen->GetWidth() / 2 - 138, screen->GetHeight() / 8 - 62, screen->GetWidth() / 2 + 138, screen->GetHeight() / 8 + 22, (155 << 16) + (116 << 8) + 0);
            screen->Bar(screen->GetWidth() / 2 - 128, screen->GetHeight() / 8 - 52, screen->GetWidth() / 2 + 128, screen->GetHeight() / 8 + 12, (235 << 16) + (166 << 8) + 85);
            explText.Draw(screen, screen->GetWidth() / 2 - 128, screen->GetHeight() / 8 - 52);


        }
        // second level
        if (gameState == 2) {
            {

                if (gameStart) {
                    Sleep(200);
                    px = startX;
                    py = startY;
                    roseCollected = false;
                    roseX = tileSize + xOfset;
                    roseY = 3 * tileSize + yOfset;
                    gameStart = false;
                    gameOn = true;
                    for (int y = 0; y < 5; y++) {
                        for (int x = 0; x < 30; x++) {
                            map[y][x] = level2[y][x];
                        }
                    }

                }


            }

        }
        // third level
        if (gameState == 3) {
            if (gameStart) {
                Sleep(200);
                px = startX;
                py = startY;
                roseCollected = false;
                roseX = 6 * tileSize + xOfset;
                roseY = 3 * tileSize + yOfset;
                gameStart = false;
                gameOn = true;
                for (int y = 0; y < 5; y++) {
                    for (int x = 0; x < 30; x++) {
                        map[y][x] = level3[y][x];
                    }
                }


            }




        }
        // win screen
        if (gameState == 4) {
            screen->Bar(screen->GetWidth() / 2 - 210, screen->GetHeight() / 2 - 74, screen->GetWidth() / 2 + 210, screen->GetHeight() / 2 + 74, (155 << 16) + (116 << 8) + 0);
            screen->Bar(screen->GetWidth() / 2 - 200, screen->GetHeight() / 2 - 64, screen->GetWidth() / 2 + 200, screen->GetHeight() / 2 + 64, (235 << 16) + (166 << 8) + 85);
            winText.Draw(screen, screen->GetWidth() / 2 - 200, screen->GetHeight() / 2 - 64);
            Sleep(10);
            if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
                gameState = 0;
            }
        }
        // lose screen
        if (gameState == 5) {
            screen->Bar(screen->GetWidth() / 2 - 210, screen->GetHeight() / 2 - 74, screen->GetWidth() / 2 + 210, screen->GetHeight() / 2 + 74, (155 << 16) + (116 << 8) + 0);
            screen->Bar(screen->GetWidth() / 2 - 200, screen->GetHeight() / 2 - 64, screen->GetWidth() / 2 + 200, screen->GetHeight() / 2 + 64, (235 << 16) + (166 << 8) + 85);
            loseText.Draw(screen, screen->GetWidth() / 2 - 200, screen->GetHeight() / 2 - 64);

            if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
                gameState = 0;
            }
        }

        // controls for the game
        if (gameOn) {
            int nx = px, ny = py;
            if (GetAsyncKeyState('D')) {
                nx++;
                slime.SetFrame(0);
            }
            if (GetAsyncKeyState('W')) {
                ny--;
            }
            if (GetAsyncKeyState('S')) {
                ny++;
            }
            if (GetAsyncKeyState('A')) {
                nx--;
                slime.SetFrame(1);
            }

            for (int y = 0; y < 5; y++)
                for (int x = 0; x < 10; x++)
                {
                    int tx = map[y][x * 3] - 'a', ty = map[y][x * 3 + 1] - 'a';
                    DrawTile(tx, ty, screen, x * tileSize + xOfset, y * tileSize + yOfset);
                }
            if (checkTeleport(nx - xOfset, ny - yOfset) && checkTeleport(nx - xOfset + slime.GetWidth(), ny - yOfset) && checkTeleport(nx - xOfset + slime.GetWidth(), ny + slime.GetHeight() - yOfset) && checkTeleport(nx - xOfset, ny + slime.GetHeight() - yOfset)) {
                nx += 2 * tileSize;

            }


            if (checkPos(nx - xOfset, ny - yOfset) && checkPos(nx - xOfset + slime.GetWidth(), ny - yOfset) && checkPos(nx - xOfset + slime.GetWidth(), ny + slime.GetHeight() - yOfset) && checkPos(nx - xOfset, ny + slime.GetHeight() - yOfset)) {
                px = nx, py = ny;

            }

            if (!(checkDeath(nx - xOfset, ny - yOfset) && checkDeath(nx - xOfset + slime.GetWidth(), ny - yOfset) && checkDeath(nx - xOfset + slime.GetWidth(), ny - yOfset + slime.GetHeight()) && checkDeath(nx - xOfset, ny - yOfset + slime.GetHeight()))) {

                lives -= 1;
                px = startX, py = startY;
                if (roseCollected) {
                    roseCollected = false;
                    collected -= 1;
                }
                
            }

            


            if (lives == 0) {
                gameState = 5;
                gameOn = false;
                collected = 0;
            }

            if (distance(roseX, roseY, px, py) > tileSize * 2) {
                rose.SetFrame(0);
            }
            if (distance(roseX, roseY, px, py) > tileSize && distance(roseX, roseY, px, py) < 2 * tileSize) {
                rose.SetFrame(1);
            }
            if (distance(roseX, roseY, px, py) < tileSize) {
                rose.SetFrame(2);
            }
            if (distance(roseX, roseY, px, py) < tileSize / 2) {
                if (!roseCollected) {
                   roseCollected = true;
                   std::cout << "Rose is collected";
                   collected += 1;
                } 
            }

            for (int i = 1; i < lives; i++) {
                slime.Draw(screen, screen->GetWidth() - i * 32 - 16, 16);
            }

            roseCollect.SetFrame(collected);
            roseCollect.Draw(screen, 0, screen->GetHeight() - roseCollect.GetHeight());

            slime.Draw(screen, px, py);
            if (!roseCollected) {
                rose.Draw(screen, roseX, roseY);
            }

            if ((checkFinish(nx - xOfset, ny - yOfset) && checkFinish(nx - xOfset + slime.GetWidth(), ny - yOfset) && checkFinish(nx - xOfset + slime.GetWidth(), ny + slime.GetHeight() - yOfset) && checkPos(nx - xOfset, ny + slime.GetHeight() - yOfset))) {
                if (roseCollected) {
                    if (gameState == 1 || gameState == 2) {
                        gameStart = true;
                    }
                    else { gameOn = false; }
                    gameState += 1;
                }
                else
                    std::cout << "Collect the rose first";
            }

        }
    }

};