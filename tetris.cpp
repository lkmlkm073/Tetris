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
bool boardTiles[10][20];				// to check if the square is occupied by the block or not
vec2 points_for_board[1200];
vec3 colors_for_board[1200];
vec2 points_for_block[24];
vec3 colors_for_block[24];

vec2 tile[4];                     // each tile composed of 4 cells, i.e. 2d vectors from the centre cell
vec2 tilepos = vec2(5,19);        // starting position

int block_type;
vec2 xy_coord[4];
bool I_flag;

// vaos
GLuint grid;
GLuint board;
GLuint block;
// vbos: [0] - position, [1] - colors
GLuint buffer_grid[2];
GLuint buffer_board[2];
GLuint buffer_block[2];

GLuint loc_x;
GLuint loc_y;
GLuint vPosition;
GLuint vColor;

// blocks
vec2 Block_O[4] = {vec2(-1,0), vec2(0,0), vec2(-1,-1), vec2(0,-1)};     // block_type = 0
vec2 Block_I[4] = {vec2(-1,0), vec2(0,0), vec2(1,0), vec2(2,0)};        // 1
vec2 Block_S[4] = {vec2(-1,-1), vec2(0,-1), vec2(0,0), vec2(1,0)};      // 2
vec2 Block_Z[4] = {vec2(-1,0), vec2(0,0), vec2(0,-1), vec2(1,-1)};      // 3
vec2 Block_L[4] = {vec2(-1,-1), vec2(-1,0), vec2(0,0), vec2(1,0)};      // 4
vec2 Block_J[4] = {vec2(-1,0), vec2(0,0), vec2(1,0), vec2(1,-1)};       // 5
vec2 Block_T[4] = {vec2(-1,0), vec2(0,0), vec2(1,0), vec2(0,-1)};       // 6

