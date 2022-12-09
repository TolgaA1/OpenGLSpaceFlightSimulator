/*
* TO-DO
* Maybe make pieces fall off when damaged? - slighty done
* When knocked back, add a bit of rotation -
* Add another camera view into the environment - done
* Fix landing -
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

float amount = 0.001;
float temp = 0.002f;
	
Sphere planeCollisionSphere, boxCollisionSphere, marsCollisionSphere, venusCollisionSphere, mercuryCollisionSphere, AIShipCollisionSphere, spaceShipLandingSphere, sateliteCollisionSphere, frontPoint,backPoint;

CThreeDModel venusPlanet, marsPlanet, mercuryPlanet,boxRight, boxFront, AIShip,shipFrontDmg,shipBackDmg,shipFrontBackDmg,shipPiece, satelite, sun;
CThreeDModel plane; //A threeDModel object is needed for each model loaded
COBJLoader objLoader;	//this object is used to load the 3d models.
ModelLoader modelLoader;
///END MODEL LOADING

glm::mat4 ProjectionMatrix; // matrix for the orthographic projection
glm::mat4 ModelViewMatrix;  // matrix for the modelling and viewing

glm::mat4 objectRotation,AIShipRotation;
glm::vec3 translation = glm::vec3(0.0, 0.0, 0.0);
glm::vec3 spaceShipLandingSpherePos(0.0f, 0.0f, 0.0f);
glm::vec3 pos = glm::vec3(0.0f,0.0f,0.0f); //vector for the position of the object.
glm::vec3 tempPos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 AIPos = glm::vec3{ 0.0f,10.0f,0.0f };
glm::vec3 frontPointPos = glm::vec3{ 0.0f,0.0f,0.0f };
glm::vec3 backPointPos = glm::vec3{ 0.0f,0.0f,0.0f };
glm::vec3 piecePos = glm::vec3{ 0.0f,0.0f,0.0f };
//Material properties
float Material_Ambient[4] = {0.07f, 0.07f, 0.07f, 1.0f};
float Material_Diffuse[4] = {0.8f, 0.8f, 0.8f, 1.0f};
float Material_Specular[4] = {0.9f,0.9f,0.9f,1.0f};
float Material_Shininess = 100;

//Light Properties
float Light_Ambient_And_Diffuse[4] = { 1.0f, 1.f, 1.f, 1.0f };
float Light_Ambient_And_Diffuse_Spaceship[4] = { 1.0f, 0.f, 0.f, 1.0f };
//float Light_Specular[4] = {1.0f,1.0f,1.0f,1.0f};
float Light_Specular[4] = { 0.1f,0.1f,0.1f,0.1f };
float LightPos[4] = {0.0f,0.0f,0.0f,1.0f};
float LightPos2[4] = { -137325.0f, 22617.0f, 300889.4f, 0.0f };


int	mouse_x=0, mouse_y=0;
bool LeftPressed = false;
bool frontDamage, backDamage, frontBackDamage;
bool isCockpitView = true;
bool isThirdpersonView = false;
bool isEnvironmentView = false;
bool isPlanetView = false;
bool isKnockedBack = false;
bool isLanded = false;
bool successfullyLanded = false;
bool landingMode = false;
bool pieceDropped = false;
bool slowMode = false;

float pieceTimer = 10.0f;
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
bool r = false;

double minx = 0, miny = 0, minz = 0, maxx = 0, maxy = 0, maxz = 0;
float AIShipPosX, AIShipPosY, AIShipPosZ;
float spin=180;
float speed=0;
float ySpeed = 0;

//OPENGL FUNCTION PROTOTYPES
void display();				//called in winmain to draw everything to the screen
void reshape(int width, int height);				//called when the window is resized
void init();				//called in winmain when the program starts.
void processKeys();         //called in winmain to process keyboard input
void idle();		//idle function
void updateTransform(float xinc, float yinc, float zinc);
void damageVisualManager();
void collisionManager();

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

	pos.x += objectRotation[1][0] * ySpeed;
	pos.y += objectRotation[1][1] * ySpeed;
	pos.z += objectRotation[1][2] * ySpeed;
	std::cout << pos.x << std::endl;
	//Camera view toggling.
	if (isEnvironmentView)
	{
		//viewingMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(105104, 25994.3f, 46772.4));
		viewingMatrix = glm::lookAt(glm::vec3(50, 4, 30), pos, glm::vec3(0,1,0));
	}
	else
	{
		if (isCockpitView)
		{
			viewingMatrix = glm::lookAt(glm::vec3(pos.x, pos.y, pos.z), glm::vec3(pos.x + objectRotation[2][0], pos.y + objectRotation[2][1], pos.z + objectRotation[2][2]), glm::vec3(objectRotation[1][0], objectRotation[1][1], objectRotation[1][2]));
		}
		else if (isThirdpersonView)
		{
			glm::vec3 direction = glm::normalize(glm::vec3(objectRotation[2][0], objectRotation[2][1], objectRotation[2][2]));
			viewingMatrix = glm::lookAt(glm::vec3(pos.x - direction.x * 15, pos.y - direction.y * 15, pos.z - direction.z * 15), glm::vec3(pos.x, pos.y, pos.z), glm::vec3(objectRotation[1][0], objectRotation[1][1], objectRotation[1][2]));
		}
	}

	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ViewMatrix"), 1, GL_FALSE, &viewingMatrix[0][0]);

	
	//LightPos[0] = pos.x - objectRotation[2][0]*5;
	//LightPos[1] = pos.y - objectRotation[2][1]*5;
	//LightPos[2] = pos.z - objectRotation[2][2]*5;
	
	LightPos[0] = pos.x;
	LightPos[1] = pos.y;
	LightPos[2] = pos.z;
	float direction[4] = { objectRotation[2][0], objectRotation[2][1], objectRotation[2][2],1};
	//Passing variables onto shader
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "LightPos2"), 1, LightPos2);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "LightDir"), 1, direction);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "LightPos"), 1, LightPos);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "light_ambient"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "light_diffuse"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "light_diffuse_spaceship"), 1, Light_Ambient_And_Diffuse_Spaceship);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "light_specular"), 1, Light_Specular);

	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "material_ambient"), 1, Material_Ambient);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "material_diffuse"), 1, Material_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "material_specular"), 1, Material_Specular);
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "material_shininess"), Material_Shininess);
	//0.000000000005
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "constantAttenuation"), 0.000000005f);
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "linearAttenuation"), 0.000000005f);
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "quadraticAttenuation"), 0.000000005f);
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "constantAttenuation2"), 0.000000005f);
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "linearAttenuation2"), 0.000000005f);
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "quadraticAttenuation2"), 0.000000005f);

	//Spaceship rendering
	glm::mat4 modelmatrix =  glm::translate(glm::mat4(1.0f), pos);
	ModelViewMatrix = viewingMatrix * modelmatrix * objectRotation;
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
	
	damageVisualManager();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Spaceship landing collision sphere rendering




	//SPACE SHIP COLLISION SPHERES

	spaceShipLandingSpherePos.x = pos.x - objectRotation[1][0] * 10.5f;
	spaceShipLandingSpherePos.y = pos.y - objectRotation[1][1] * 10.5f;
	spaceShipLandingSpherePos.z = pos.z - objectRotation[1][2] * 10.5f;

	ModelViewMatrix = glm::translate(viewingMatrix, spaceShipLandingSpherePos);
	//ModelViewMatrix = viewingMatrix * modelmatrix * objectRotation;
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
	spaceShipLandingSphere.render();

	frontPointPos.x = pos.x - objectRotation[2][0] * -4.5f;
	frontPointPos.y = pos.y - objectRotation[2][1] * -4.5f;
	frontPointPos.z = pos.z - objectRotation[2][2] * -4.5f;

	ModelViewMatrix = glm::translate(viewingMatrix, frontPointPos);
	//ModelViewMatrix = viewingMatrix * modelmatrix * objectRotation;
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
	//frontPoint.render();

	backPointPos.x = pos.x - objectRotation[2][0] * 4.5f;
	backPointPos.y = pos.y - objectRotation[2][1] * 4.5f;
	backPointPos.z = pos.z - objectRotation[2][2] * 4.5f;

	ModelViewMatrix = glm::translate(viewingMatrix, backPointPos);
	//ModelViewMatrix = viewingMatrix * modelmatrix * objectRotation;
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
	//backPoint.render();

	ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(50, 1, 30));
	//ModelViewMatrix = viewingMatrix * modelmatrix * objectRotation;
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
	satelite.DrawElementsUsingVBO(myShader);
	//sateliteCollisionSphere.render();



	
	//if (glm::length(frontPointPos - glm::vec3(105104, 24994.3, 46772.4)) < (frontPoint.getRadius() + marsCollisionSphere.getRadius()))



	//-------------------------------------
	// 
	// 
	//planeCollisionSphere.render();



	plane.CalcBoundingBox(minx, miny, minz, maxx, maxy, maxz);
	//Switch to basic shader to draw the lines for the bounding boxes
	glUseProgram(myBasicShader->GetProgramObjID());
	projMatLocation = glGetUniformLocation(myBasicShader->GetProgramObjID(), "ProjectionMatrix");
	glUniformMatrix4fv(projMatLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);



	//Render the scene objects
	//switch back to the shader for textures and lighting on the objects.
	glUseProgram(myShader->GetProgramObjID());  // use the shader


	//testModel.DrawElementsUsingVBO(myShader);
	//testModel.CalcBoundingBox(minx, miny, minz, maxx, maxy, maxz);
	//venusPlanet.CalcBoundingBox(minx,miny,minz,maxx,maxy,maxz);
	
	//PLANET CENTERING
	ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(105104, 24994.3f, 46772.4));
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	marsPlanet.DrawElementsUsingVBO(myShader);
	//marsCollisionSphere.render();


	ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(92783, 4154, 609174));
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	mercuryPlanet.DrawElementsUsingVBO(myShader);
	//mercuryCollisionSphere.render();

	ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(-305416, -18431.8f, 7792.3f));
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	venusPlanet.DrawElementsUsingVBO(myShader);
	//venusCollisionSphere.render();

	glUseProgram(myBasicShader->GetProgramObjID());  // use the shader

	ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(0, 0, 0));
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myBasicShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	boxRight.DrawElementsUsingVBO(myBasicShader);

	//LIGHT SOURCE
	ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(LightPos2[0], LightPos2[1], LightPos2[2]));
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myBasicShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	sun.DrawElementsUsingVBO(myBasicShader);



	//PLANET COLLISION BOX SPHERES
	ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(105104, 24994.3f, 46772.4));
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myBasicShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);




	glUseProgram(myShader->GetProgramObjID());
	
	if (backDamage)
	{

		ModelViewMatrix = glm::translate(viewingMatrix, piecePos);
		//ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(pos.x - objectRotation[2][0] * -4.5f, pos.y - objectRotation[2][1] * -4.5f, pos.z - objectRotation[2][2] * -4.5f));
		normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
		glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
		shipPiece.DrawElementsUsingVBO(myShader);
	}
	else
	{
		piecePos.x = pos.x - objectRotation[0][0] * -4.5f;
		piecePos.y = pos.y - objectRotation[0][1] * -4.5f;
		piecePos.z = pos.z - objectRotation[0][2] * -4.5f;
		/*
		* 		
		*/

	}

	minx += pos.x;
	maxx += pos.x;
	miny += pos.y;
	maxy += pos.y;
	minz += pos.z;
	maxz += pos.z;
	


	/*
	* 	if (((10 - pos.x)*(10 - pos.x)) + ((20 -pos.y)*(20-pos.y)) +((0 - pos.z)*(0-pos.z)) < (6 + 8) * (6 + 8)) {
		std::cout << "COLLISIONk" << std::endl;
	}
	else
	{

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		boxCollisionSphere.render();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	*/

	//boxFront.drawElementsUsingVBO(myShader);
	/*
	* 	double x, y, z;
	x = 0;
	y = 0.4;
	z = 0.9;
	*/
	double x, y, z;
	x = 105104;
	y = 24994.3f;
	z = 46772.4;
	glm::vec4 bob = glm::inverse(modelmatrix * objectRotation) * glm::vec4(x, y, z, 1);
	//glm::vec4 bob = glm::vec4(x,y,z,1) * glm::inverse(modelmatrix*objectRotation);

	ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(x, y, z));
	/*
	
	
		double halfWidthX = maxx - minx;
	double halfWidthY = maxy - miny;
	double halfWidthZ = maxz - minz;
	glm::vec3 aabb_half(halfWidthX, halfWidthY, halfWidthZ);
	glm::vec3 difference = glm::vec3(x,y,z) - pos;
	glm::vec3 clamped = glm::clamp(difference, -aabb_half, aabb_half);
	glm::vec3 closest = pos + clamped;
	difference = closest - glm::vec3(x, y, z);
	if (glm::length(difference) < marsCollisionSphere.getRadius())
	{
		std::cout << "COLLISIONBOUNDINGSPEHERE" << std::endl;
	}
	
	
	*/

	
	/*
		if (((minx < x && maxx > x) && (miny < y && maxy> y) && (minz < z && maxz > z))) {
		std::cout << "COLLISIONBOUNDING" << std::endl;
	}
	*/


	/*
	* 	if (plane.isColliding(glm::vec3(bob))) {
		std::cout << "COLLISION" << std::endl;
	}
	*/
	//if ti works add distance check
	/*
	

	*/
	//if speed to fast make spaceship bounce and dmg spaceship
		//sphere to sphere collision

	collisionManager();

	double xe = spaceShipLandingSpherePos.x - 105104;
	double ye = spaceShipLandingSpherePos.y - 24994.3;
	double ze = spaceShipLandingSpherePos.z - 46772.4;
	double e = pow(xe, 2) + pow(ye, 2) + pow(ze, 2);



	//if (glm::length(spaceShipLandingSpherePos - glm::vec3(105104, 24994.3, 46772.4)) < (spaceShipLandingSphere.getRadius() + marsCollisionSphere.getRadius()))

	//(((spaceShipLandingSpherePos.x - 105104) * (spaceShipLandingSpherePos.x - 105104)) + ((spaceShipLandingSpherePos.y - 24994.3) * (spaceShipLandingSpherePos.y - 24994.3)) + ((spaceShipLandingSpherePos.z - 46772.4) * (spaceShipLandingSpherePos.z - 46772.4)) < (spaceShipLandingSphere.getRadius() + marsCollisionSphere.getRadius()) * (spaceShipLandingSphere.getRadius() + marsCollisionSphere.getRadius()))


	if (isLanded)
	{
		ySpeed = 0;
		speed = 0;
		isKnockedBack = false;
		if (!successfullyLanded)
		{
			pos.y -= 60;
			successfullyLanded = true;
		}

	}
	if (isKnockedBack)
	{

		if (speed > 1)
		{
			speed -= 0.01f;
		}
		else if (speed < -1)
		{
			speed += 0.01f;
		}
		else
		{
			isKnockedBack = false;
		}
	}


	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	boxCollisionSphere.render();


	/*
	* FOR ROTATION USE AS REFERENCE
	* glm::mat4 AImodelmatrix =  glm::translate(glm::mat4(1.0f), AIShipPosX,AIShipPosY,AIShipPosZ);
	ModelViewMatrix = viewingMatrix * AImodelmatrix * AIShipRotation;
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
	*/

	/*
		AIShipRotation = glm::rotate(AIShipRotation, 0.0015f, glm::vec3(0, 0, 1));
	AIShipPosX = 1.1f;
	AIShipPosY = 0.4f;
	AIShipPosZ += 0.0005f;
	AIPos.x = 0.01f*AIShipRotation[2][0];
	AIPos.y = 0.01f*AIShipRotation[2][1];
	AIPos.z = 0.01f*AIShipRotation[2][2];
	glm::mat4 AImodelmatrix = glm::translate(glm::mat4(1.0f), AIPos);

	ModelViewMatrix = viewingMatrix * AImodelmatrix * AIShipRotation;
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
	AIShip.DrawElementsUsingVBO(myShader);
	*/


	/*
	* 	pos.x += objectRotation[2][0] * speed;
	pos.y += objectRotation[2][1] * speed;
	pos.z += objectRotation[2][2] * speed;
	* 	//ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(AIShipPosX, AIShipPosY, AIShipPosZ));
	AIShipRotation = glm::rotate(objectRotation, 0.0015f, glm::vec3(1, 0, 0));
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	AIShip.DrawElementsUsingVBO(myShader);
	*/

	
	AIShipPosX = 1.1f;
	AIShipPosY = 0.4f;
	AIShipPosZ += 0.0005f;
	//used to be 0.0105f
	AIPos.x += 0.00105f * AIShipRotation[2][0];
	AIPos.y += 0.00105f * AIShipRotation[2][1];
	AIPos.z += 0.00105f * AIShipRotation[2][2];
	//used to be 0.00015
	//FIX SHIP ROTATING WIH PLAYER
	AIShipRotation = glm::rotate(AIShipRotation, 0.000075f, glm::vec3(0, 1, 0));
	glm::mat4 AImodelmatrix = glm::translate(glm::mat4(1.0f), AIPos);

	ModelViewMatrix = viewingMatrix * AImodelmatrix * AIShipRotation;
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
	AIShip.DrawElementsUsingVBO(myShader);
	//AIShipCollisionSphere.render();

	glFlush();
	glutSwapBuffers();
}

