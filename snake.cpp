#include "snake.hpp"

using namespace blit;

#define BLOCK 5
#define SCREEN_WIDTH 120 / BLOCK
#define SCREEN_HEIGHT 120 / BLOCK

short status = 0;
// 0 = start screen
// 1 = game
// 2 = end

short dT;
short pX[60], pY[60], pD, pL = 5;
short pDIR[5] = {-1,0,1,0,-1};
short aX, aY, aS;
short highscore = 0;
int freq = 0;

void new_apple()
{
    bool j;
    do
    {
        j = false;
        aX = std::rand() %SCREEN_WIDTH;
        aY = std::rand() %SCREEN_HEIGHT;
        short i;
        for (i = 0; i < pL; i++)
        {
            if (aX == pX[i] && aY == pY[i])
            {
                j = true;
                break;
            }
        }
    }
    while (j);
}

// init()
//
// setup your game here
void init() 
{
    set_screen_mode(ScreenMode::lores);

    if (read_save(highscore))
    {
    }
    else 
    {
        highscore = 0;
    }

    channels[0].waveforms = Waveform::SQUARE;
    channels[0].frequency = 0;
    channels[0].attack_ms = 10;
    channels[0].decay_ms = 400;
    channels[0].sustain = 0;
    channels[0].release_ms = 10;
}

// render(time)
//
// This function is called to perform rendering of the game. time is the 
// amount if milliseconds elapsed since the start of your game
void render(uint32_t time) 
{
    screen.clear();
    screen.alpha = 255;
    screen.mask = nullptr;
    if (status == 0)
    {
        screen.pen = Pen(0, 255, 0);
        screen.text("SNAKE", minimal_font, Point(SCREEN_WIDTH / 2 * BLOCK, 48), true, TextAlign::center_center);
        screen.pen = Pen(255, 255, 255);
        screen.text("PRESS A TO START", minimal_font, Point(SCREEN_WIDTH / 2 * BLOCK, 92), true, TextAlign::center_center);
    }
    else
    {
        screen.pen = Pen(0, 155, 0);
        short i;
        for (i = 1; i < pL; i++)
        {
            screen.rectangle(Rect(pX[i] * BLOCK, pY[i] * BLOCK, BLOCK, BLOCK));
        }
        screen.pen = Pen(0, 255, 0);
        screen.rectangle(Rect(pX[0] * BLOCK, pY[0] * BLOCK, BLOCK, BLOCK));
        screen.pen = Pen(255, 0, 0);        
        screen.rectangle(Rect(aX * BLOCK, aY * BLOCK, BLOCK, BLOCK));
        screen.pen = Pen(255, 255, 255);
        if (status > 1)
        {
            screen.text("GAME OVER", minimal_font, Point(SCREEN_WIDTH / 2 * BLOCK, SCREEN_HEIGHT / 2 * BLOCK), true, TextAlign::center_center);
            if (status == 3)
            {
                screen.text("- NEW HIGHSCORE -", minimal_font, Point(SCREEN_WIDTH / 2 * BLOCK, ((SCREEN_HEIGHT / 2) + 3) * BLOCK), true, TextAlign::center_center);
            }

             screen.text("PRESS A", minimal_font, Point(SCREEN_WIDTH / 2 * BLOCK, 92), true, TextAlign::center_center);
        }
    }
    screen.text(std::to_string(pL + 5) + "/" + std::to_string(highscore), minimal_font, Point(SCREEN_WIDTH * BLOCK - BLOCK, BLOCK - 1), true, TextAlign::top_right);
    screen.pen = Pen(0, 0, 0);
}

// update(time)
//
// This is called to update your game state. time is the 
// amount if milliseconds elapsed since the start of your game
void update(uint32_t time) 
{
    if (status == 0) 
    {
        if (buttons.pressed & Button::A)
        {
            status = 1;
            short i;
            for (i = 0; i < 5; i++)
            {
                pX[i] = SCREEN_WIDTH / 2;
                pY[i] = SCREEN_HEIGHT + i;
            }
            dT = 0;
            pL = 5;
            pD = 1;
            new_apple();
            aS = 0;
            freq = 1000;
        }
    }
    else if (status == 1)
    {
        dT--;
        if (dT < (int)floor(pL * .1))
        {
            dT = 9;
            short i;
            for (i = pL; i > 0; i--)
            {
                pX[i] = pX[i - 1];
                pY[i] = pY[i - 1];
            }
            pX[0] += pDIR[pD];
            pY[0] += pDIR[pD - 1];
       
            if (pX[0] < 0)
            {
                pX[0] = SCREEN_WIDTH - 1;
            }
            else if (pX[0] > SCREEN_WIDTH - 1)
            {
                pX[0] = 0;
            }
            else if (pY[0] < 0)
            {
                pY[0] = SCREEN_HEIGHT - 1;
            }
            else if (pY[0] > SCREEN_HEIGHT - 1)
            {
                pY[0] = 0;
            }

 
            if (pX[0] == aX && pY[0] == aY)
            {
                if (pL < 100)
                {
                    pL++;
                }
                freq = 500;
                new_apple();
                aS = 4;
            }
            else
            {
                short i;
                for (i = 1; i < pL; i++)
                {
                    if (pX[0] == pX[i] && pY[0] == pY[i])
                    {
                        status = 2;
                        aS = 15;
                        freq = 1000;                      
                        if (pL - 5 > highscore)
                        {
                            highscore = pL - 5;
                            write_save(highscore);
                            status = 3;
                        }
                    }
                }
            }
        }

        if (aS > 0)
        {
            aS--;
        }

        if (buttons & Button::DPAD_UP)
        {
            pD = 1;
        }
        if (buttons & Button::DPAD_RIGHT)
        {
            pD = 2;
        }
        if (buttons & Button::DPAD_DOWN)
        {
            pD = 3;
        }
        if (buttons & Button::DPAD_LEFT)
        {
            pD = 4;
        }
        if (freq < 800)
        {
            channels[0].frequency = freq;
            channels[0].trigger_attack();
            freq += 25;
        }    

    }
    else
    {
        if (buttons.pressed & Button::A)
        {
            status = 0;
        }
        if (aS > 0)
        {
            aS--;
        }

        if (freq > 0)
        {
            channels[0].frequency = freq;
            channels[0].trigger_attack();
            freq -= 10;
        }    
    }
}
