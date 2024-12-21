
#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>
#include <string>
#include <queue>
#include <vector>
#define MAX_INPUT_CHARS     2
using namespace std;
static bool allowMove = false;
Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};

Color beige = { 211, 176, 131, 255 }; 
Color darkBrown = { 76, 63, 47, 255 } ;   
Color brown={ 127, 106, 79, 255 };  

int cellSize = 30;
int cellCount = 20;
int offset = 75;//for the boundary


void LaunchSnakeGame();
void LaunchMazeSolver();

void LaunchSnakeGame() {
    // Your Snake Game implementation
    TraceLog(LOG_INFO, "Snake Game Launched!");
}

// Placeholder function for Maze Solver
void LaunchMazeSolver() {
    // Your Maze Solver implementation
    TraceLog(LOG_INFO, "Maze Solver Launched!");
}


vector<vector<int>> mat(20, vector<int>(20, 1));
int n=20;
const int snake_size=30;
const int hole_height=35;
const int hole_width=46;
double lastUpdateTime = 0;
int t1=0;
int t2=0;
string pathit="";
int dr[4] = {1, 0, 0, -1}; // Directions for row movement: Down, Left, Right, Up
int dc[4] = {0, -1, 1, 0}; // Directions for column movement
char direction[4] = {'D', 'L', 'R', 'U'};



bool isValid(int row, int col, vector<vector<int>>& mat, vector<vector<bool>>& visited) {
    return row >= 0 && col >= 0 && row < n && col < n && mat[row][col] == 1 && !visited[row][col];
}

string findPath(int startRow, int startCol, int endRow, int endCol, vector<vector<int>>& mat) {
    vector<vector<bool>> visited(n, vector<bool>(n, false));
    queue<pair<pair<int, int>, string>> q; // Queue to store coordinates and the current path

    // Initialize BFS
    q.push({{startRow, startCol}, ""});
    visited[startRow][startCol] = true;

    while (!q.empty()) {
        pair<pair<int, int>, string> current = q.front();
pair<int, int> coords = current.first;
string path = current.second;

        q.pop();
        int row = coords.first;
        int col = coords.second;

        // Check if we reached the destination
        if (row == endRow && col == endCol) {
            return path;
        }

        // Explore neighbors
        for (int i = 0; i < 4; i++) {
            int nextRow = row + dr[i];
            int nextCol = col + dc[i];

            if (isValid(nextRow, nextCol, mat, visited)) {
                visited[nextRow][nextCol] = true;
                q.push({{nextRow, nextCol}, path + direction[i]});
            }
        }
    }

    return "No Path Found"; // Return this if no path exists
}


