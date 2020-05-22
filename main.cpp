//
//  Display a rotating cube 
//

#include "Angel.h"

typedef vec4  color4;
typedef vec4  point4;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];
color4 colors[NumVertices];

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
	point4(-0.2, -0.2,  0.2, 1.0),
	point4(-0.2,  0.2,  0.2, 1.0),
	point4(0.2,  0.2,  0.2, 1.0),
	point4(0.2, -0.2,  0.2, 1.0),
	point4(-0.2, -0.2, -0.2, 1.0),
	point4(-0.2,  0.2, -0.2, 1.0),
	point4(0.2,  0.2, -0.2, 1.0),
	point4(0.2, -0.2, -0.2, 1.0)
};

// RGBA olors
color4 vertex_colors[6] = {
	color4(1.0, 0.0, 0.0, 1.0),  // red
	color4(1.0, 1.0, 0.0, 1.0),  // yellow
	color4(0.0, 1.0, 0.0, 1.0),  // green
	color4(0.0, 0.0, 1.0, 1.0),  // blue
	color4(1.0, 0.5, 0.0, 1.0),  // orange
	color4(1.0, 1.0, 1.0, 1.0),  // white
};

// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int  Axis = Xaxis;
GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };

// Model-view and projection matrices uniform location
GLuint  ModelView, Projection;

mat4  model_view, model_view1, model_view2, model_view3, model_view4, model_view5, model_view6, model_view7, model_view8;
mat4 *models[6][4] = {   &model_view1, &model_view2, &model_view4, &model_view3, 
					   	 &model_view2, &model_view5, &model_view8, &model_view4, 
						 &model_view5, &model_view6, &model_view7, &model_view8, 
						 &model_view6, &model_view1, &model_view3, &model_view7 ,
						 &model_view5, &model_view2, &model_view1, &model_view6 ,
						 &model_view7, &model_view3, &model_view4, &model_view8 };

mat4 temp[6][4];



mat4  projection;

bool rotate;
int anglecounter;
int turn = -1;
int square_id = 0;
int ran;
int face;
int random;
//----------------------------------------------------------------------------

// quad generates two triangles for each face and assigns colors
//    to the vertices

int Index = 0;

void
quad(int a, int b, int c, int d,int e)
{
	// Initialize colors

	colors[Index] = vertex_colors[e]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[e]; points[Index] = vertices[b]; Index++;
	colors[Index] = vertex_colors[e]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[e]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[e]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[e]; points[Index] = vertices[d]; Index++;
}

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors
void
colorcube()
{
	quad(1, 0, 3, 2, 0);
	quad(2, 3, 7, 6, 1);
	quad(3, 0, 4, 7, 2);
	quad(6, 5, 1, 2, 3);
	quad(4, 5, 6, 7, 4);
	quad(5, 4, 0, 1, 5);
}

//----------------------------------------------------------------------------

// OpenGL initialization
void
init()
{
	colorcube();
	rotate = true;
	anglecounter = 0;

	random = 0;
	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

	// Load shaders and use the resulting shader program
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	// Retrieve transformation uniform variable locations
	ModelView = glGetUniformLocation(program, "ModelView");
	Projection = glGetUniformLocation(program, "Projection");

	// Set current program object
	glUseProgram(program);
	model_view = identity();
	model_view2 = identity()*Translate(0.2,0.2,0.2);
	model_view1 = identity()*Translate(-0.2, 0.2, 0.2);
	model_view4 = identity()*Translate(0.2, -0.2, 0.2);
	model_view3 = identity()*Translate(-0.2, -0.2, 0.2);
	model_view5 = identity()*Translate(0.2, 0.2, -0.2);
	model_view6 = identity()*Translate(-0.2, 0.2, -0.2);
	model_view8 = identity()*Translate(0.2, -0.2, -0.2);
	model_view7 = identity()*Translate(-0.2, -0.2, -0.2);

	
	

	// Set projection matrix
	projection = Ortho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0); // Ortho(): user-defined function in mat.h
														 //projection = Perspective( 45.0, 1.0, 0.5, 3.0 );
	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);

	// Enable hiddden surface removal
	glEnable(GL_DEPTH_TEST);
	
	std::cout << "--My rubic cube rotates only in the right direction-- " << std::endl;
	std::cout << "W,A,S,D: Rotate camera " << std::endl;
	std::cout << "Left Click: Selects the face" << std::endl;
	std::cout << "Right arrow key: Rotates the selected face" << std::endl;
	std::cout << "R: Randomly rotates the cube" << std::endl;


}

//----------------------------------------------------------------------------