//==============================================================================
// return 0~6 randomly
int random(int bound){
  random_device r;
  mt19937 ran(r());
  uniform_int_distribution<int> dist(0, bound);
  int result = dist(ran);
  return result;
}
// update current block's location
void updateTile(){
  glBindBuffer(GL_ARRAY_BUFFER, buffer_block[0]);
  int count = 0;
  for(int i = 0; i < 4; i++){
    GLfloat x = tilepos.x + tile[i].x;
    GLfloat y = tilepos.y + tile[i].y;

    vec2 p1 = vec2(frame + (x * spacing), frame + (y * spacing));
    vec2 p2 = vec2(frame + (x * spacing), frame + (y * spacing) + spacing);
    vec2 p3 = vec2(frame + (x * spacing) + spacing, frame + (y * spacing));
    vec2 p4 = vec2(frame + (x * spacing) + spacing, frame + (y * spacing) + spacing);

    vec2 newPoints[6] = {p1, p2, p3, p2, p3, p4};
    for(int j = 0; j < 6; j++){
      points_for_block[count] = newPoints[j];
      count++;
    }
  }
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points_for_block), points_for_block);
  glBindVertexArray(0);
}
// create new block
void newTile(){
  tilepos = vec2(5,19);
  block_type = random(6);

  for(int i = 0; i < 4; i++){
    switch(block_type){
      case 0:
        tile[i] = Block_O[i];
        break;
      case 1:
        tilepos = vec2(4, 18);
        tile[i] = Block_I[i];
        I_flag = 1;
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

  if(boardTiles[5][19] == true || boardTiles[4][19] == true || boardTiles[6][19] == true){
    printf("Gameover!!\n");

    exit(EXIT_SUCCESS);
  }
}
// 90 degree ccw rotation
void rotate(){
  mat2 rotation = mat2(0, 1,
                      -1, 0);
  for(int i = 0; i < 4; i++){
    tile[i] = rotation * tile[i];
  }
}
// 90 degree cw rotation to cancel the ccw rotation
void rotate_cw(){
  mat2 rotation = mat2(0, -1,
                      1, 0);
  for(int i = 0; i < 4; i++){
    tile[i] = rotation * tile[i];
  }
}
//==============================================================================
void initGrid(){
  // need 21+11 = 32 lines = 64 points
  vec2 points_for_grid[64];
	vec3 colors_for_grid[64];
	// 21 horizontal lines
	for(int i = 0; i < row+1; i++){
		// start point
		points_for_grid[2 * i] = vec2( frame, (window_size_y - frame) - (spacing * i));
		// end point
		points_for_grid[2 * i + 1] = vec2( (window_size_x - frame), (window_size_y - frame) - (spacing * i));
	}
	// 11 vertical lines
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
  // vertex position of grid
  glBindBuffer(GL_ARRAY_BUFFER, buffer_grid[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(points_for_grid), points_for_grid, GL_STATIC_DRAW);
  glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vPosition);
  // vertex color of grid
  glBindBuffer(GL_ARRAY_BUFFER, buffer_grid[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colors_for_grid), colors_for_grid, GL_STATIC_DRAW);
  glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vColor);
}
//==============================================================================
void initBoard(){
	// board has 20 x 10 square blocks. triangluate -> 400 triangles
	// we need 3 points for each triangles
	for (int r = 0; r < row; r++){
		for (int c = 0; c < col; c++){
			vec2 p1 = vec2(frame + (spacing * c), frame + (spacing * r));
			vec2 p2 = vec2(frame + (spacing * c), frame + (spacing * r) + spacing);
			vec2 p3 = vec2(frame + (spacing * c) + spacing, frame + (spacing * r));
			vec2 p4 = vec2(frame + (spacing * c) + spacing, frame + (spacing * r) + spacing);

      points_for_board[(10 * r + c) * 6] = p1;
      points_for_board[(10 * r + c) * 6 + 1] = p2;
      points_for_board[(10 * r + c) * 6 + 2] = p3;
      points_for_board[(10 * r + c) * 6 + 3] = p2;
      points_for_board[(10 * r + c) * 6 + 4] = p3;
      points_for_board[(10 * r + c) * 6 + 5] = p4;
		}
	}
  for(int i = 0; i < 1200; i++){
		colors_for_board[i] = vec3(0.0, 0.0, 0.0);    // initialize all board colors to black
	}
  for(int i = 0; i < col; i++){
    for(int j = 0; j < row; j++){                 // initialize all boardTiles[10][20] to false
      boardTiles[i][j] = false;
    }
  }

  glGenVertexArrays(1, &board);
	glBindVertexArray(board);
	glGenBuffers(2, buffer_board);
  // vertex positions of board
	glBindBuffer(GL_ARRAY_BUFFER, buffer_board[0]);
	glBufferData(GL_ARRAY_BUFFER, 1200 * sizeof(vec2), points_for_board, GL_STATIC_DRAW);
  glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vPosition);
  // vertex colors of board
  glBindBuffer(GL_ARRAY_BUFFER, buffer_board[1]);
  glBufferData(GL_ARRAY_BUFFER, 1200 * sizeof(vec3), colors_for_board, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vColor);
}
//==============================================================================
// initially, no geometry for block
void initBlock(){
  glGenVertexArrays(1, &block);
  glBindVertexArray(block);

  glGenBuffers(2, buffer_block);
  // vertex position of block
  glBindBuffer(GL_ARRAY_BUFFER, buffer_block[0]);
  glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(vec2), NULL, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vPosition);
  // vertex colors of block
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
  // Game initialization
  newTile();

  glBindVertexArray(0);
  glClearColor(0,0,0,0);
}
//==============================================================================
// check conditions
// return leftmost, rightmost and bottom tile(vec2) of the block
vec2 find_min_max(int mode){
  int index = 0;
  int max_x = (int)xy_coord[0].x;
  int min_x = (int)xy_coord[0].x;
  int min_y = (int)xy_coord[0].y;

  switch(mode){
    // max_x
    case 1:
      for(int i = 1; i < 4; i++){
        if(max_x < (int)xy_coord[i].x){
          max_x = (int)xy_coord[i].x;
          index = i;
        }
      }
      break;
    // min_x
    case 2:
      for(int i = 1; i < 4; i++){
        if(min_x > (int)xy_coord[i].x){
          min_x = (int)xy_coord[i].x;
          index = i;
        }
      }
      break;
    // min_y
    case 3:
      for(int i = 1; i < 4; i++){
        if(min_y > (int)xy_coord[i].y){
          min_y = (int)xy_coord[i].y;
          index = i;
        }
      }
      break;
    default:
      break;
  }
  return xy_coord[index];
}
bool checkBelow(){
  for(int i = 0; i < 4; i++){
    int x = tilepos.x + tile[i].x;
    int y = tilepos.y + tile[i].y;
    if(y == 0 || boardTiles[x][y - 1]){
      return true;
    }
  }
  return false;
}
bool checkLeft(){
  for(int i = 0; i < 4; i++){
    int x = tilepos.x + tile[i].x;
    int y = tilepos.y + tile[i].y;
    if(x == 0 || boardTiles[x - 1][y]){
      return true;
    }
  }
  return false;
}
bool checkRight(){
  for(int i = 0; i < 4; i++){
    int x = tilepos.x + tile[i].x;
    int y = tilepos.y + tile[i].y;
    if(x == 9 || boardTiles[x + 1][y]){
      return true;
    }
  }
  return false;
}
bool checkRotation(){
  // if block O
  if(block_type == 0){
    return false;
  }
  // if block I
  else if(block_type == 1){
    // vertical I
    if(I_flag == 1){
      for(int i = 0; i < 4; i++){
        xy_coord[i] = vec2(tilepos.x + tile[i].x, tilepos.y + tile[i].y);
      }
      vec2 bottom_tile = find_min_max(3);
      if(bottom_tile.y + 3 > 19){
        return false;
      }
      if(bottom_tile.y < 0 || boardTiles[(int)bottom_tile.x][(int)bottom_tile.y]){
        if(!boardTiles[(int)bottom_tile.x][(int)bottom_tile.y + 4]){
          I_flag = 0;
          tilepos = tilepos + vec2(0, 1);
          return true;
        }
        else{
          return false;
        }
      }
      I_flag = 0;
      return true;
    }
    // horizontal I
    if(I_flag == 0){
      rotate_cw();
      rotate_cw();

      for(int i = 0; i < 4; i++){
        xy_coord[i] = vec2(tilepos.x + tile[i].x, tilepos.y + tile[i].y);
      }
      vec2 right_tile = find_min_max(1);
      vec2 left_tile = find_min_max(2);

      if(left_tile.x == -1 || boardTiles[(int)left_tile.x][(int)left_tile.y]){
        if(right_tile.x + 1 > 9 || boardTiles[(int)right_tile.x + 1][(int)right_tile.y]){
          // do not rotate
          rotate();
          return true;
        }
        else{
          if(boardTiles[(int)right_tile.x][(int)right_tile.y] || boardTiles[(int)right_tile.x - 1][(int)right_tile.y]){
            // do not rotate
            rotate();
            return true;
          }
          I_flag = 1;
          tilepos = tilepos + vec2(1, 0);
          return true;
        }
      }

      if(right_tile.x == 11 || right_tile.x == 10 || boardTiles[(int)right_tile.x][(int)right_tile.y] || boardTiles[(int)right_tile.x - 1][(int)right_tile.y]){
        if(right_tile.x == 11){
          if(boardTiles[(int)left_tile.x - 2][(int)left_tile.y] || boardTiles[(int)left_tile.x - 1][(int)left_tile.y]){
            // do not rotate
            rotate();
            return true;
          }
          else{
            I_flag = 1;
            tilepos = tilepos - vec2(2, 0);
            return true;
          }
        }
        if(right_tile.x == 10){
          if(boardTiles[(int)left_tile.x - 1][(int)left_tile.y]){
            // do not rotate
            rotate();
            return true;
          }
          else{
            I_flag = 1;
            tilepos = tilepos - vec2(1, 0);
            return true;
          }
        }
        if(boardTiles[(int)right_tile.x][(int)right_tile.y] && !boardTiles[(int)right_tile.x - 1][(int)right_tile.y]){
          if(left_tile.x - 1 < 0 || boardTiles[(int)left_tile.x - 1][(int)left_tile.y]){
            // do not rotate
            rotate();
            return true;
          }
          else{
            I_flag = 1;
            tilepos = tilepos - vec2(1, 0);
            return true;
          }
        }
        if(boardTiles[(int)right_tile.x - 1][(int)right_tile.y]){
          if(left_tile.x - 2 < 0 || boardTiles[(int)left_tile.x - 2][(int)left_tile.y] || boardTiles[(int)left_tile.x - 1][(int)left_tile.y]){
            // do not rotate
            rotate();
            return true;
          }
          else{
            I_flag = 1;
            tilepos = tilepos - vec2(2, 0);
            return true;
          }
        }
      }

      for(int i = 0; i < 4; i++){
        if(boardTiles[(int)xy_coord[i].x][(int)xy_coord[i].y]){
          rotate();
          return true;
        }
      }
      I_flag = 1;
      return true;
    }
  }
  // other blocks
  else{
    for(int i = 0; i < 4; i++){
      xy_coord[i] = vec2(tilepos.x + tile[i].x, tilepos.y + tile[i].y);
    }
    vec2 right_tile = find_min_max(1);
    vec2 left_tile = find_min_max(2);
    vec2 bottom_tile = find_min_max(3);

    if(left_tile.x < 0){
      if(!boardTiles[(int)right_tile.x + 1][(int)right_tile.y]){
        tilepos = tilepos + vec2(1, 0);
        return true;
      }
      else{
        return false;
      }
    }
    if(right_tile.x > 9){
      if(!boardTiles[(int)left_tile.x - 1][(int)left_tile.y]){
        tilepos = tilepos - vec2(1, 0);
        return true;
      }
      else{
        return false;
      }
    }
    if(bottom_tile.y < 0){
      tilepos = tilepos + vec2(0, 1);
      return true;
    }

    if((int)tilepos.x - (int)left_tile.x == 1 && boardTiles[(int)tilepos.x - 1][(int)tilepos.y] && !boardTiles[(int)right_tile.x + 1][(int)right_tile.y]){
      if(right_tile.x + 1 > 9){
        return false;
      }
      tilepos = tilepos + vec2(1, 0);
      return true;
    }

    if((int)right_tile.x - (int)tilepos.x == 1 && boardTiles[(int)tilepos.x + 1][(int)tilepos.y] && !boardTiles[(int)left_tile.x - 1][(int)left_tile.y]){
      if(left_tile.x - 1 < 0){
        return false;
      }
      tilepos = tilepos - vec2(1, 0);
      return true;
    }

    for(int i = 0; i < 4; i++){
      if(boardTiles[(int)xy_coord[i].x][(int)xy_coord[i].y]){
        return false;
      }
    }
  }
  return true;
}
// check if the row is fully occupied
void checkRow(int row){
  bool full_row = true;

  for(int i = 0; i < 10; i++){
    full_row = full_row && boardTiles[i][row] == true;
  }

  if(full_row){
    glBindBuffer(GL_ARRAY_BUFFER, buffer_board[1]);
    for(int i = row; i < 20; i++){
      // row(10 squares and 6 points each)
      for(int j = 0; j < 60; j++){
        if(i == 19){
          colors_for_board[(19 * 60) + j] = vec3(0, 0, 0);
        }
        else{
          colors_for_board[(60 * i) + j] = colors_for_board[60 * (i + 1) + j];
        }
      }
      for(int j = 0; j < 10; j++){
        if(i == 19){
          boardTiles[j][i] = false;
        }
        else{
          boardTiles[j][i] = boardTiles[j][i + 1];
        }
      }
    }
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors_for_board), colors_for_board, GL_DYNAMIC_DRAW);
    glBindVertexArray(0);
  }
  if(full_row){
    checkRow(row);
  }
  else{
    if(row < 20){
      checkRow(row + 1);
    }
  }
}
// find the location of tile and change the board color
void setBoard(){
  int min_y = 20;
  for(int i = 0; i < 4; i++){
    int x = tilepos.x + tile[i].x;
    int y = tilepos.y + tile[i].y;

    if(y < min_y){
      min_y = y;
    }

    int index = (x + y * 10) * 6;
    for(int j = index; j < index+6; j++){
      colors_for_board[j] = vec3(0.5, 0.5, 0.5);
    }
    boardTiles[x][y] = true;
  }
  glBindBuffer(GL_ARRAY_BUFFER, buffer_board[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colors_for_board), colors_for_board, GL_DYNAMIC_DRAW);

  checkRow(min_y);

  newTile();
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
void keyboardSpecial(int key, int x, int y){
  switch (key){
    // rotate first and then check, if false, rotate cw to put a block back
    case GLUT_KEY_UP:
      rotate();
      if(checkRotation()){
        updateTile();
      }
      else{
        rotate_cw();
        updateTile();
      }
      break;
    // press down arrow key, drop the block faster
    case GLUT_KEY_DOWN:
      if(checkBelow()){
        setBoard();
      }
      tilepos = tilepos - vec2(0, 1.0);
      updateTile();
      glutPostRedisplay();
      break;
    // press left arrow key, move to left
    case GLUT_KEY_LEFT:
      if(checkLeft()){
        break;
      }
      tilepos = tilepos - vec2(1.0, 0);
      updateTile();
      glutPostRedisplay();
      break;
    // press right arrow key, move to right
    case GLUT_KEY_RIGHT:
      if (checkRight()){
        break;
      }
      tilepos = tilepos + vec2(1.0, 0);
      updateTile();
      glutPostRedisplay();
      break;
  }
}
//==============================================================================
// slowly moves the block down
void dropDelay(int){
  if(checkBelow()){
    setBoard();
  }
  else{
    tilepos = tilepos - vec2(0, 1.0);
    updateTile();
  }
  glutPostRedisplay();
  glutTimerFunc(500.0, dropDelay, 0);         // speed change here
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
  glutTimerFunc(500.0, dropDelay, 0);

  glutMainLoop();
  return 0;
}
