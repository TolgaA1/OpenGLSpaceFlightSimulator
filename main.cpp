/*
* TO-DO
* PUT PLANETS AND MAKE THE MAP LOOK LIKE SPACE
* ADD LIGHT ONTO PLANE
* ALLOW CAMERA SWITCH ONTO SOMEWHERE IN THE ENVIRONMENT
* FIGURE OUT COLLISIONS
* ALLOW PLANE TO TAKE OFF AND LAND
* ADD MOVING OBJECT TO ENVIRONMENT
* ALLOW COLLISION BETWEEN THAT AND PLANE
* FIGURE OUT DAMAGE
* atmosphere!!!
*/
#include <iostream>
using namespace std;


//--- OpenGL ---
#include "GL\glew.h"
#include "GL\wglew.h"
#pragma comment(lib, "glew32.lib")
//--------------

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_inverse.hpp"
#include "ModelLoader.h"

#include "GL\freeglut.h"

#include "Images\FreeImage.h"

#include "shaders\Shader.h"

#include "Sphere/Sphere.h"

CShader* myShader;  ///shader object test
CShader* myBasicShader;

//MODEL LOADING
#include "3DStruct\threeDModel.h"
#include "Obj\OBJLoader.h"

float amount = 0;
float temp = 0.002f;
	
Sphere planeCollisionSphere;
Sphere boxCollisionSphere;

CThreeDModel venusPlanet, marsPlanet, mercuryPlanet,boxRight, boxFront, testModel;
CThreeDModel plane; //A threeDModel object is needed for each model loaded
COBJLoader objLoader;	//this object is used to load the 3d models.
ModelLoader modelLoader;
///END MODEL LOADING

glm::mat4 ProjectionMatrix; // matrix for the orthographic projection
glm::mat4 ModelViewMatrix;  // matrix for the modelling and viewing

glm::mat4 objectRotation;
glm::vec3 translation = glm::vec3(0.0, 0.0, 0.0);
glm::vec3 pos = glm::vec3(0.0f,0.0f,0.0f); //vector for the position of the object.

//Material properties
float Material_Ambient[4] = {0.1f, 0.1f, 0.1f, 1.0f};
float Material_Diffuse[4] = {0.8f, 0.8f, 0.5f, 1.0f};
float Material_Specular[4] = {0.9f,0.9f,0.8f,1.0f};
float Material_Shininess = 100;

//Light Properties
//float Light_Ambient_And_Diffuse[4] = {0.8f, 0.8f, 0.6f, 1.0f};
float Light_Ambient_And_Diffuse[4] = { 1.0f, 1.f, 1.f, 1.0f };
//float Light_Specular[4] = {1.0f,1.0f,1.0f,1.0f};
float Light_Specular[4] = { 0.1f,0.1f,0.1f,0.1f };
float LightPos[4] = {0.0f, 0.0f, 0.0f, 0.0f};
float LightDir[4] = {0.0f,0.0f,0.0f,1.0f};
//float LightPos[4] = { pos.x, pos.y, pos.z, 0.0f };
//

int	mouse_x=0, mouse_y=0;
bool LeftPressed = false;

bool isCockpitView = true;
bool isThirdpersonView = false;
bool isPlanetView = false;
int screenWidth=600, screenHeight=600;

//booleans to handle when the arrow keys are pressed or released. TEST
bool Left = false;
bool Right = false;
bool Up = false;
bool Down = false;
bool q = false;
bool e = false;
bool o = false;
bool p = false;
bool a = false;
bool d = false;
bool w = false;
bool s = false;
bool v = false;

double minx = 0, miny = 0, minz = 0, maxx = 0, maxy = 0, maxz = 0;

float spin=180;
float speed=0;

//OPENGL FUNCTION PROTOTYPES
void display();				//called in winmain to draw everything to the screen
void reshape(int width, int height);				//called when the window is resized
void init();				//called in winmain when the program starts.
void processKeys();         //called in winmain to process keyboard input
void idle();		//idle function
void updateTransform(float xinc, float yinc, float zinc);

