
#include <iostream>
#include <cstdlib>
#include <random>
#include "include/Angel.h"

using namespace std;

// constant variables
const int row = 20;
const int col = 10;
const int frame = 50;
const int spacing = 30; 				// 30 x 30 squares

const int window_size_x = frame * 2 + spacing * 10;   // x = 100 + 300 = 400
const int window_size_y = frame * 2 + spacing * 20;   // y = 700

// global variables
bool isOccupied[10][20];				// to check if the square is occupied by the block or not
vec2 points_for_board[1200];
vec3 colors_for_board[1200];
vec2 points_for_block[24];
vec3 colors_for_block[24];

vec2 tile[4];                     // each tile composed of 4 cells, i.e. 2d vectors from the centre cell
vec2 tilepos = vec2(5, 19);        // starting position

// vao
GLuint grid;
GLuint board;
GLuint block;
// vbo
GLuint buffer_grid[2];
GLuint buffer_board[2];
GLuint buffer_block[2];

GLuint loc_x;
GLuint loc_y;
GLuint vPosition;
GLuint vColor;

vec2 Block_O[4] = {vec2(-1,0), vec2(0,0), vec2(-1,-1), vec2(0,-1)};
vec2 Block_I[4] = {vec2(-2,0), vec2(-1,0), vec2(0,0), vec2(1,0)};
vec2 Block_S[4] = {vec2(-1,-1), vec2(0,-1), vec2(0,0), vec2(1,0)};
vec2 Block_Z[4] = {vec2(-1,0), vec2(0,0), vec2(0,-1), vec2(1,-1)};
vec2 Block_L[4] = {vec2(-1,-1), vec2(-1,0), vec2(0,0), vec2(1,0)};
vec2 Block_J[4] = {vec2(-1,0), vec2(0,0), vec2(1,0), vec2(1,-1)};
vec2 Block_T[4] = {vec2(-1,0), vec2(0,0), vec2(1,0), vec2(0,-1)};

enum block_type{
  O,
  I,
  S,
  Z,
  L,
  J,
  T
};
int current_block_type;


int random(int bound){
  random_device r;
  mt19937 ran(r());
  uniform_int_distribution<int> dist(0, bound);
  int result = dist(ran);
  return result;
}

void updateTile(){
  glBindBuffer(GL_ARRAY_BUFFER, buffer_block[0]);

  for(int i = 0; i < 4; i++){
    GLfloat x = tilepos.x + tile[i].x;
    GLfloat y = tilepos.y + tile[i].y;

    vec2 p1 = vec2(frame + (x * spacing), frame + (y * spacing));
    vec2 p2 = vec2(frame + (x * spacing), frame + (y * spacing) + spacing);
    vec2 p3 = vec2(frame + (x * spacing) + spacing, frame + (y * spacing));
    vec2 p4 = vec2(frame + (x * spacing) + spacing, frame + (y * spacing) + spacing);

    vec2 newPoints[6] = {p1, p2, p3, p2, p3, p4};

    glBufferSubData(GL_ARRAY_BUFFER, i * 6 * sizeof(vec2), 6 * sizeof(vec2), newPoints);
  }
  glBindVertexArray(0);
}

void newTile(){
  tilepos = vec2(5,19); // place the block in the middle of the board (** change)
  int tile_type = random(6);
  current_block_type = tile_type;

  for(int i = 0; i < 4; i++){
    switch(tile_type){
      case 0:
        tile[i] = Block_O[i];
        break;
      case 1:
        tile[i] = Block_I[i];
        break;
      case 2:
        tile[i] = Block_S[i];
        break;
      case 3:
        tile[i] = Block_Z[i];
        break;
      case 4:
        tile[i] = Block_L[i];
        break;
      case 5:
        tile[i] = Block_J[i];
        break;
      case 6:
        tile[i] = Block_T[i];
        break;
      default:
        break;
    }
  }
  updateTile();

  for(int i = 0; i < 24; i++){
    colors_for_block[i] = vec3(1.0, 0.0, 0.0);
  }
  glBindBuffer(GL_ARRAY_BUFFER, buffer_block[1]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(colors_for_block), colors_for_block);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);

  if(isOccupied[5][19] == true || isOccupied[4][19] == true || isOccupied[6][19] == true){
    printf("Gameover!!");

    exit(EXIT_SUCCESS);
  }
}

