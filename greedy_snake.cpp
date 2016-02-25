#include <iostream>
#include <windows.h>
#include <conio.h>
#include <deque>
#include <string>
#include <ctime>    //srand(time(NULL));

#define SIZE 22
#define UP 0
#define LEFT 1
#define RIGHT 2
#define DOWN 3

using namespace std;

void gotoxy(int x,int y);

typedef struct
{
    int x;
    int y;
} Pos2D;

class Snake;
class Food;

class Food
{
    friend class Snake;
private:
    Pos2D food_pos;
    string food_shape = "★";
public:
    Food(Snake snake);
    void changeFood(Snake snake);
};

class Snake
{
    friend class Food;
private:
    Pos2D head;     //the position of snake head
    Pos2D tail;     // the element behind the snake
    Pos2D food;
    int score = 0;
    bool touchfood = false;     // whether touch food or not
    string bodyShape = "▇";
    string clearShape = "  ";
    deque<Pos2D> body_area;     //snake body area (a vector of position)
    void changeBodyArea(int move_direction);
    void paintBody();

public:
    Snake(int headx, int heady);
    Snake(deque<Pos2D> posSet);
    void goUp();
    void goLeft();
    void goRight();
    void goDown();
    void showBody();
    bool hasCrash();
    void EatOrNot(Food food);
    void controlSnake(Food food);
    int getScore();
};

//====================  class Snake =============================
Snake::Snake(int headx, int heady)
{
    head.x = headx;
    head.y = heady;
    tail.x = headx;
    tail.y = heady;
    body_area.push_front(head);
}

Snake::Snake(deque<Pos2D> posSet)
{
    body_area = posSet;
    head.x = posSet.begin()->x;
    head.y = posSet.begin()->y;
    tail.x = posSet.rbegin()->x;
    tail.y = posSet.rbegin()->y;
}

void Snake::showBody()
{
    deque<Pos2D>::iterator pos;
    for(pos=body_area.begin(); pos!=body_area.end(); pos++)
    {
        gotoxy(2*(pos->x), pos->y);
        cout << bodyShape;
    }
}

void Snake::changeBodyArea(int move_direciton)
{
    switch(move_direciton)
    {
    case UP:
        head.y--;
        break;
    case LEFT:
        head.x--;
        break;
    case RIGHT:
        head.x++;
        break;
    case DOWN:
        head.y++;
        break;
    }
    Pos2D newhead = {head.x, head.y};
    body_area.push_front(newhead);      //insert head

    tail.x = body_area.rbegin() -> x;
    tail.y = body_area.rbegin() -> y;
    if (!touchfood)
        body_area.pop_back();           //delete tail
}

void Snake::paintBody()
{
    /*  clear the tail */
    if (!touchfood)
    {
        gotoxy(2*tail.x, tail.y);       //clear tail
        cout << clearShape;
    }

    /*  paint the head */
    if (!hasCrash())
    {
        gotoxy(2*head.x, head.y);           //add new head
        cout << bodyShape;
    }
}

void Snake::goUp()
{
    changeBodyArea(UP);
    paintBody();
}

void Snake::goLeft()
{
    changeBodyArea(LEFT);
    paintBody();
}

void Snake::goRight()
{
    changeBodyArea(RIGHT);
    paintBody();
}

void Snake::goDown()
{
    changeBodyArea(DOWN);
    paintBody();
}

bool Snake::hasCrash()
{
    if (head.x<0
            || head.x == SIZE
            || head.y == SIZE
            || head.y < 0)               // crash the wall
        return true;

    deque<Pos2D>::iterator pos = ++body_area.begin();
    for(pos; pos!=body_area.end(); pos++)
        if(head.x==pos->x && head.y==pos->y)            // crash self
            return true;

    return false;
}

void Snake::EatOrNot(Food food)
{
    if (head.x==food.food_pos.x && head.y==food.food_pos.y)
    {
        score += 10;
        touchfood = true;
    }
    else
        touchfood = false;
}

int Snake::getScore()
{
    return score;
}