/*************    START OF OPENGL FUNCTIONS   ****************/
void display()									
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(myShader->GetProgramObjID());  // use the shader
	float test = sqrt(((10 - pos.x) * (10 - pos.x)) + ((20 - pos.y) * (20 - pos.y)) + ((0 - pos.z) * (0 - pos.z)));

	//Part for displacement shader.
	amount += temp;
	if(amount > 1.0f || amount < -1.5f)
		temp = -temp;
	//amount = 0;
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "displacement"), amount);

	//Set the projection matrix in the shader
	GLuint projMatLocation = glGetUniformLocation(myShader->GetProgramObjID(), "ProjectionMatrix");  
	glUniformMatrix4fv(projMatLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);

	glm::mat4 viewingMatrix = glm::mat4(1.0f);
	
	//translation and rotation for view
	//viewingMatrix = glm::translate(glm::mat4(1.0), glm::vec3(pos.x, pos.y, pos.z));

	//apply a rotation to the view
	//static float angle = 0.0f;
	//angle += 0.01;
	//viewingMatrix = glm::rotate(viewingMatrix, angle, glm::vec3(1.0f, 0.0f, 0.0));

	pos.x += objectRotation[2][0]* speed;
	pos.y += objectRotation[2][1]* speed;
	pos.z += objectRotation[2][2]* speed;

	//Camera view toggling.
	if (isCockpitView)
	{
		viewingMatrix = glm::lookAt(glm::vec3(pos.x, pos.y, pos.z), glm::vec3(pos.x + objectRotation[2][0], pos.y + objectRotation[2][1], pos.z + objectRotation[2][2]), glm::vec3(objectRotation[1][0], objectRotation[1][1], objectRotation[1][2]));
	}
	else if (isThirdpersonView)
	{
		glm::vec3 direction = glm::normalize(glm::vec3(objectRotation[2][0], objectRotation[2][1], objectRotation[2][2]));
		viewingMatrix = glm::lookAt(glm::vec3(pos.x - direction.x * 15,pos.y - direction.y * 15,pos.z- direction.z *15), glm::vec3(pos.x, pos.y, pos.z), glm::vec3(objectRotation[1][0], objectRotation[1][1], objectRotation[1][2]));
	}

	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ViewMatrix"), 1, GL_FALSE, &viewingMatrix[0][0]);

	
	LightPos[0] = pos.x - objectRotation[2][0]*5;
	LightPos[1] = pos.y - objectRotation[2][1]*5;
	LightPos[2] = pos.z - objectRotation[2][2]*5;
	


	//Passing variables onto shader
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "LightDir"), 1, LightDir);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "LightPos"), 1, LightPos);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "light_ambient"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "light_diffuse"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "light_specular"), 1, Light_Specular);

	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "material_ambient"), 1, Material_Ambient);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "material_diffuse"), 1, Material_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "material_specular"), 1, Material_Specular);
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "material_shininess"), Material_Shininess);
	//0.000000000005
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "constantAttenuation"), 0.0000000005f);
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "linearAttenuation"), 0.0000000005f);
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "quadraticAttenuation"), 0.0000000005f);

	//Spaceship rendering
	glm::mat4 modelmatrix =  glm::translate(glm::mat4(1.0f), pos);
	ModelViewMatrix = viewingMatrix * modelmatrix * objectRotation;
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
	
	plane.DrawElementsUsingVBO(myShader);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//planeCollisionSphere.render();
	

	//Switch to basic shader to draw the lines for the bounding boxes
	glUseProgram(myBasicShader->GetProgramObjID());
	projMatLocation = glGetUniformLocation(myBasicShader->GetProgramObjID(), "ProjectionMatrix");
	glUniformMatrix4fv(projMatLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);



	//Render the scene objects
	//switch back to the shader for textures and lighting on the objects.
	glUseProgram(myShader->GetProgramObjID());  // use the shader
	ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(0, 0, 0));
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	venusPlanet.DrawElementsUsingVBO(myShader);
	marsPlanet.DrawElementsUsingVBO(myShader);
	mercuryPlanet.DrawElementsUsingVBO(myShader);
	testModel.DrawElementsUsingVBO(myShader);
	venusPlanet.CalcBoundingBox(minx,miny,minz,maxx,maxy,maxz);

	glUseProgram(myBasicShader->GetProgramObjID());  // use the shader

	ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(0, 0, 0));
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myBasicShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	//boxRight.DrawElementsUsingVBO(myBasicShader);


	/*
	std::cout << test << std::endl;
	if (((10 - pos.x)*(10 - pos.x)) + ((20 -pos.y)*(20-pos.y)) +((0 - pos.z)*(0-pos.z)) < (6 + 8) * (6 + 8)) {
		std::cout << "COLLISION" << std::endl;
	}
	else
	{

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		boxCollisionSphere.render();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	//boxFront.drawElementsUsingVBO(myShader);
	
	ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(-6, 10, 4));

	if ((minx < -6 && maxx > -6 || miny < 10 && maxy > 10 || minz < 4 && maxz > 4)) {
		std::cout << "COLLISION" << std::endl;
	}
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	boxCollisionSphere.render();
	*/
	glFlush();
	glutSwapBuffers();
}

