/*
Description: A console interfaced pathfinding program, that takes as input either
            a maze from a text file, or uses user input data to randomly generate a maze.
            The maze is then solved using Breadth-First Search, and the output is shown in the console.
Usage: <exe> |<-f> <infile>|
        The -f flag, along with the infile, can be optionally passed in if the user has their own maze
*/

// import necessary libraries
#include <iostream>
#include <iomanip>
#include <fstream>
#include <queue>
#include <ctime>
#include "termcolor.hpp"
using std::cout;
using std::cin;
using std::endl;
using std::setw;
using std::queue;
using std::ifstream;

// function prototypes
void PrintEnvironment(int** intGrid, char** charGrid, int width, int height);
void BFS(int** intGrid, int width, int height, int goalX, int goalY);
int findPath(int** intGrid, char** charGrid, int width, int height, int startX, int startY, int goalX, int goalY);

// main driver function that does error handling and controls the entire UI
int main(int argc, char** argv)
{
    // Declare variables
    int width, height;
    int** intGrid;
    // if run normally, enter this if statement
    if (argc == 1)
    {
        // Introduction to program
        // Get width and height from user
        cout << termcolor::bright_cyan << "Welcome to Sagar's CSE240 WaveFront Pather\n" << endl;
        cout << "Please tell me about the grid you want to generate." << endl;
        cout << "Enter the width (at least 10): " << termcolor::reset;
        cin >> width;
        cout << termcolor::bright_cyan << "Enter the height (at least 10): " << termcolor::reset;
        cin >> height;

        // Dynamic allocation of 2d int array
        intGrid = new int*[width];
        for(int i = 0; i < width; i++)
            intGrid[i] = new int[height];

        // Initialize grid positions to 0
        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < height; j++)
                intGrid[i][j] = 0;
        }

        char userPrompt;
        int impassTerrain = 0;
        // Prompt user to enter percent of impassable terrain
        // Input checking using a while loop
        while (true)
        {
            cout << termcolor::bright_cyan << "Enter the percent of impassable terrain (0 - 100): " << termcolor::reset;
            cin >> impassTerrain;
            if (impassTerrain < 0 || impassTerrain > 100)
            {
                cout << termcolor::bright_red << "\nPlease enter a value between 0 and 100\n" << termcolor::reset << endl;
                continue;
            }
            if (impassTerrain > 35)
            {
                cout << termcolor::bright_red << "\nHaving a value greater than 35% might create poor results, do you wish to continue? (y/n): " << termcolor::reset;
                cin >> userPrompt;
                cout << endl;
                if (userPrompt == 'y')
                    break;
            }
            else
                break;
        }

        int numBlocked = (impassTerrain/100.0) * width * height;
        int numMarked = 0;
        srand(time(0)); // set random seed
        // mark user defined percentage of the grid as "blocked" or -1
        while (numMarked < numBlocked)
        {
            int x = rand() % width;
            int y = rand() % height;
            if (intGrid[x][y] == 0)
            {
                intGrid[x][y] = -1;
                numMarked++;
            }
        }
    }
    else if (argc == 3 && argv[1][0] == '-' && argv[1][1] == 'f')
    {
        ifstream infile(argv[2]);
        if (!infile)
        {
            cout << termcolor::bright_red << "Error while reading file" << termcolor::reset << endl;
            cout << termcolor::bright_red << "Error -- usage: <exe> |<-f> <infile>|" << termcolor::reset << endl;
            return 0;
        }
        else
        {
            try
            {
                // get width and height from first two lines
                infile >> width;
                infile >> height;
                // Dynamic allocation of 2d int array
                intGrid = new int*[width];
                for(int i = 0; i < width; i++)
                    intGrid[i] = new int[height];
                int integer;
                // read each integer and update the int grid
                for (int i = 0; i < height; i++)
                {
                    for (int j = 0; j < width; j++)
                    {
                        infile >> integer;
                        if (integer == 1)
                            intGrid[j][i] = -1;
                        else
                            intGrid[j][i] = 0;
                    }
                }
                // close file after done
                infile.close();
            }
            // catch any errors and display to user
            catch (...)
            {
                cout << termcolor::bright_red << "Error while reading file" << termcolor::reset << endl;
                cout << termcolor::bright_red << "Error -- usage: <exe> |<-f> <infile>|" << termcolor::reset << endl;
                return 0;
            }
        }
    }
    else
    {
        // print error statement
        cout << termcolor::bright_red << "Error -- usage: <exe> |<-f> <infile>|" << termcolor::reset << endl;
        return 0;
    }
    // Dynamic Allocation of 2d char array
    // This is the array that will be presented
    char** charGrid = new char*[width];
    for(int i = 0; i < width; i++)
        charGrid[i] = new char[height];

    // Update blocked positions with a "#" from the int array
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            if (intGrid[i][j] == -1)
                charGrid[i][j] = '#';
            else
                charGrid[i][j] = ' ';
        }
    }

    // Print the grid
    PrintEnvironment(intGrid, NULL, width, height);

    // Prompt user to enter starting and ending coordinates
    int goalX, goalY, startX, startY;

    while (true)
    {
        // get goal position with some error handling
        cout << termcolor::bright_cyan << "Please enter the Goal Position X: " << termcolor::reset;
        cin >> goalX;
        cout << termcolor::bright_cyan << "Please enter the Goal Position Y: " << termcolor::reset;
        cin >> goalY;
        if (goalX >= width || goalY >= height || goalX < 0 || goalY < 0)
        {
            cout << termcolor::bright_red << "\nSorry, that position is out of bounds.\n" << termcolor::reset << endl;
            continue;
        }
        else if (intGrid[goalX][goalY] == -1)
        {
            cout << termcolor::bright_red << "\nSorry, that position is inside an obstacle.\n" << termcolor::reset << endl;
            continue;
        }
        intGrid[goalX][goalY] = 1;
        break;
    }
    // update goal position in char grid
    charGrid[goalX][goalY] = '$';
    // print the char grid
    PrintEnvironment(NULL, charGrid, width, height);
    // run the BFS algorithm
    BFS(intGrid, width, height, goalX, goalY);

    while (true)
    {
        // get starting positions with some error handling
        cout << termcolor::bright_cyan << "Please enter the Start Position X: " << termcolor::reset;
        cin >> startX;
        cout << termcolor::bright_cyan << "Please enter the Start Position Y: " << termcolor::reset;
        cin >> startY;
        if (startX == goalX && startY == goalY)
        {
            cout << termcolor::bright_red << "\nSorry, that is the goal position.\n" << termcolor::reset << endl;
            continue;
        }
        else if (startX >= width || startY >= height || startX < 0 || startY < 0)
        {
            cout << termcolor::bright_red << "\nSorry, that position is out of bounds.\n" << termcolor::reset << endl;
            continue;
        }
        else if (intGrid[startX][startY] == -1)
        {
            cout << termcolor::bright_red << "\nSorry, that position is inside an obstacle.\n" << termcolor::reset << endl;
            continue;
        }
        break;
    }

    // find the path
    int found = findPath(intGrid, charGrid, width, height, startX, startY, goalX, goalY);

    // print char grid
    PrintEnvironment(NULL, charGrid, width, height);
    // if no path found, print to user
    if (found == 0)
        cout << termcolor::bright_red << "No Path Found" << termcolor::reset << endl;

    // delete int array
    for(int i = 0; i < width; ++i)
    {
        delete [] intGrid[i];
    }
    delete [] intGrid;

    // delete char array
    for(int i = 0; i < width; ++i)
    {
        delete [] charGrid[i];
    }
    delete [] charGrid;
    return 1;
}