void Snake::controlSnake(Food food)   // 1) control snake to move;    2) tell the snake status
{
    char direction = ' ';
    while (true)
    {
        if(kbhit()!=0)
        {
            direction = getch();
            switch(direction)
            {
            case 'w':
                goUp();
                EatOrNot(food);
                food.changeFood(*this);
                break;
            case 'a':
                goLeft();
                EatOrNot(food);
                food.changeFood(*this);
                break;
            case 'd':
                goRight();
                EatOrNot(food);
                food.changeFood(*this);
                break;
            case 's':
                goDown();
                EatOrNot(food);
                food.changeFood(*this);
                break;
            }
            if (direction == 'e')
            {
                gotoxy(0,SIZE+1);
                break;
            }
            if (hasCrash())
            {
                gotoxy(SIZE-5, SIZE/2);
                cout << "Game Over..." << endl;
                gotoxy(0, SIZE+1);
                break;
            }
        }
        else
        {
            switch(direction)
            {
            case 'w':
                goUp();
                EatOrNot(food);
                food.changeFood(*this);
                break;
            case 'a':
                goLeft();
                EatOrNot(food);
                food.changeFood(*this);
                break;
            case 'd':
                goRight();
                EatOrNot(food);
                food.changeFood(*this);
                break;
            case 's':
                goDown();
                EatOrNot(food);
                food.changeFood(*this);
                break;
            }
            if (hasCrash())
            {
                gotoxy(SIZE-5, SIZE/2);
                cout << "Game Over..." << endl;
                gotoxy(0, SIZE+1);
                break;
            }
        }
        while(kbhit()!=0)   //清理kbhit缓冲区
            getch();
        gotoxy(2*(SIZE+1), 11);
        cout << "Your Score:                ";   // clear the score
        gotoxy(2*(SIZE+1), 11);
        cout << "Your Score: " << getScore();
        Sleep(200);
    }
}


//=============================== class Food ========================
Food::Food(Snake snake)
{
    //  create food
    int randx, randy;
    deque<Pos2D>::iterator pos;
    bool snake_food_crash = true;
    while(snake_food_crash == true)
    {
        //  if snake body and food have repeated
        snake_food_crash = false;
        randx = rand() % SIZE;
        randy = rand() % SIZE;
        for(pos=snake.body_area.begin(); pos!=snake.body_area.end(); pos++)
            if (randx == pos->x && randy == pos->y)
                snake_food_crash = true;
    }
    food_pos.x = randx;
    food_pos.y = randy;
    gotoxy(food_pos.x*2, food_pos.y);   //  paint the food
    cout << food_shape;
}

void Food::changeFood(Snake snake)
{
    if (snake.touchfood)
    {
        //  create food
        int randx, randy;
        deque<Pos2D>::iterator pos;
        bool snake_food_crash = true;
        while(snake_food_crash == true)
        {
            //  if snake body and food have repeated
            snake_food_crash = false;
            randx = rand() % SIZE;
            randy = rand() % SIZE;
            for(pos=snake.body_area.begin(); pos!=snake.body_area.end(); pos++)
                if (randx == pos->x && randy == pos->y)
                    snake_food_crash = true;
        }
        food_pos.x = randx;
        food_pos.y = randy;
        gotoxy(food_pos.x*2, food_pos.y);   //  paint the food
        cout << food_shape;
    }
}

void paintField(int lenX, int lenY);
//====================== main ==========================
int main(void)
{
    char inputx;
    do
    {
        srand(time(NULL));          //  select random seed (according to the time)
        paintField(SIZE, SIZE);
        //Snake snake1(3, 10);
        deque<Pos2D> snakeBody;     //  create the snake
        for(int i=5; i>0; i--)
        {
            Pos2D newpos = {i, 3};
            snakeBody.push_back(newpos);
        }
        Snake snake1(snakeBody);
        snake1.showBody();

        Food food(snake1);          //  create the food according to the snake
        snake1.controlSnake(food);       // 1) control snake to move;    2) tell the snake status

        gotoxy(0, SIZE+1);
        cout << "Press 'r' to restart game..."
                << endl
                << "   ...or Press 'e' to exit..." << endl;
        inputx = getch();
        while(inputx != 'e' && inputx != 'r')
            inputx = getch();
    }
    while(inputx == 'r');

    return 0;
}

void paintField(int x, int y)
{
    int i,j;

    /* paint the field */
    for(i=0; i<y; i++)
    {
        for(j=0; j<2*x; j++)
        {
            gotoxy(j, i);
            cout << "  ";
        }
    }

    /* paint the border */
    for(i=0; i<y; i++)
    {
        gotoxy(2*x, i);
        cout << "|";
    }
    for(i=0; i<x; i++)
    {
        gotoxy(2*i, y);
        cout << "--";
    }

    gotoxy(2*(SIZE+1), 3);
    cout << "-- Yancy's game --";
    gotoxy(2*(SIZE+1), 5);
    cout << "Press 'w/a/s/d' to move!";
    gotoxy(2*(SIZE+1), 7);
    cout << "Press 'e' to end the game.";
    gotoxy(2*(SIZE+1), 9);
    cout << "Press any other key to pause.";

    /* clear status bar */
    gotoxy(0, SIZE+1);
    cout << "                                            "
            <<endl
                <<"                                              "
                    << endl;
}

void gotoxy(int x,int y)
{
    CONSOLE_SCREEN_BUFFER_INFO    csbiInfo;
    HANDLE    hConsoleOut;
    hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hConsoleOut,&csbiInfo);
    csbiInfo.dwCursorPosition.X = x;
    csbiInfo.dwCursorPosition.Y = y;
    SetConsoleCursorPosition(hConsoleOut,csbiInfo.dwCursorPosition);
}