//==============================================================================
void initGrid(){
  // need 11+21 = 32 lines = 64 points
  vec2 points_for_grid[64];
	vec3 colors_for_grid[64];
	// 21 horizontal lines
	// spacing between 30;
	// points_for_line[0...41]
	for(int i = 0; i < row+1; i++){
		// start point
		points_for_grid[2 * i] = vec2( frame, (window_size_y - frame) - (spacing * i));
		// end point
		points_for_grid[2 * i + 1] = vec2( (window_size_x - frame), (window_size_y - frame) - (spacing * i));
	}
	// 11 vertical lines
	// spacing between 30;
	// points_for_line[42...63]
	for(int i = 0; i < col+1; i++){
		// start point (even)
		points_for_grid[(2 * i) + 42] = vec2( frame + (spacing * i), (window_size_y - frame));
		// end point (odd)
		points_for_grid[(2 * i + 1) + 42] = vec2( frame + (spacing * i), frame);
	}
	// set all color to white
	for(int i = 0; i < 64; i++){
		colors_for_grid[i] = vec3(1.0, 1.0, 1.0);
	}

  glGenVertexArrays(1, &grid);
  glBindVertexArray(grid);

  glGenBuffers(2, buffer_grid);
  // vertex position of Grid
  glBindBuffer(GL_ARRAY_BUFFER, buffer_grid[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(points_for_grid), points_for_grid, GL_STATIC_DRAW);
  glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vPosition);

  glBindBuffer(GL_ARRAY_BUFFER, buffer_grid[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colors_for_grid), colors_for_grid, GL_STATIC_DRAW);
  glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vColor);
}
//==============================================================================
void initBoard(){
	// board has 20 x 10 square blocks. triangluate -> 400 triangles
	// we need 3 points for each triangles
	int index = 0;
	for (int r = 0; r < row; r++){
		for (int c = 0; c < col; c++){
			points_for_board[index++] = vec2( frame + (spacing * c), window_size_y - frame - (spacing * r));
			points_for_board[index++] = vec2( frame + (spacing * c), window_size_y - frame - (spacing * r) - spacing);
			points_for_board[index++] = vec2( frame + (spacing * c) + spacing, window_size_y - frame - (spacing * r) - spacing);
			points_for_board[index++] = vec2( frame + (spacing * c), window_size_y - frame - (spacing * r));
			points_for_board[index++] = vec2( frame + (spacing * c) + spacing, window_size_y - frame - (spacing * r));
			points_for_board[index++] = vec2( frame + (spacing * c) + spacing, window_size_y - frame - (spacing * r) - spacing);
		}
	}
  for(int i = 0; i < 1200; i++){
		colors_for_board[i] = vec3(0.0, 0.0, 0.0);
	}
  for(int i = 0; i < col; i++){
    for(int j = 0; j < row; j++){
      isOccupied[i][j] = false;
    }
  }

  glGenVertexArrays(1, &board);
	glBindVertexArray(board);
	glGenBuffers(2, buffer_board);

	glBindBuffer(GL_ARRAY_BUFFER, buffer_board[0]);
	glBufferData(GL_ARRAY_BUFFER, 1200 * sizeof(vec2), points_for_board, GL_STATIC_DRAW);
  glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vPosition);

  glBindBuffer(GL_ARRAY_BUFFER, buffer_board[1]);
  glBufferData(GL_ARRAY_BUFFER, 1200 * sizeof(vec3), colors_for_board, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vColor);
}