void reshape(int width, int height)		// Resize the OpenGL window
{
	screenWidth=width; screenHeight = height;           // to ensure the mouse coordinates match 
														// we will use these values to set the coordinate system

	glViewport(0,0,width,height);						// Reset The Current Viewport

	//Set the projection matrix
	ProjectionMatrix = glm::perspective(glm::radians(60.0f), (GLfloat)screenWidth/(GLfloat)screenHeight, 1.0f, 20000000.0f);
}
void init()
{
	glClearColor(1.0, 1.0, 1.0, 0.0);						//sets the clear colour to yellow
											//glClear(GL_COLOR_BUFFER_BIT) in the display function
											//will clear the buffer to this colour
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);


	myShader = new CShader();
	//if(!myShader->CreateShaderProgram("BasicView", "glslfiles/basicTransformationsWithDisplacement.vert", "glslfiles/basicTransformationsWithDisplacement.frag"))
	if (!myShader->CreateShaderProgram("BasicView", "glslfiles/basicTransformations.vert", "glslfiles/basicTransformations.frag"))
	{
		std::cout << "failed to load shader" << std::endl;
	}

	myBasicShader = new CShader();
	if (!myBasicShader->CreateShaderProgram("Basic", "glslfiles/basic.vert", "glslfiles/basic.frag"))
	{
		std::cout << "failed to load shader" << std::endl;
	}

	glUseProgram(myShader->GetProgramObjID());  // use the shader

	glEnable(GL_TEXTURE_2D);

	objectRotation = glm::mat4(1.0f);

	modelLoader.initModel("TestModels/Sample_Ship.obj", plane,myShader, true);
	modelLoader.initModel("TestModels/Venus_1K.obj", venusPlanet, myShader, false);
	modelLoader.initModel("TestModels/mars.obj", marsPlanet, myShader, false);
	modelLoader.initModel("TestModels/Mercury_1K.obj", mercuryPlanet, myShader, false);
	modelLoader.initModel("TestModels/boxLeft.obj", testModel, myShader, true);
	planeCollisionSphere.setCentre(0, 0, 0);
	planeCollisionSphere.setRadius(1);
	planeCollisionSphere.constructGeometry(myBasicShader, 16);

	boxCollisionSphere.setCentre(0, 0, 0);
	boxCollisionSphere.setRadius(1);
	boxCollisionSphere.constructGeometry(myBasicShader, 16);

	glUseProgram(myBasicShader->GetProgramObjID());  
	glEnable(GL_TEXTURE_2D);
	modelLoader.initModel("TestModels/skybox.obj", boxRight, myBasicShader, false);


}