// function that handles the printing of the int and char grid to the console
void PrintEnvironment(int** intGrid, char** charGrid, int width, int height)
{
    // if int grid is passed in, enter this if statement
    if (intGrid != NULL)
    {   
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width ; j++)
            {
                // print each number with its respective color styling
                if (intGrid[j][i] == -1)
                    cout << termcolor::bright_red << setw(4) << intGrid[j][i] << termcolor::reset;
                else
                    cout << termcolor::bright_magenta << setw(4) << intGrid[j][i] << termcolor::reset;
            }
            cout << endl;
        }
    }
    // if chargrid is passed and not int grid
    else
    {
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width ; j++)
            {
                // print each char with its respective color styling
                if (charGrid[j][i] == '#')
                    cout << termcolor::bright_red << setw(3) << charGrid[j][i] << termcolor::reset;
                else if (charGrid[j][i] == '@')
                    cout << termcolor::bright_blue << setw(3) << charGrid[j][i] << termcolor::reset;
                else if (charGrid[j][i] == '$' || charGrid[j][i] == '*')
                    cout << termcolor::bright_green << setw(3) << charGrid[j][i] << termcolor::reset;
                else
                    cout << setw(3) << charGrid[j][i];
            }
            // go to next line
            cout << endl;
        }
    }
    // return nothing
    return;
}

