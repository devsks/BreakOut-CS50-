//
// breakout.c
//
// Computer Science 50
// Problem Set 3
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include <spl/gevents.h>
#include <spl/gobjects.h>
#include <spl/gwindow.h>
#include <spl/ginteractors.h>

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3
// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));
    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);
    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;
    
    GLabel label = initScoreboard(window);
    
    int points = 0;
    double vx=1.5,vy=3.0;
    
    GEvent event=NULL;
    // keep playing until game over
    waitForClick();
    while (lives > 0 && bricks > 0)
    {
        // move ball
        move(ball, vx, vy);
        pause(10);
        
        // check for mouse event.
        GEvent event = getNextEvent(MOUSE_EVENT);
        
        // Lock the paddle X to the cursor.
        if (event != NULL)
        {
            // if the event was movement
            if (getEventType(event) == MOUSE_MOVED)
            {
                // ensure paddle follows top cursor
                double x = getX(event) - getWidth(paddle) / 2;
                double y = 540;
                if(x+getWidth(paddle) < getWidth(window ) && x>0)
                setLocation(paddle, x, y);
            }
        }
        
        GObject object = detectCollision(window, ball);
        
        if (object != NULL)
        {
            // If the ball hits the paddle.
            if (object == paddle)
            {
                    vy = -vy;
            }
            
            // If the ball hits a block. Remove block, add a point, decrement count and bounce.
            else if (strcmp(getType(object), "GRect") == 0)
            {
                removeGWindow(window, object);
                vy = -vy;
                points++;
                bricks--;                
                updateScoreboard(window, label, points);
            }
        }
        
        // If the ball hits the right wall.
        
       
        if (getX(ball) + getWidth(ball) >= getWidth(window))
        {
            vx= -vx;
        }
        
        // If the ball hits the left wall.
        if (getX(ball) <= 0)
        {
            vx = -vx;
        }
        
        // If the ball hits the top wall.
        if (getY(ball) <= 0)
        {
            vy = -vy;
        }
        
        // If the ball hits the bottom. Remove a life. Start over.
        if (getY(ball) + getHeight(ball) >= getHeight(window))
        {
            lives--;
            //move ball to start
            setLocation(ball, 190, 200);
            //move paddle to start
            setLocation(paddle, 70, 540);
            if(lives)
            waitForClick();
        }
        
    }
    
    // Displaing Message
    if (lives && bricks == 0)
       setLabel(label,"You WON ! :)");
    else
       setLabel(label,"No Lives Left :(");
        
    setColor(label, "CYAN");    
    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    y = y - 6;
    setLocation(label, x, y);


    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    int i, l = 0, c = 5;
    char brick_color[5][10] = {"RED","ORANGE","YELLOW","GREEN","CYAN"};
    while(c--)
    {
        for( i = 2 ; i + 35 <= 400; i += 40)
        {
        
            GRect rec = newGRect(i, 50 + l * 15, 35, 10);
            setColor(rec, brick_color[l]);
            setFilled(rec, true);
            add(window, rec);
        }   
        
        l++;
    }
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    GOval circle = newGOval(190, 290, 20, 20);
    setColor(circle, "BLACK");
    setFilled( circle, true);
    add (window, circle);
    return circle;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    GRect paddle = newGRect(170,540,60,10);
    setColor(paddle,"BLACK");
    setFilled(paddle,true);
    add(window,paddle);
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    GLabel label = newGLabel("0");
    setFont(label, "SansSerif-50");
    setColor(label, "C0C0C0");
    add(window, label);
    setLocation(label, 185, 300);
    return label;
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    char score[2];
    // update label
    sprintf(score,"%d",points);
    setLabel(label,score);
    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    y = y - 6;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
