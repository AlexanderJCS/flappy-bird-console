#include <iostream>
#include <windows.h>
#include <conio.h>
#include <string>
#include <vector>

#define RED "\u001b[31m"
#define GREEN "\u001b[32m"
#define YELLOW "\u001b[33m"
#define RESETCOLOR "\u001b[0m"

const int WIDTH = 50;
const int HEIGHT = 25;
const int REFRESH = 100;

const int JUMP_HEIGHT = HEIGHT / 4 - 1;

bool playAgain()
{
    std::cout << "Play again? (y/n): ";
    char response;
    std::cin >> response;

    if (response == 'y')
        return true;

    else
        return false;
}

class Bird {
public:
    int x = 4;
    int y = HEIGHT / 2;

    void jump(bool smallJump) 
    {
        if (smallJump)
            y -= 3;
        
        else
            y -= JUMP_HEIGHT;
    }

    void fall()
    {
        y++;
    }
};

class Pipe 
{
public:
    int x = WIDTH;
    int bottom = rand() % (HEIGHT - 7 + 1);
    int top = bottom + 5 + rand() % 2;

    void move()
    {
        x--;
    }
};

class Game 
{
private:
    Bird bird;
    std::vector<Pipe> pipes;

    void draw()
    {
        std::vector<std::string> screen;  // Create board to be printed

        screen.push_back(std::string(""));  // Add empty line

        // Print top border
        screen[0] += "+";
        for (int i = 0; i <= WIDTH; i++)
            screen[0] += "- ";

        screen[0] += "- +\n";

        // Print the main area
        for (int y = 0; y < HEIGHT - 1; y++)
        {
            screen.push_back(std::string(""));  // Add empty line
            screen[y + 1] += "|";  // Add the side border

            for (int x = 0; x <= WIDTH; x++)
            {
                if (y == bird.y && x == bird.x)
                    screen[y + 1] += "O";

                else
                {
                    bool isPipe = false;

                    // Iterate through all pipes to see if there is a pipe at this position
                    for (Pipe pipe : pipes)
                    {
                        if ((x == pipe.x && y >= pipe.top) || (x == pipe.x && y <= pipe.bottom))
                        {
                            isPipe = true;
                            screen[y + 1] += std::string(GREEN) + '|' + std::string(RESETCOLOR);
                            break;
                        }
                    }
                    if (!isPipe)
                        screen[y + 1] += " ";  // Add the empty space
                }
                screen[y + 1] += " ";  // Makes the board evenly spaced between x and y
            }
            screen[y + 1] += "|\n";  // Add the side border
        }

        screen.push_back(std::string(""));  // Add empty line

        // Add the bottom border
        screen[screen.size() - 1] += "+";
        for (int i = 0; i < WIDTH; i++)
            screen[screen.size() - 1] += "- ";

        screen[screen.size() - 1] += "- +";

        // Print the board
        system("cls");  // Windows only

        for (std::string line : screen)
            std::cout << line;
    }

    bool jumped()  // Gets user input
    {
        if (_kbhit())
        {
            switch (_getch())
            {
                case ' ':
                    return true;
                
                case 'x':
                    exit(0);
                    break;
            }
        }
        return false;
    }

    bool collisionCheck()  // Check if the bird collides with a pipe or floor
    {
        if (bird.y < 0 || bird.y > HEIGHT)
            return true;

        for (Pipe pipe : pipes)
        {
            if (pipe.x != bird.x)
                continue;
            
            if (pipe.top <= bird.y || pipe.bottom >= bird.y)
                return true;
        }
        return false;
    }

    bool determineJumpHeight()
    {
        bool smallJump = false;

        if (bird.y < JUMP_HEIGHT)  // If the bird should do a small jump
            return true;

        else  // If the bird is near a pipe, do a small jump
        {
            for (Pipe pipe : pipes)
            {
                int diff = pipe.x - bird.x;

                if (0 <= diff && diff <= 2)
                    return true;
            }
        }
        return false;
    }

    void waitForStart()
    {
        while (true)
        {
            draw();
            std::cout << YELLOW << "\nPress space to start" << RESETCOLOR << "\n";

            if (jumped())
                return;
            
            Sleep(REFRESH);
        }
    }

public:
    void run()
    {
        waitForStart();
        int score = 0;
        while (true)
        {
            pipes.push_back(Pipe());
            for (int i = 0; i < rand()% (25 - 18 + 1) + 18; i++) 
            {   
                draw();
                std::cout << GREEN << "\nScore: " << score << RESETCOLOR << "\n";

                // Check if the bird collides with a pipe or floor
                if (collisionCheck())
                    goto endGame;

                if (jumped())  // Check if the the player pressed the spacebar
                    bird.jump(determineJumpHeight());  // Jump the bird

                else
                    bird.fall();
                
                // Move the pipes back and update score
                for (Pipe &pipe : pipes) 
                {
                    pipe.move();

                    if (pipe.x == bird.x)
                        score += 10;
                }

                // Check if a pipe is out of the screen
                for (int i = pipes.size(); i >= 0; i--)
                {
                    if (pipes[i].x < 0)
                        pipes.erase(pipes.begin() + i);
                }
                
                Sleep(REFRESH); 
            }
        }

        endGame:
            return;
    }
};

int main()
{
    while (true)
    {
        Game game;
        game.run();

        if (!playAgain())
            break;
    }
    return 0;
}