void
display(void)
{
	// Set state variable "clear color" to clear buffer with.
	glClearColor(0.0, 0.7, 1.3, 1.0);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//use this to have rotation around fixed axes
	model_view = Scale(2.0,2.0,2.0);
	
	//When random selected; 
	if (ran >0 && random == 1 && rotate) {
		square_id = face;

		if (square_id == 1) {
			Axis = Zaxis;
			turn = 1;
			rotate = false;
		}
		if (square_id == 2) {
			Axis = Xaxis;
			turn = 1;
			rotate = false;
		}
		if (square_id == 3) {
			Axis = Zaxis;
			turn = 1;
			rotate = false;
		}
		if (square_id == 4) {
			Axis = Xaxis;
			turn = 1;
			rotate = false;
		}
		if (square_id == 5) {
			Axis = Yaxis;
			turn = 1;
			rotate = false;
		}
		if (square_id == 6) {
			Axis = Yaxis;
			turn = 1;
			rotate = false;
		}

	}

	switch (square_id)
	{
		int i;
	case 1:
		for (i = 0; i < 4; i++) {
			*models[0][i] = RotateX(Theta[Xaxis]) *
				RotateY(Theta[Yaxis]) *
				RotateZ(Theta[Zaxis]) * *models[0][i];

			glDrawArrays(GL_TRIANGLES, 0, NumVertices);
			glUniformMatrix4fv(ModelView, 1, GL_TRUE, *models[0][i]);
		}

		break;
	case 2:
		for (i = 0; i < 4; i++) {
			*models[1][i] = RotateX(Theta[Xaxis]) *
				RotateY(Theta[Yaxis]) *
				RotateZ(Theta[Zaxis]) * *models[1][i];

			glDrawArrays(GL_TRIANGLES, 0, NumVertices);
			glUniformMatrix4fv(ModelView, 1, GL_TRUE, *models[1][i]);
		}

		break;
	case 3:
		for (i = 0; i < 4; i++) {
			*models[2][i] = RotateX(Theta[Xaxis]) *
				RotateY(Theta[Yaxis]) *
				RotateZ(Theta[Zaxis]) * *models[2][i];

			glDrawArrays(GL_TRIANGLES, 0, NumVertices);
			glUniformMatrix4fv(ModelView, 1, GL_TRUE, *models[2][i]);
		}

		break;
	case 4:
		for (i = 0; i < 4; i++) {
			*models[3][i] = RotateX(Theta[Xaxis]) *
				RotateY(Theta[Yaxis]) *
				RotateZ(Theta[Zaxis]) * *models[3][i];

			glDrawArrays(GL_TRIANGLES, 0, NumVertices);
			glUniformMatrix4fv(ModelView, 1, GL_TRUE, *models[3][i]);
		}

		break;
	case 5:
		for (i = 0; i < 4; i++) {
			*models[4][i] = RotateX(Theta[Xaxis]) *
				RotateY(Theta[Yaxis]) *
				RotateZ(Theta[Zaxis]) * *models[4][i];

			glDrawArrays(GL_TRIANGLES, 0, NumVertices);
			glUniformMatrix4fv(ModelView, 1, GL_TRUE, *models[4][i]);
		}

		break;
	case 6:
		for (i = 0; i < 4; i++) {
			*models[5][i] = RotateX(Theta[Xaxis]) *
				RotateY(Theta[Yaxis]) *
				RotateZ(Theta[Zaxis]) * *models[5][i];

			glDrawArrays(GL_TRIANGLES, 0, NumVertices);
			glUniformMatrix4fv(ModelView, 1, GL_TRUE, *models[5][i]);
		}

		break;
	
	}

	

	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view1);



	
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view2);
	


	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view3);
	


	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view4);



	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view5); 
	


	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view6);


	
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view7);
	

	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view8);



	glutSwapBuffers();

}

//---------------------------------------------------------------------
//
// reshape
//

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	mat4  projection;
	if (w <= h)
		projection = Ortho(-1.0, 1.0, -1.0 * (GLfloat)h / (GLfloat)w,
			1.0 * (GLfloat)h / (GLfloat)w, -1.0, 1.0);
	else  projection = Ortho(-1.0* (GLfloat)w / (GLfloat)h, 1.0 *
		(GLfloat)w / (GLfloat)h, -1.0, 1.0, -1.0, 1.0);
	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);

}


//----------------------------------------------------------------------------