void special(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		Left = true;
		break;
	case GLUT_KEY_RIGHT:
		Right = true;
		break;
	case GLUT_KEY_UP:
		Up = true;
		break;
	case GLUT_KEY_DOWN:
		Down = true;
		break;
	}
}
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'q':
		q = true;
		break;
	case 'e':
		e = true;
		break;
	case 'o':
		o = true;
		break;
	case 'p':
		p = true;
		break;
	case 'a':
		a = true;
		break;
	case 'd':
		d = true;
		break;
	case 's':
		s = true;
		break;
	case 'v':
		isThirdpersonView = !isThirdpersonView;
		isCockpitView = !isCockpitView;
		break;
	case 'w':
		w = true;
		break;
	}
}
void keyboardUp(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'q':
		q = false;
		break;
	case 'e':
		e = false;
		break;
	case 'o':
		o = false;
		break;
	case 'p':
		p = false;
		break;
	case 'a':
		a = false;
		break;
	case 'd':
		d = false;
		break;
	case 's':
		s = false;
		break;
	case 'w':
		w = false;
		break;
	}
}
void specialUp(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		Left = false;
		break;
	case GLUT_KEY_RIGHT:
		Right = false;
		break;
	case GLUT_KEY_UP:
		Up = false;
		break;
	case GLUT_KEY_DOWN:
		Down = false;
		break;
	case GLUT_KEY_HOME:
		q = false;
		break;
	case GLUT_KEY_END:
		e = false;
		break;		
	}
}

void processKeys()
{
	float spinXinc = 0.0f, spinYinc = 0.0f, spinZinc = 0.0f;
	//used to be 0.015
	if (Left)
	{
		spinYinc = 0.00015f;

	}
	if (Right)
	{
		spinYinc = -0.00015f;
	}
	if (Up)
	{
		spinXinc = 0.00015f;
	}
	if (Down)
	{
		spinXinc = -0.00015f;
	}
	if (q)
	{
		spinZinc = 0.0005f;
	}
	if (e)
	{
		spinZinc = -0.0005f;
	}
	if (o)
	{
		pos.y += 0.2f;
	}
	if (p)
	{
		pos.y -= 0.2f;
	}
	if (a)
	{
		pos.x -= 0.2f;
	}
	//used to be 0.105
	if (w)
	{
		speed += 0.00105f;
	}
	if (s)
	{
		speed -= 0.00105f;
	}

	updateTransform(spinXinc, spinYinc, spinZinc);
}


void updateTransform(float xinc, float yinc, float zinc)
{
	objectRotation = glm::rotate(objectRotation, xinc, glm::vec3(1,0,0));
	objectRotation = glm::rotate(objectRotation, yinc, glm::vec3(0,1,0));
	objectRotation= glm::rotate(objectRotation, zinc, glm::vec3(0,0,1));

}

void idle()
{
	spin += speed;
	if(spin > 360)
		spin = 0;

	processKeys();

	glutPostRedisplay();
}
/**************** END OPENGL FUNCTIONS *************************/

int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(screenWidth, screenHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Spaceship Simulator");

	//This initialises glew - it must be called after the window is created.
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cout << " GLEW ERROR" << endl;
	}

	//Check the OpenGL version being used
	int OpenGLVersion[2];
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);
	cout << OpenGLVersion[0] << " " << OpenGLVersion[1] << endl;


	//initialise the objects for rendering
	init();

	glutReshapeFunc(reshape);
	//specify which function will be called to refresh the screen.
	glutDisplayFunc(display);

	glutSpecialFunc(special);
	glutSpecialUpFunc(specialUp);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);

	glutIdleFunc(idle);

	//starts the main loop. Program loops and calls callback functions as appropriate.
	glutMainLoop();

	return 0;
}
