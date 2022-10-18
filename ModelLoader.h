#pragma once
#include "GL\glew.h"
#include "GL\wglew.h"
#pragma comment(lib, "glew32.lib")
//--------------

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_inverse.hpp"

#include "GL\freeglut.h"

#include "Images\FreeImage.h"

#include "shaders\Shader.h"

//MODEL LOADING
#include "3DStruct\threeDModel.h"
#include "Obj\OBJLoader.h"

class ModelLoader
{
private:
	CShader* myShader;
	CShader* myBasicShader;
	COBJLoader objLoader;


		
public:


	void initModel(std::string path, CThreeDModel &model, CShader* myShader,bool isCenteredOnScene)
	{


		//lets initialise our object's rotation transformation 
		//to the identity matrix

		std::cout << " loading model " << std::endl;
		if (objLoader.LoadModel(path))//returns true if the model is loaded
		{
			std::cout << " model loaded " << std::endl;

			//copy data from the OBJLoader object to the threedmodel class
			model.ConstructModelFromOBJLoader(objLoader);

			//if you want to translate the object to the origin of the screen,
			//first calculate the centre of the object, then move all the vertices
			//back so that the centre is on the origin.
			if (isCenteredOnScene)
			{
				model.CalcCentrePoint();
				model.CentreOnZero();
			}



			model.InitVBO(myShader);
		}
		else
		{
			std::cout << " model failed to load " << std::endl;
		}

	}




};