bool EventTriggered(double interval)//makes the program wait for the given interval
{
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval)
    {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

void generaterandomobstacles()
{
    for(int x=0;x<20;x++)
    {
		float numberofblocks=GetRandomValue(1,15);
		for(int j=0;j<numberofblocks;j++)
		{
			float y=GetRandomValue(0,19);
			mat[x][(int)y]=0;
		}
	}
}
void findpathbetween()
{
    if (t1 < 1 || t1 > 20 || t2 < 1 || t2 > 20) {
        cout << "Invalid input: t1 or t2 out of bounds!" << endl;
        pathit = "No Path Found";
        return;
    }
    int startRow = t1 - 1, startCol = 0;
    int endRow = t2 - 1, endCol = 19;

    pathit = findPath(startRow, startCol,endRow, endCol, mat);
    if (pathit == "No Path Found") {
        cout << "No valid path exists!" << endl;
    } else {
        cout << "Path: " << pathit << endl;
    }
}
void drawblocks()
{
	   for(int i=0;i<20;i++)
	   {
		   for(int j=0;j<20;j++)
		   {
			   if(mat[i][j]==0)
			   {
			   Rectangle block = Rectangle{(float)offset + j*cellSize,(float) offset + i*cellSize, (float)cellSize, (float)cellSize};
	           DrawRectangleRec(block, brown);
		      }
		   }
	   }
}

class smol_snake
{
	public:
	Texture2D texture;
	Texture2D texture2;
	Sound walkSound;
	smol_snake()
	{
		//InitAudioDevice();
		Image image = LoadImage("smol_snake2.png");
		Image image2 = LoadImage("snake_hole3.png");
		texture = LoadTextureFromImage(image);
		texture2 = LoadTextureFromImage(image2);
        UnloadImage(image);//unloads image
        UnloadImage(image2);//unloads image
        walkSound = LoadSound("snake-rattle-sound-hq-240150.mp3");
        
        
        
	}
	~smol_snake()
    {
        UnloadTexture(texture);
        UnloadTexture(texture2);
        UnloadSound(walkSound);
        //CloseAudioDevice();
    }
};


bool ElementInDeque(Vector2 element, deque<Vector2> deque)//checks if the snake has collided with itself
//by checking if the two locations in vector format are same
{
    for (unsigned int i = 0; i < deque.size(); i++)//loop for the snake's body
    {
        if (Vector2Equals(deque[i], element))
        {
            return true;
        }
    }
    return false;
}


class Snake
{
public:
// The snake is represented as a deque so that insertion and deletion can be done from both sides
    deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};//initial 3-body of the snake
    //the point (6,9) is the head
    Vector2 direction = {1, 0}; // initial direction of the snake
    bool addSegment = false;

    void Draw()
    {
        for (unsigned int i = 0; i < body.size(); i++)
        {
			//The snake gets drawn from left to right, we create the snake's head and consecutive body
			//segments are present at the beginning of the dequeue
            float x = body[i].x;
            float y = body[i].y;
            Rectangle segment = Rectangle{offset + x * cellSize, offset + y * cellSize, (float)cellSize, (float)cellSize};
            DrawRectangleRounded(segment, 0.5, 6, darkGreen);
        }
    }

    void Update()
    {
        body.push_front(Vector2Add(body[0], direction));//another segment added to the deck
        if (addSegment == true)// addsegment is true on collision with food, then we don't need to pop_back
        {
            addSegment = false;
        }
        else
        {
            body.pop_back();//hence the snake moves forward
        }
    }

    void Reset()//resets initial condition
    {
        body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
        direction = {1, 0};
    }
};

class Food
{

public:
    Vector2 position;// position of the food
    Texture2D texture;

    Food(deque<Vector2> snakeBody)//constructor which initializes the values
    {
        Image image = LoadImage("food.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);//unloads image
        position = GenerateRandomPos(snakeBody);
    }

    ~Food()
    {
        UnloadTexture(texture);
    }

    void Draw()
    {
        DrawTexture(texture, offset + position.x * cellSize, offset + position.y * cellSize, WHITE);
        //here white is a filter, white means no color filter is applied
    }

    Vector2 GenerateRandomCell()//generates random x and y values
    {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{x, y};
    }

    Vector2 GenerateRandomPos(deque<Vector2> snakeBody)
    {
        Vector2 position = GenerateRandomCell();
        while (ElementInDeque(position, snakeBody))//checks if the random position collides with the snake body
        {
            position = GenerateRandomCell();
        }
        return position;
    }
};

class Game
{
public:
    Snake snake = Snake();//snake object created
    Food food = Food(snake.body);//food object created
    bool running = true;//this flag is made so that when collision is encountered, this flag changes to false
    //and no update is made to the snake body
    int score = 0;//initial score
    Sound eatSound;//object for the sound class
    Sound wallSound;

    Game()
    {
        //InitAudioDevice();//this has to be done to initiate the relevant dependancies
        eatSound = LoadSound("eat.mp3");
        wallSound = LoadSound("wall.mp3");
    }

    ~Game()
    {
        UnloadSound(eatSound);
        UnloadSound(wallSound);
        //CloseAudioDevice();
    }

    void Draw()//draws snake and food
    {
        food.Draw();
        snake.Draw();
    }

    void Update()//updates snake position and checks collision
    {
        if (running)
        {
            snake.Update();
            CheckCollisionWithFood();
            CheckCollisionWithEdges();
            CheckCollisionWithTail();
        }
    }

    void CheckCollisionWithFood()
    {
        if (Vector2Equals(snake.body[0], food.position))//checks if their positions are equal
        {
            food.position = food.GenerateRandomPos(snake.body);//after a particular food is consumed
            //another is generated instantly at another position
            snake.addSegment = true;//this will allow for increase in the snake's body
            score++;//score increases
            PlaySound(eatSound);//plays 
        }
    }

    void CheckCollisionWithEdges()
    {
		/* The game board is generated from position 0 to position cellCount-1
		 * If the position of the head exceeds cellcount-1 or recedes 0
		 * this marks a collision with the walls
		 */
        if (snake.body[0].x == cellCount || snake.body[0].x == -1)
        {
            GameOver();
        }
        if (snake.body[0].y == cellCount || snake.body[0].y == -1)
        {
            GameOver();
        }
    }

