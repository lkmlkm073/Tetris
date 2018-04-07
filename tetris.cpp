#include <cstdlib>
#include <ctime>
#include <iostream>
#include "include/Angel.h"

using namespace std;

// constant variables
const int row = 20;
const int col = 10;
const int frame = 50;
const int spacing = 30; 				// 30 x 30 squares
const int pieces = 7;

const int window_size_x = frame * 2 + spacing * 10;   // x = 100 + 300 = 400
const int window_size_y = frame * 2 + spacing * 20;   // y = 700

// global variables
bool isOccupied[row][col];				// to check if the square is occupied by the block or not
vec2 points_for_board[1200];
vec3 colors_for_board[1200];
vec2 points_for_block[24];

// vao
GLuint grid;
GLuint board;
GLuint block;
// vbo
GLuint buffer_board;
GLuint buffer_color;
GLuint buffer_block;


GLuint win_x;
GLuint win_y;
GLuint vPosition;
GLuint vColor;

//==============================================================================
void Grid(){
  // need 11+21 = 32 lines = 64 points
  vec2 points_for_line[64];
	vec3 colors_for_line[64];
	// 21 horizontal lines
	// spacing between 30;
	// points_for_line[0...41]
	for(int i = 0; i < row+1; i++){
		// start point
		points_for_line[2 * i] = vec2( frame, (window_size_y - frame) - (spacing * i) );
		// end point
		points_for_line[2 * i + 1] = vec2( (window_size_x - frame), (window_size_y - frame) - (spacing * i) );
	}
	// 11 vertical lines
	// spacing between 30;
	// points_for_line[42...63]
	for(int i = 0; i < col+1; i++){
		// start point (even)
		points_for_line[(2 * i) + 42] = vec2( frame + (spacing * i), (window_size_y - frame) );
		// end point (odd)
		points_for_line[(2 * i + 1) + 42] = vec2( frame + (spacing * i), frame );
	}
	// set all color to white
	for(int i = 0; i < 64; i++){
		colors_for_line[i] = vec3(1.0, 1.0, 1.0);
	}

  glGenVertexArrays( 1, &grid);
  glBindVertexArray( grid );

  GLuint buffer_grid;
  glGenBuffers( 1, &buffer_grid);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_grid);

  glBufferData( GL_ARRAY_BUFFER, sizeof(points_for_line) + sizeof(colors_for_line), points_for_line, GL_STATIC_DRAW);

  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points_for_line), points_for_line );
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(points_for_line), sizeof(colors_for_line), colors_for_line );

  glEnableVertexAttribArray( vPosition );
  glVertexAttribPointer( vPosition, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

  glEnableVertexAttribArray( vColor );
  glVertexAttribPointer( vColor, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points_for_line)) );
}
//==============================================================================
void Board(){
  for(int i = 0; i < row; i++){
    for(int j = 0; j < col; j++){
      isOccupied[i][j] = false;
    }
  }
	// board has 20 x 10 square blocks. triangluate -> 400 triangles
	// we need 3 points for each triangles
	int index = 0;
	for (int r = 0; r < row; r++){
		for (int c = 0; c < col; c++){
			points_for_board[index++] = vec2( frame + (spacing * c), window_size_y - frame - (spacing * r) );
			points_for_board[index++] = vec2( frame + (spacing * c), window_size_y - frame - (spacing * r) - spacing );
			points_for_board[index++] = vec2( frame + (spacing * c) + spacing, window_size_y - frame - (spacing * r) - spacing );
			points_for_board[index++] = vec2( frame + (spacing * c), window_size_y - frame - (spacing * r) );
			points_for_board[index++] = vec2( frame + (spacing * c) + spacing, window_size_y - frame - (spacing * r) );
			points_for_board[index++] = vec2( frame + (spacing * c) + spacing, window_size_y - frame - (spacing * r) - spacing );
		}
	}

	glGenVertexArrays(1, &board);
	glBindVertexArray(board);

	glGenBuffers(1, &buffer_board);
	glBindBuffer(GL_ARRAY_BUFFER, buffer_board);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points_for_board), points_for_board, GL_STATIC_DRAW);
  glEnableVertexAttribArray( vPosition );
  glVertexAttribPointer( vPosition, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

  glGenBuffers(1, &buffer_color);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_color);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colors_for_board), colors_for_board, GL_STATIC_DRAW);
  glEnableVertexAttribArray( vColor );
  glVertexAttribPointer( vColor, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
}
//==============================================================================
void Block(){
  vec3 colors_for_block[24];
  for(int i = 0; i < 24; i++){
    colors_for_block[i] = vec3(1.0, 0.0, 0.0);
  }

  glGenVertexArrays(1, &block);
  glBindVertexArray(block);

  glGenBuffers(1, &buffer_block);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_block);
  glBufferData(GL_ARRAY_BUFFER, sizeof(points_for_block) + sizeof(colors_for_block), points_for_block, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points_for_block), points_for_block);
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(points_for_block), sizeof(colors_for_block), colors_for_block);

  glEnableVertexAttribArray(vPosition);
  glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(vColor);
  glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points_for_block)));
}