// function that finds the path and updates the char grid after the BFS algorithm is run
int findPath(int** intGrid, char** charGrid, int width, int height, int startX, int startY, int goalX, int goalY)
{
    // set stopping criteria for the loop
    int maxIterations = width * height;
    int iter = 0;
    // update starting position of char grid
    charGrid[startX][startY] = '@';
    int currX = startX;
    int currY = startY;
    while (iter < maxIterations)
    {
        // breaking condition
        if (currX == goalX && currY == goalY)
            return 1;
        iter++;
        int adjX, adjY;
        int adjacentX[8] = {1,0,1,-1,-1,-1,0,1};
        int adjacentY[8] = {1,1,0,0,-1,1,-1,-1};
        // for each adjacent position
        for (int i = 0; i < 8; i++)
        {
            adjX = currX + adjacentX[i];
            adjY = currY + adjacentY[i];
            // if it is a valid position and less than current position
            if (adjX < width && adjY < height && adjX >= 0 && adjY >= 0 && intGrid[adjX][adjY] != -1 && 
            intGrid[adjX][adjY] < intGrid[currX][currY])
            {
                // make this the current position
                currX = adjX;
                currY = adjY;
                if (adjX == goalX && adjY == goalY)
                    continue;
                // make it a path on the char grid
                charGrid[adjX][adjY] = '*';
            }
        }
    }
    return 1;
}

// function that performs BFS and updates the int grid accordingly with the correct distances
void BFS(int** intGrid, int width, int height, int goalX, int goalY)
{
    // Dynamic Allocation of 2d bool array
    bool** boolGrid = new bool*[width];
    for(int i = 0; i < width; i++)
        boolGrid[i] = new bool[height];

    // this is a grid to keep track of nodes I have already visited
    // true means visited, and false means unvisited
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            if (intGrid[i][j] == -1 || intGrid[i][j] == 1)
                boolGrid[i][j] = true;
            else
                boolGrid[i][j] = false;
        }
    }
    // queues used for implementing BFS
    queue<int> xPos;
    queue<int> yPos;
    // push goal positions into queue
    xPos.push(goalX);
    yPos.push(goalY);

    int currX, currY, adjX, adjY;
    // while there are positions to be explored, stay in the while loop
    while (!xPos.empty())
    {
        // get front positions in the queue and pop them
        currX = xPos.front();
        xPos.pop();
        currY = yPos.front();
        yPos.pop();

        boolGrid[currX][currY] = true;
        int adjacentX[8] = {1,0,1,-1,-1,-1,0,1};
        int adjacentY[8] = {1,1,0,0,-1,1,-1,-1};
        // check all adjacent positions
        for (int i = 0; i < 8; i++)
        {
            adjX = currX + adjacentX[i];
            adjY = currY + adjacentY[i];
            // if it is in bounds and unvisited, update its value
            if (adjX < width && adjY < height && adjX >= 0 && adjY >= 0 && boolGrid[adjX][adjY] == false)
            {
                intGrid[adjX][adjY] = intGrid[currX][currY] + 1;
                boolGrid[adjX][adjY] = true;    // set it as visited
                xPos.push(adjX);
                yPos.push(adjY);
            }
        }
    }
    // delete the bool grid
    for(int i = 0; i < width; ++i)
    {
        delete [] boolGrid[i];
    }
    delete [] boolGrid;
    // return nothing
    return;
}