void collisionManager()
{
	glm::vec3 marsPos = glm::vec3(105104, 24994.3f, 46772.4);

	glm::vec3 venusPos = glm::vec3(-305416, -18431.8f, 7792.3f);
	glm::vec3 mercuryPos = glm::vec3(92783, 4154, 609174);

	if (speed > 0)
	{
		if (glm::length(frontPointPos - marsPos) < (frontPoint.getRadius() + marsCollisionSphere.getRadius()))
		{

			std::cout << "FRONT HIT" << std::endl;
			frontDamage = true;
		}

		else if (glm::length(frontPointPos - mercuryPos) < (frontPoint.getRadius() + mercuryCollisionSphere.getRadius()))
		{

			std::cout << "FRONT HIT" << std::endl;
			frontDamage = true;
		}

		else if (glm::length(frontPointPos - venusPos) < (frontPoint.getRadius() + venusCollisionSphere.getRadius()))
		{

			std::cout << "FRONT HIT" << std::endl;
			frontDamage = true;
		}

		else if (glm::length(frontPointPos - AIPos) < (frontPoint.getRadius() + AIShipCollisionSphere.getRadius()))
		{

			std::cout << "FRONT HIT" << std::endl;
			frontDamage = true;
		}

		else if (glm::length(frontPointPos - glm::vec3(50, 1, 30)) < (frontPoint.getRadius() + sateliteCollisionSphere.getRadius()))
		{

			std::cout << "FRONT HIT" << std::endl;
			frontDamage = true;
		}
	}
	else {
		if (glm::length(backPointPos - marsPos) < (backPoint.getRadius() + marsCollisionSphere.getRadius()))
		{
			std::cout << "BACK HIT" << std::endl;
			backDamage = true;
		}

		else if (glm::length(backPointPos - mercuryPos) < (backPoint.getRadius() + mercuryCollisionSphere.getRadius()))
		{
			std::cout << "BACK HIT" << std::endl;
			backDamage = true;
		}

		else if (glm::length(backPointPos - venusPos) < (backPoint.getRadius() + venusCollisionSphere.getRadius()))
		{
			std::cout << "BACK HIT" << std::endl;
			backDamage = true;
		}

		else if (glm::length(backPointPos - AIPos) < (backPoint.getRadius() + AIShipCollisionSphere.getRadius()))
		{

			std::cout << "BACK HIT" << std::endl;
			backDamage = true;
		}
		else if (glm::length(backPointPos - glm::vec3(50, 1, 30)) < (backPoint.getRadius() + sateliteCollisionSphere.getRadius()))
		{

			std::cout << "BACK HIT" << std::endl;
			backDamage = true;
		}

	}

	if (plane.isColliding(marsCollisionSphere.getRadius(), marsPos, pos, speed) && !isLanded)
	{


		std::cout << "COLLISIONBOUNDINGOCTREE" << std::endl;
		pos = tempPos;
		speed = -speed;
		objectRotation = glm::rotate(objectRotation, 0.045f, glm::vec3(0, 0, 1));
		//ySpeed = -ySpeed;
		isKnockedBack = true;

		//pos = tempPos;
	}
	else
	{
		tempPos = pos;
		if (glm::length(spaceShipLandingSpherePos - marsPos) < (spaceShipLandingSphere.getRadius() + marsCollisionSphere.getRadius()))
		{
			std::cout << "COLLISIONkzzz" << std::endl;
			isLanded = true;
		}
	}

	if (plane.isColliding(sateliteCollisionSphere.getRadius(), glm::vec3(50, 1, 30), pos, speed) && !isLanded)
	{


		std::cout << "COLLISIONBOUNDINGOCTREE" << std::endl;
		speed = -speed;
		//ySpeed = -ySpeed;
		isKnockedBack = true;
	}

	if (plane.isColliding(mercuryCollisionSphere.getRadius(), mercuryPos, pos, speed) && !isLanded)
	{


		std::cout << "COLLISIONBOUNDINGOCTREE" << std::endl;
		pos = tempPos;
		speed = -speed;
		//ySpeed = -ySpeed;
		isKnockedBack = true;

		//pos = tempPos;
	}
	else
	{
		tempPos = pos;
		if (glm::length(spaceShipLandingSpherePos - mercuryPos) < (spaceShipLandingSphere.getRadius() + mercuryCollisionSphere.getRadius()))
		{
			std::cout << "COLLISIONkzzz" << std::endl;
			isLanded = true;
		}
	}

	if (plane.isColliding(venusCollisionSphere.getRadius(), venusPos, pos, speed) && !isLanded)
	{


		std::cout << "COLLISIONBOUNDINGOCTREE" << std::endl;
		pos = tempPos;
		speed = -speed;
		//ySpeed = -ySpeed;
		isKnockedBack = true;

		//pos = tempPos;
	}
	else
	{
		tempPos = pos;
		if (glm::length(spaceShipLandingSpherePos - venusPos) < (spaceShipLandingSphere.getRadius() + venusCollisionSphere.getRadius()))
		{
			std::cout << "COLLISIONkzzz" << std::endl;
			isLanded = true;
		}
	}

	if (plane.isColliding(AIShipCollisionSphere.getRadius(), AIPos, pos, speed) && !isLanded)
	{


		std::cout << "COLLISIONBOUNDINGOCTREE WITH OTHER SPACE SHIP" << std::endl;
		//pos = tempPos;
		speed = -speed;
		//ySpeed = -ySpeed;
		isKnockedBack = true;

		//pos = tempPos;
	}


}
void damageVisualManager()
{
	if (frontDamage && backDamage)
	{
		shipFrontBackDmg.DrawElementsUsingVBO(myShader);

	}
	else if (frontDamage)
	{

		shipFrontDmg.DrawElementsUsingVBO(myShader);
	}
	else if (backDamage)
	{
		shipBackDmg.DrawElementsUsingVBO(myShader);
	}
	else
	{

		plane.DrawElementsUsingVBO(myShader);
	}
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
	AIShipRotation = glm::mat4(1.0f);
	modelLoader.initModel("TestModels/otherSpaceship.obj", plane,myShader, true);
	modelLoader.initModel("TestModels/otherSpaceshipFrontBackDmg.obj", shipFrontBackDmg, myShader, true);
	modelLoader.initModel("TestModels/otherSpaceshipBackDmg.obj", shipBackDmg, myShader, true);
	modelLoader.initModel("TestModels/otherShipPiece.obj", shipPiece, myShader, true);
	modelLoader.initModel("TestModels/otherSpaceshipFrontDmg.obj", shipFrontDmg, myShader, true);
	modelLoader.initModel("TestModels/Venus_1K.obj", venusPlanet, myShader, true);
	modelLoader.initModel("TestModels/mars.obj", marsPlanet, myShader, true);
	modelLoader.initModel("TestModels/Mercury_1K.obj", mercuryPlanet, myShader, false);
	modelLoader.initModel("TestModels/Sample_Ship.obj", AIShip, myShader, true);
	modelLoader.initModel("TestModels/boxRight.obj", boxFront, myShader, true);
	modelLoader.initModel("TestModels/UHFSatcom.obj", satelite, myShader, true);
	modelLoader.initModel("TestModels/sun.obj", sun, myShader, true);

	planeCollisionSphere.setCentre(0, 0, 0);
	planeCollisionSphere.setRadius(1);
	planeCollisionSphere.constructGeometry(myBasicShader, 16);
	
	marsCollisionSphere.setCentre(0, 0, 0);
	marsCollisionSphere.setRadius(27020);
	marsCollisionSphere.constructGeometry(myBasicShader, 50);

	venusCollisionSphere.setCentre(0, 0, 0);
	venusCollisionSphere.setRadius(29020);
	venusCollisionSphere.constructGeometry(myBasicShader, 50);

	mercuryCollisionSphere.setCentre(0, 0, 0);
	mercuryCollisionSphere.setRadius(16020);
	mercuryCollisionSphere.constructGeometry(myBasicShader, 50);

	boxCollisionSphere.setCentre(0, 0, 0);
	boxCollisionSphere.setRadius(0.05f);
	boxCollisionSphere.constructGeometry(myBasicShader, 16);

	spaceShipLandingSphere.setCentre(0, 0, 0);
	spaceShipLandingSphere.setRadius(3.0f);
	spaceShipLandingSphere.constructGeometry(myBasicShader, 16);


	AIShipCollisionSphere.setCentre(0, 0, 0);
	AIShipCollisionSphere.setRadius(8.0f);
	AIShipCollisionSphere.constructGeometry(myBasicShader, 16);

	sateliteCollisionSphere.setCentre(0, 0, 0);
	sateliteCollisionSphere.setRadius(8.0f);
	sateliteCollisionSphere.constructGeometry(myBasicShader, 16);

	frontPoint.setCentre(0, 0, 0);
	frontPoint.setRadius(0.4f);
	frontPoint.constructGeometry(myBasicShader, 16);

	backPoint.setCentre(0, 0, 0);
	backPoint.setRadius(0.4f);
	backPoint.constructGeometry(myBasicShader, 16);

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
	case 'r':
		r = true;
		break;
	case 'z':
		slowMode = !slowMode;
		break;
	case 's':
		s = true;
		break;
	case 'v':
		isThirdpersonView = !isThirdpersonView;
		isCockpitView = !isCockpitView;
		break;
	case 'b':
		isEnvironmentView = !isEnvironmentView;
		break;
	case 'f':
		speed = 0;
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
		ySpeed = 0;
		break;
	case 'p':
		p = false;
		ySpeed = 0;
		break;
	case 'a':
		a = false;
		break;
	case 'd':
		d = false;
		break;
	case 'r':
		r = false;
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
	if (Left && !isLanded)
	{
		//used to be 0.0015
		spinYinc = 0.05f;

	}
	if (Right && !isLanded)
	{
		spinYinc = -0.05f;
	}
	if (Up && !isLanded)
	{
		spinXinc = 0.05f;
	}
	if (Down)
	{
		spinXinc = -0.05f;
	}
	if (q)
	{
		spinZinc = 0.015f;
	}
	if (e)
	{
		spinZinc = -0.015f;
	}
	//used to be 2.2 and -2.4
	if (o)
	{
		ySpeed = 12.2f;
	}
	if (p && !isLanded)
	{
		ySpeed = -12.4f;
	}
	if (a)
	{
		pos.x -= 0.2f;
	}
	//used to be 0.0105
	if (w)
	{
		isLanded = false;
		successfullyLanded = false;
		
		if (slowMode)
		{
			//used to be 0.00000105f
			speed += 0.00105f;
		}
		else
		{
			speed += 10.105f;
		}
	
	}
	if (s)
	{
		if (slowMode)
		{
			//used to be 0.00000105f
			speed -= 0.00105f;
		}
		else
		{
			speed -= 10.105f;
		}
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