    void GameOver()
    {
        snake.Reset();//initial snake position is re-drawn
        food.position = food.GenerateRandomPos(snake.body);//new food generated
        running = false;//this prevents the game from updating
        score = 0;//score resets
        PlaySound(wallSound);//the sound for failure is played
    }

    void CheckCollisionWithTail()
    {
        deque<Vector2> headlessBody = snake.body;//this will store the body minus the head
        headlessBody.pop_front();
        if (ElementInDeque(snake.body[0], headlessBody))//checks if that particular position is present
        //in the dequeue
        {
            GameOver();
        }
    }
};

void snake_game()
{
	cout << "Starting the game..." << endl;
    //InitWindow(2 * offset + cellSize * cellCount, 2 * offset + cellSize * cellCount, "Retro Snake");
    /*This initialises the dependencies, a window is created of the specified size
     *but nothing would be shown until anything is drawn
     * */
    SetTargetFPS(60);
    //FPS is set so that the game runs in a uniform speed on all machines
    Game game = Game();//Game object is created

    while (WindowShouldClose() == false)//While escape or cross is not pressed
    {
        BeginDrawing();//raylib function, does what it states

        if (EventTriggered(0.2))//This makes sure that we donot see the snake move at 60fps
        {
            allowMove = true;
            game.Update();
        }

        if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1 && allowMove)
        {//when key_direction and snake direction are not opposite
            game.snake.direction = {0, -1};//changes the direction to up
            game.running = true;
            /* Updation of the snake depends on the running parameter
             * therefore we make sure that its true when we need the snake to move
             * the running parameter is false when a collision is made
             */
            allowMove = false;
        }
        if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1 && allowMove)
        {
            game.snake.direction = {0, 1};
            game.running = true;
            allowMove = false;
        }
        if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1 && allowMove)
        {
            game.snake.direction = {-1, 0};
            game.running = true;
            allowMove = false;
        }
        if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1 && allowMove)
        {
            game.snake.direction = {1, 0};
            game.running = true;
            allowMove = false;
        }

        // Drawing
        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle{(float)offset - 5, (float)offset - 5, (float)cellSize * cellCount + 10, (float)cellSize * cellCount + 10}, 5, darkGreen);
        //Draws rectangle with extended parameters and defined thickness
        DrawText("Retro Snake", offset - 5, 20, 40, darkGreen);
        DrawText(TextFormat("%i", game.score), offset - 5, offset + cellSize * cellCount + 10, 40, darkGreen);
        game.Draw();

        EndDrawing();
    }
    //CloseWindow();
}
void maze_solver()
{
	
	int ind=0;
	int newx=0;
	int newy=0;
	bool alreadyreached=false;
	bool done=false;
	
	
	
	
    Rectangle textBox3 = { (float)((offset+cellSize*cellCount)/2), (float)(offset + cellSize * cellCount + 10), 50, 50 };
    bool mouseOnText3 = false;
	char name2[MAX_INPUT_CHARS + 1] = "\0";      // NOTE: One extra space required for null terminator char '\0'
    int letterCount2 = 0;

    Rectangle textBox2 = { (float)(offset + cellSize * cellCount-109), (float)(offset + cellSize * cellCount + 30),(float)30,(float)30 };
    bool mouseOnText2 = false;

	char name[MAX_INPUT_CHARS + 1] = "\0";      // NOTE: One extra space required for null terminator char '\0'
    int letterCount = 0;
    Rectangle textBox = {(float)(offset), (float)(offset + cellSize * cellCount + 30),(float)30,(float)30 };
    bool mouseOnText = false;
	for(int i=0;i<20;i++)
	{
		for(int j=0;j<20;j++)
		{
			mat[i][j]=1;
		}
		cout<<endl;
	}
    cout << "Starting the game..." << endl;
    //InitWindow(2 * offset + cellSize * cellCount, 2 * offset + cellSize * cellCount, "Retro Snake");
    /*This initialises the dependencies, a window is created of the specified size
     *but nothing would be shown until anything is drawn
     * */
    SetTargetFPS(60);
    //FPS is set so that the game runs in a uniform speed on all machines
    
    smol_snake smsn= smol_snake();
    Texture2D sn=smsn.texture;
    Texture2D hol=smsn.texture2;
    Sound walkSound=smsn.walkSound;
    generaterandomobstacles();
    
    bool mousepressed=false;
    findpathbetween();
    cout<<"Path : "<<pathit<<endl;
    
    while (WindowShouldClose() == false)//While escape or cross is not pressed
    {
		if (CheckCollisionPointRec(GetMousePosition(), textBox3)) mouseOnText3 = true;
        else mouseOnText3 = false;

        if (mouseOnText3)
        {
            
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
             {
                mousepressed=true;
                //findpathbetween();
		     } 
        }
		if (CheckCollisionPointRec(GetMousePosition(), textBox2)) mouseOnText2 = true;
        else mouseOnText2 = false;

        if (mouseOnText2&&!mousepressed)
        {
            // Set the window's cursor to the I-Beam
            SetMouseCursor(MOUSE_CURSOR_IBEAM);

            // Get char pressed (unicode character) on the queue
            int key2 = GetCharPressed();

            // Check if more characters have been pressed on the same frame
            while (key2 > 0)
            {
                // NOTE: Only allow keys in range [32..125]
                if ((key2 >= 32) && (key2 <= 125) && (letterCount2 < MAX_INPUT_CHARS))
                {
                    name2[letterCount2] = (char)key2;
                    name2[letterCount2+1] = '\0'; // Add null terminator at the end of the string.
                    letterCount2++;
                }

                key2 = GetCharPressed();  // Check next character in the queue
            }

            if (IsKeyPressed(KEY_BACKSPACE))
            {
                letterCount2--;
                if (letterCount2 < 0) letterCount2 = 0;
                name2[letterCount2] = '\0';
            }
        }
        else {SetMouseCursor(MOUSE_CURSOR_DEFAULT);};

        
		
        if (CheckCollisionPointRec(GetMousePosition(), textBox)) mouseOnText = true;
        else mouseOnText = false;

        if (mouseOnText&&!mousepressed)
        {
            // Set the window's cursor to the I-Beam
            SetMouseCursor(MOUSE_CURSOR_IBEAM);

            // Get char pressed (unicode character) on the queue
            int key = GetCharPressed();

            // Check if more characters have been pressed on the same frame
            while (key > 0)
            {
                // NOTE: Only allow keys in range [32..125]
                if ((key >= 32) && (key <= 125) && (letterCount < MAX_INPUT_CHARS))
                {
                    name[letterCount] = (char)key;
                    name[letterCount+1] = '\0'; // Add null terminator at the end of the string.
                    letterCount++;
                }

                key = GetCharPressed();  // Check next character in the queue
            }

            if (IsKeyPressed(KEY_BACKSPACE))
            {
                letterCount--;
                if (letterCount < 0) letterCount = 0;
                name[letterCount] = '\0';
            }
        }
        else SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        
        
        if (mousepressed && (!done)) {
    try {
        t1 = stoi(name);
        t2 = stoi(name2);
        findpathbetween();
        done = true;
    } catch (exception& e) {
        cout << "Invalid input: " << e.what() << endl;
        done = true;
    }
}
        BeginDrawing();//raylib function, does what it states
        // Drawing
        ClearBackground(beige);
        DrawRectangleLinesEx(Rectangle{(float)offset - 5, (float)offset - 5, (float)cellSize * cellCount + 10, (float)cellSize * cellCount + 10}, 5, darkBrown);
        //Draws rectangle with extended parameters and defined thickness
        DrawText("Maze Solver", offset - 5, 20, 40, darkBrown);
        for(int i=1;i<=20;i++)
        {
			DrawText(TextFormat("%i", i), offset+cellSize*cellCount+10, offset+cellSize*(i-1),30,darkBrown);
		}
		DrawText("Snake Row : ", offset - 5, offset + cellSize * cellCount + 10, 20, darkBrown);
		DrawText("House Row : ", offset + cellSize * cellCount-115, offset + cellSize * cellCount + 10, 20, darkBrown);
		DrawRectangleRec(textBox2, brown);
            if (mouseOnText2) DrawRectangleLines((int)textBox2.x, (int)textBox2.y, (int)textBox2.width, (int)textBox2.height, RED);
            else DrawRectangleLines((int)textBox2.x, (int)textBox2.y, (int)textBox2.width, (int)textBox2.height, DARKGRAY);

            DrawText(name2, (int)textBox2.x + 5, (int)textBox2.y + 8, 20, darkBrown);
		DrawRectangleRec(textBox, brown);
		if (mouseOnText) DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, RED);
            else DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, DARKGRAY);

            DrawText(name, (int)textBox.x + 5, (int)textBox.y + 8, 20, darkBrown);
        
        DrawRectangleRec(textBox3, brown);
            if (mouseOnText3) DrawRectangleLines((int)textBox3.x, (int)textBox3.y, (int)textBox3.width, (int)textBox3.height, RED);
            else DrawRectangleLines((int)textBox3.x, (int)textBox3.y, (int)textBox3.width, (int)textBox3.height, DARKGRAY);

            DrawText("Start", (int)textBox3.x + 2, (int)textBox3.y + 8, 18, darkBrown);
        drawblocks();
        if(!mousepressed)
        {
			DrawTexture(sn,35,offset+cellSize*19, WHITE);
			DrawTexture(hol,offset+cellCount*cellSize+15,offset, WHITE);//draws hole
		}
		else
		{
			DrawTexture(hol,offset+cellCount*cellSize+35,offset+(t2-1)*cellSize, WHITE);
			if (EventTriggered(0.7))//This makes sure that we donot see the snake move at 60fps
            { 
			   if(!alreadyreached) PlaySound(walkSound);
                 if(!alreadyreached)
                 {
                    if(pathit[ind]=='D')
					{
						newx=newx+1;
					}
					else if(pathit[ind]=='L')
					{
						newy=newy-1;
					}
					else if(pathit[ind]=='R')
					{
						newy=newy+1;
					}
					else if(pathit[ind]=='U')
					{
						newx=newx-1;
					}
					
					ind++;
					if(ind==pathit.size())
					{
						alreadyreached=true;
					}
				}
				
		} 
		DrawTexture(sn,offset+(newy)*cellSize,offset+cellSize*(t1-1)+newx*cellSize, WHITE);	
		}
        EndDrawing();
    }
    //CloseWindow();
}
void menu()
{
	Sound bgSound;
	InitAudioDevice();
	bgSound = LoadSound("Backgroundmusic.mp3");
	const int screenWidth = 2 * offset + cellSize * cellCount;
    const int screenHeight = 2 * offset + cellSize * cellCount;
    InitWindow(screenWidth, screenHeight, "Snake Empire");
    Texture2D background = LoadTexture("background.png"); 
    // Define button properties
    Rectangle snakeButton = {(float) screenWidth / 2 - 100, 200, 200, 50 };
    Rectangle mazeButton = {(float) screenWidth / 2 - 100, 300, 200, 50 };
    Rectangle exitButton = {(float) screenWidth / 2 - 100, 400, 200, 50 };

    bool exitProgram = false;
    PlaySound(bgSound);
    // Main game loop
    while (!exitProgram && !WindowShouldClose()) {
        // Check button clicks
        
        Vector2 mousePoint = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(mousePoint, snakeButton)) {
                snake_game();
            } else if (CheckCollisionPointRec(mousePoint, mazeButton)) {
                maze_solver();
            } else if (CheckCollisionPointRec(mousePoint, exitButton)) {
                exitProgram = true;
            }
        }

        // Drawing
        BeginDrawing();
        DrawTexture(background, 0, 0, WHITE); 
        ClearBackground(RAYWHITE);

        // Draw buttons
        DrawRectangleRec(snakeButton, brown);
        DrawRectangleRec(mazeButton, brown);
        DrawRectangleRec(exitButton, brown);

        // Highlight button on hover
        if (CheckCollisionPointRec(mousePoint, snakeButton)) DrawRectangleRec(snakeButton, darkBrown);
        if (CheckCollisionPointRec(mousePoint, mazeButton)) DrawRectangleRec(mazeButton, darkBrown);
        if (CheckCollisionPointRec(mousePoint, exitButton)) DrawRectangleRec(exitButton, darkBrown);

        // Draw button labels
        DrawText("Snake Game", snakeButton.x + 40, snakeButton.y + 15, 20, BLACK);
        DrawText("Maze Solver", mazeButton.x + 40, mazeButton.y + 15, 20, BLACK);
        DrawText("Exit", exitButton.x + 80, exitButton.y + 15, 20, BLACK);

        EndDrawing();
    }

    // Cleanup
    UnloadSound(bgSound);
    CloseAudioDevice();
    UnloadTexture(background);
    CloseWindow();
}
int main()
{
	menu();
	return 0;
}