//==============================================================================
void init(){
  // for test purpose, draw one block
  points_for_block[0] = vec2(170, 680);
  points_for_block[1] = vec2(170, 650);
  points_for_block[2] = vec2(200, 650);

  points_for_block[3] = vec2(170, 680);
  points_for_block[4] = vec2(200, 650);
  points_for_block[5] = vec2(200, 680);

  points_for_block[6] = vec2(200, 680);
  points_for_block[7] = vec2(200, 650);
  points_for_block[8] = vec2(230, 650);

  points_for_block[9] = vec2(200, 680);
  points_for_block[10] = vec2(230, 650);
  points_for_block[11] = vec2(230, 680);

  points_for_block[12] = vec2(230, 650);
  points_for_block[13] = vec2(230, 680);
  points_for_block[14] = vec2(260, 650);

  points_for_block[15] = vec2(230, 680);
  points_for_block[16] = vec2(260, 650);
  points_for_block[17] = vec2(260, 680);

  points_for_block[18] = vec2(200, 680);
  points_for_block[19] = vec2(230, 680);
  points_for_block[20] = vec2(200, 710);

  points_for_block[21] = vec2(200, 710);
  points_for_block[22] = vec2(230, 680);
  points_for_block[23] = vec2(230, 710);

  // set colors for board
  for(int i = 0; i < 1200; i++){
		colors_for_board[i] = vec3(0.0, 0.0, 0.0);
	}
  // loads shader program
	GLuint shader = InitShader( "vshader.glsl", "fshader.glsl" );
  glUseProgram( shader );

	vPosition = glGetAttribLocation( shader, "vPosition");
  vColor = glGetAttribLocation( shader, "vColor");
  Grid();
  Block();
  win_x = glGetUniformLocation(shader, "win_x");
	win_y = glGetUniformLocation(shader, "win_y");
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
  Board();
  Block();

  glClearColor(0, 0, 0, 0);
	glClear( GL_COLOR_BUFFER_BIT );

 	glUniform1i(win_x, window_size_x);
	glUniform1i(win_y, window_size_y);

  glBindVertexArray(board);
  glDrawArrays( GL_TRIANGLES, 0, 1200);

  glBindVertexArray(block);
  glDrawArrays( GL_TRIANGLES, 0, 24);

  glBindVertexArray(grid);
  glDrawArrays( GL_LINES, 0, 64);

  glFlush();
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
void find_location(){
  glBindVertexArray(board);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_color);
  for(int i = 0; i < 1200; i++){
    if(points_for_board[i] == points_for_block[i]){
        colors_for_board[i] = vec3(0.0, 1.0, 0.0);
      }
  }
  glBufferData(GL_ARRAY_BUFFER, sizeof(colors_for_board), colors_for_board, GL_STATIC_DRAW);
}
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
//==============================================================================
// avoid the window reshaping
void resize(int width, int height) {
    // by ignore the params and set the window size
    glutReshapeWindow(window_size_x, window_size_y);
}
//==============================================================================
int main(int argc, char* argv[]){
	srand(time(NULL));

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA);
  glutInitWindowSize(window_size_x, window_size_y);
	glutInitWindowPosition(850, 200);
  glutCreateWindow("Tetris");
  glewInit();
  init();

  glutDisplayFunc(display);
	glutReshapeFunc(resize);

  glutTimerFunc(500.0, dropDelay, 0);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(keyboardSpecial);

  glutMainLoop();
}