void
idle(void)
{
	int i;
	int j;
	
	if (anglecounter < 900 && rotate == false) { //angle counter is 900 instead of 90 because degrees increases 
												 //0.1 for smooth and notable turn

		if (turn == 1)
		Theta[Axis] = -0.1;
		if (turn == 0)
		Theta[Axis] = 0.1;

		anglecounter++;

	}
	//update arrays when single rotating process is over
	else if (anglecounter == 900 ) {
		
		if (ran > 0) {
			ran--;
			face = rand() % 6 + 1;
		}

		for (i = 0; i < 6; i++) {
			for (j = 0; j < 4; j++) {
				temp[i][j] = *models[i][j];
			}
		}
		
			if (turn == 1 && square_id == 1 || 2 || 5) {
				for (i = 0; i < 6; i++) {
					for (j = 0; j < 4; j++) {
						if (temp[square_id - 1][0] == temp[i][j]) {
							*models[i][j] = temp[square_id - 1][(0 + 3) % 4];
						}if (temp[square_id - 1][1] == temp[i][j]) {
							*models[i][j] = temp[square_id - 1][(1 + 3) % 4];
						}if (temp[square_id - 1][2] == temp[i][j]) {
							*models[i][j] = temp[square_id - 1][(2 + 3) % 4];
						}if (temp[square_id - 1][3] == temp[i][j]) {
							*models[i][j] = temp[square_id - 1][(3 + 3) % 4];
						}
					}
				}
			}
			else if (turn == 1 && square_id == 4 || 3 || 6) {
				for (i = 0; i < 6; i++) {
					for (j = 0; j < 4; j++) {
						if (temp[square_id - 1][0] == temp[i][j]) {
							*models[i][j] = temp[square_id - 1][(0 - 3) % 4];
						}if (temp[square_id - 1][1] == temp[i][j]) {
							*models[i][j] = temp[square_id - 1][(1 - 3) % 4];
						}if (temp[square_id - 1][2] == temp[i][j]) {
							*models[i][j] = temp[square_id - 1][(2 - 3) % 4];
						}if (temp[square_id - 1][3] == temp[i][j]) {
							*models[i][j] = temp[square_id - 1][(3 - 3) % 4];
						}
					}
				}
			}

		
		
		anglecounter += 1;

	}
	
	//Default position
	else {

		Theta[0] = Theta[1] = Theta[2] = 0.0;
		rotate = true;
		anglecounter = 0;
	}
		glutPostRedisplay();
	
}

//----------------------------------------------------------------------------

void
keyboard(unsigned char key, int x, int y)
{

	if (key == 'R' | key == 'r') {
		random++;
		ran = rand() % 3 + 3;
		face = rand() % 6 + 1;
	}
	
	//////////////////////////////////ROTATE CAMERA/////////////////////////////////////

	if (key == 'W' | key == 'w') {
		mat4 rx = RotateX(1);
		projection = rx*projection;
		glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);

	}
	if (key == 'A' | key == 'a') {
		mat4 ry = RotateY(1);
		projection = ry*projection;
		glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
	}
	if (key == 'S' | key == 's') {
		mat4 rx = RotateX(-1);
		projection = rx*projection;
		glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
	}
	
	if (key == 'D' | key == 'd') {
		mat4 ry = RotateY(-1);
		projection = ry*projection;
		glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
	}


}

//----------------------------------------------------------------------------

void mouse(int button, int state, int x, int y)
{

		if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
			glDrawArrays(GL_TRIANGLES, 0, NumVertices);
			glFlush();
			y = glutGet(GLUT_WINDOW_HEIGHT) - y;

			unsigned char pixel[4];
			glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
			if (pixel[0] == 255 && pixel[1] == 0 && pixel[2] == 0 && rotate) { square_id = 1; std::cout << square_id << std::endl;  }
			else if (pixel[0] == 0 && pixel[1] == 0 && pixel[2] == 255&&rotate) { square_id = 5; std::cout << square_id << std::endl; }
			else if (pixel[0] == 255 && pixel[1] == 255 && pixel[2] == 0 && rotate) { square_id = 2; std::cout << square_id << std::endl;  }
			else if (pixel[0] == 0 && pixel[1] == 255 && pixel[2] == 0 && rotate) { square_id = 6; std::cout << square_id << std::endl; }
			else if (pixel[0] == 255 && pixel[1] == 255 && pixel[2] == 255 && rotate) { square_id = 4; std::cout << square_id << std::endl;  }
			else if (pixel[0] == 255 && pixel[1] == 128 && pixel[2] == 0&&rotate) { square_id = 3; std::cout << square_id << std::endl;  }
			else std::cout << "None" << std::endl;

	

			glutPostRedisplay();
		
	}


}


//----------------------------------------------------------------------------
void ArrowInput(int key, int x, int y)
{

	switch (key)
	{
	
	case GLUT_KEY_RIGHT:

		if (rotate == true) {
			
			    if (square_id == 1) {
					Axis = Zaxis;
					turn = 1;
					rotate = false;
					anglecounter = 0;
				}
				if (square_id == 2) {
					Axis = Xaxis;
					turn = 1;
					rotate = false;
					anglecounter = 0;
				}
				if (square_id == 3) {
					Axis = Zaxis;
					turn = 0;
					rotate = false;
					anglecounter = 0;
				}
				if (square_id == 4) {
					Axis = Xaxis;
					turn = 0;
					rotate = false;
					anglecounter = 0;
				}
				if (square_id == 5) {
					Axis = Yaxis;
					turn = 1;
					rotate = false;
					anglecounter = 0;
				}
				if (square_id == 6) {
					Axis = Yaxis;
					turn = 0;
					rotate = false;
					anglecounter = 0;
				}

			
			}

		
			break;
	
	}
}


int
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutCreateWindow("Color Cube");
	glutSpecialFunc(ArrowInput);

	glewExperimental = GL_TRUE;
	glewInit();
	init();

	glutDisplayFunc(display); // set display callback function
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);


	glutMainLoop();
	return 0;
}