//==============================================================================
void initBlock(){
  glGenVertexArrays(1, &block);
  glBindVertexArray(block);

  glGenBuffers(2, buffer_block);
  // vertex position of block
  glBindBuffer(GL_ARRAY_BUFFER, buffer_block[0]);
  glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(vec2), NULL, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vPosition);

  glBindBuffer(GL_ARRAY_BUFFER, buffer_block[1]);
  glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(vec3), NULL, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vColor);
}
//==============================================================================
void init(){
  // loads shader program
	GLuint shader = InitShader("vshader.glsl", "fshader.glsl");
  glUseProgram(shader);

	vPosition = glGetAttribLocation(shader, "vPosition");
  vColor = glGetAttribLocation(shader, "vColor");

  initGrid();
  initBoard();
  initBlock();

  loc_x = glGetUniformLocation(shader, "win_x");
	loc_y = glGetUniformLocation(shader, "win_y");

  newTile();

  glBindVertexArray(0);
  glClearColor(0,0,0,0);
}
//==============================================================================
// These for the boundary check
// get max x location of the block
int max_x(){
  int max = frame;
  for(int i = 0; i < 24; i++){
    if(points_for_block[i].x > max){
      max = points_for_block[i].x;
    }
  }
  return max;
}
// get min x location of the block
int min_x(){
  int min = window_size_x;
  for(int i = 0; i < 24; i++){
    if(points_for_block[i].x < min){
      min = points_for_block[i].x;
    }
  }
  return min;
}
// get min y location of the block
int min_y(){
  int min = window_size_y;
  for(int i = 0; i < 24; i++){
    if(points_for_block[i].y < min){
      min = points_for_block[i].y;
    }
  }
  return min;
}

//==============================================================================
void display(){
	glClear(GL_COLOR_BUFFER_BIT);

 	glUniform1i(loc_x, window_size_x);
	glUniform1i(loc_y, window_size_y);

  glBindVertexArray(board);
  glDrawArrays(GL_TRIANGLES, 0, 1200);

  glBindVertexArray(block);
  glDrawArrays(GL_TRIANGLES, 0, 24);

  glBindVertexArray(grid);
  glDrawArrays(GL_LINES, 0, 64);

  glutSwapBuffers();
}
//==============================================================================
//keyboard input
void keyboard( unsigned char key, int x, int y){
  switch (key){
    // press q to exit
    case 113:
      exit(EXIT_SUCCESS);
      break;
    // press r to restart
    case 114:
      init();
      break;
  }
}
void keyboardSpecial( int key, int x, int y){
  switch (key){
    // press down arrow key, drop the block faster
    case GLUT_KEY_DOWN:
      if (min_y() > frame){
        for(int i = 0; i < 24; i++){
          points_for_block[i] = points_for_block[i] - vec2(0.0, spacing);
          colors_for_board[i] = vec3(1.0,0.0,0.0);
        }
        glutPostRedisplay();
      }
      break;
    // press left arrow key, move to left
    case GLUT_KEY_LEFT:
      if (min_x() > frame){
        for(int i = 0; i < 24; i++){
          points_for_block[i] = points_for_block[i] - vec2(spacing, 0.0);
        }
      }
      glutPostRedisplay();
      break;
    // press right arrow key, move to right
    case GLUT_KEY_RIGHT:
      if (max_x() < window_size_x - frame){
        for(int i = 0; i < 24; i++){
          points_for_block[i] = points_for_block[i] + vec2(spacing, 0.0);
        }
      }
      glutPostRedisplay();
      break;
  }
}
//==============================================================================
// change the color of board (does not work)
// void find_location(){
//   glBindVertexArray(board);
//   glBindBuffer(GL_ARRAY_BUFFER, buffer_color);
//   for(int i = 0; i < 1200; i++){
//     if(points_for_board[i] == points_for_block[i]){
//         colors_for_board[i] = vec3(0.0, 1.0, 0.0);
//       }
//   }
// }
// slowly moves the block down
void dropDelay(int){
  if(min_y() > frame){
    for(int i = 0; i < 24; i++){
      points_for_block[i] = points_for_block[i] - vec2(0.0, spacing);
    }
  }
  glutPostRedisplay();
  glutTimerFunc(500.0, dropDelay, 0);
}
void idle(void){
  glutPostRedisplay();
}
//==============================================================================
// avoid the window reshaping
void resize(int width, int height) {
    // by ignore the params and set the window size
    glutReshapeWindow(window_size_x, window_size_y);
}
//==============================================================================
int main(int argc, char* argv[]){
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA);
  glutInitWindowSize(window_size_x, window_size_y);
	glutInitWindowPosition(850, 200);
  glutCreateWindow("Tetris");
  glewInit();
  init();

  glutDisplayFunc(display);
	glutReshapeFunc(resize);

  glutKeyboardFunc(keyboard);
  glutSpecialFunc(keyboardSpecial);
  glutIdleFunc(idle);
  glutTimerFunc(500.0, dropDelay, 0);

  glutMainLoop();
  return 0;
}
