#define GLEW_STATIC

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"

#include <iostream>
#include <map>
#include <stb_image.h>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void Do_Movement();
unsigned int loadTexture(const char* path);
void renderQuad();
unsigned int loadCubemap(std::vector<std::string> faces);

const unsigned int screenWidth = 1280;
const unsigned int screenHeight = 720;
float heightScale = 0.1;
bool blinn = false;
bool blinnKeyPressed = false;

Camera camera(glm::vec3(2.0f, 0.0f, 6.0f));
float lastX = (float)screenWidth / 2.0;
float lastY = (float)screenHeight / 2.0;
bool    keys[1024];
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

unsigned int planeVAO;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "MashaGraph", nullptr, nullptr);

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	glViewport(0, 0, screenWidth, screenHeight);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// build and compile shaders
	// -------------------------
	Shader shader("../MashGraph/5.1.parallax_mapping.vs.txt",
		"../MashGraph/5.1.parallax_mapping.fs.txt");
	Shader cubeShader("../MashGraph/cubemaps.vs.txt",
		"../MashGraph/cubemaps.fs.txt");
	Shader skyboxShader("../MashGraph/skybox.vs.txt",
		"../MashGraph/skybox.fs.txt");
	Shader refractionShader("../MashGraph/refraction.vs.txt",
		"../MashGraph/refraction.fs.txt");
	Shader lampShader("../MashGraph/lamp.vs.txt",
		"../MashGraph/lamp.frag.txt");
	Shader windowShader("../MashGraph/window.ver", "../MashGraph/window.fr");  ////смешивание

	// зеркальный
	float cubeVertices[] = {
		// positions          // normals
		1.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 2.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 2.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 2.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		1.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		1.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		1.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 2.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 2.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 2.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		1.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		1.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

		1.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		1.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		1.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		1.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		1.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		1.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 2.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 2.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 2.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 2.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 2.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 2.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		1.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 2.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 2.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 2.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		1.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		1.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		1.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 2.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 2.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 2.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		1.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		1.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	//преломляющий
	float refractVertices[] = {
		// positions          // normals
		-3.5f, -0.5f, 0.5f,  0.0f,  0.0f, -1.0f,
		 -2.5f, -0.5f, 0.5f,  0.0f,  0.0f, -1.0f,
		 -2.5f,  0.5f, 0.5f,  0.0f,  0.0f, -1.0f,
		 -2.5f,  0.5f, 0.5f,  0.0f,  0.0f, -1.0f,
		-3.5f,  0.5f, 0.5f,  0.0f,  0.0f, -1.0f,
		-3.5f, -0.5f, 0.5f,  0.0f,  0.0f, -1.0f,

		-3.5f, -0.5f,  1.5f,  0.0f,  0.0f, 1.0f,
		 -2.5f, -0.5f,  1.5f,  0.0f,  0.0f, 1.0f,
		 -2.5f,  0.5f,  1.5f,  0.0f,  0.0f, 1.0f,
		 -2.5f,  0.5f,  1.5f,  0.0f,  0.0f, 1.0f,
		-3.5f,  0.5f,  1.5f,  0.0f,  0.0f, 1.0f,
		-3.5f, -0.5f,  1.5f,  0.0f,  0.0f, 1.0f,

		-3.5f,  0.5f,  1.5f, -1.0f,  0.0f,  0.0f,
		-3.5f,  0.5f, 0.5f, -1.0f,  0.0f,  0.0f,
		-3.5f, -0.5f, 0.5f, -1.0f,  0.0f,  0.0f,
		-3.5f, -0.5f, 0.5f, -1.0f,  0.0f,  0.0f,
		-3.5f, -0.5f,  1.5f, -1.0f,  0.0f,  0.0f,
		-3.5f,  0.5f,  1.5f, -1.0f,  0.0f,  0.0f,

		 -2.5f,  0.5f,  1.5f,  1.0f,  0.0f,  0.0f,
		 -2.5f,  0.5f, 0.5f,  1.0f,  0.0f,  0.0f,
		 -2.5f, -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,
		 -2.5f, -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,
		 -2.5f, -0.5f,  1.5f,  1.0f,  0.0f,  0.0f,
		 -2.5f,  0.5f,  1.5f,  1.0f,  0.0f,  0.0f,

		-3.5f, -0.5f, 0.5f,  0.0f, -1.0f,  0.0f,
		 -2.5f, -0.5f, 0.5f,  0.0f, -1.0f,  0.0f,
		 -2.5f, -0.5f,  1.5f,  0.0f, -1.0f,  0.0f,
		 -2.5f, -0.5f,  1.5f,  0.0f, -1.0f,  0.0f,
		-3.5f, -0.5f,  1.5f,  0.0f, -1.0f,  0.0f,
		-3.5f, -0.5f, 0.5f,  0.0f, -1.0f,  0.0f,

		-3.5f,  0.5f, 0.5f,  0.0f,  1.0f,  0.0f,
		 -2.5f,  0.5f, 0.5f,  0.0f,  1.0f,  0.0f,
		 -2.5f,  0.5f,  1.5f,  0.0f,  1.0f,  0.0f,
		 -2.5f,  0.5f,  1.5f,  0.0f,  1.0f,  0.0f,
		-3.5f,  0.5f,  1.5f,  0.0f,  1.0f,  0.0f,
		-3.5f,  0.5f, 0.5f,  0.0f,  1.0f,  0.0f
	};


	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	//and windows
	std::vector<glm::vec3> windows
	{
		glm::vec3(-1.9f, 3.3f, -1.0f),
		glm::vec3(1.0f, 3.0f, -1.0f),
		glm::vec3(-0.7f, 3.5f, -1.0f)
	};

	 //прозрачный
	float transparentVertices[] = {
		//Coordinates        //TextureCoords
		 0.0f,  0.5f,  0.0f,   0.0f, 0.0f,
		 0.0f, -0.5f,  0.0f,   0.0f, 1.0f,
		 1.0f, -0.5f,  0.0f,   1.0f, 1.0f,

		 0.0f,  0.5f,  0.0f,   0.0f, 0.0f,
		 1.0f, -0.5f,  0.0f,   1.0f, 1.0f,
		 1.0f,  0.5f,  0.0f,   1.0f, 0.0f
	};

	// trfract VAO
	unsigned int refractVAO, refractVBO;
	glGenVertexArrays(1, &refractVAO);
	glGenBuffers(1, &refractVBO);
	glBindVertexArray(refractVAO);
	glBindBuffer(GL_ARRAY_BUFFER, refractVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(refractVertices), &refractVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	// cube VAO
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	// lamp
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	//for windows
	unsigned int transparentVAO, transparentVBO;
	glGenVertexArrays(1, &transparentVAO);
	glGenBuffers(1, &transparentVBO);
	glBindVertexArray(transparentVAO);
	glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	// load textures
	// -------------
	unsigned int diffuseMap = loadTexture("../MashGraph/toy_box_diffuse.png");
	unsigned int normalMap = loadTexture("../MashGraph/toy_box_normal.png");
	unsigned int heightMap = loadTexture("../MashGraph/toy_box_disp.png");
	unsigned int windowTexture = loadTexture("../MashGraph/window.png");


	std::vector<std::string> faces
	{
		"../MashGraph/skybox/right.jpg",
		"../MashGraph/skybox/left.jpg",
		"../MashGraph/skybox/top.jpg",
		"../MashGraph/skybox/bottom.jpg",
		"../MashGraph/skybox/front.jpg",
		"../MashGraph/skybox/back.jpg",
	};
	unsigned int cubemapTexture = loadCubemap(faces);

	// shader configuration
	// --------------------
	shader.Use();
	shader.setInt("diffuseMap", 0);
	shader.setInt("normalMap", 1);
	shader.setInt("depthMap", 2);

	cubeShader.Use();
	cubeShader.setInt("skybox", 0);

	refractionShader.Use();
	refractionShader.setInt("skybox", 0);

	skyboxShader.Use();
	skyboxShader.setInt("skybox", 0);

	windowShader.Use();
	windowShader.setInt("windowTexture", 0);

	// lighting info
	// -------------
	glm::vec3 lightPos(1.2f, 2.0f, -1.0f);

	// render loop

	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		Do_Movement();


		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		// configure view/projection matrices
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		shader.Use();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);

		// render parallax-mapped quad
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, 1.0f, glm::vec3(0.0, 0.0, 0.1));
		model = glm::rotate(model, glm::radians((float)glfwGetTime() * -10.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show parallax mapping from multiple directions
		shader.setMat4("model", model);
		shader.setVec3("viewPos", camera.Position);
		shader.setVec3("lightPos", lightPos);
		shader.setFloat("heightScale", heightScale); // adjust with Q and E keys

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalMap);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, heightMap);
		renderQuad();


		// refraction cube
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(3.0f, 2.0f, 0.0f));
		model = glm::rotate(model,glm::radians((float)glfwGetTime() * 20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		refractionShader.Use();
		refractionShader.setMat4("model", model);
		refractionShader.setMat4("view", view);
		refractionShader.setMat4("projection", projection);
		refractionShader.setVec3("cameraPos", camera.Position);
		glBindVertexArray(refractVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(3.0f, -1.0f, 0.0f));
		model = glm::rotate(model, glm::radians((float)glfwGetTime() * 20.0f), glm::normalize(glm::vec3(-1.0, 1.0, -1.0)));
		// draw scene as normal
		cubeShader.Use();
		
		cubeShader.setMat4("model", model);
		cubeShader.setMat4("view", view);
		cubeShader.setMat4("projection", projection);
		cubeShader.setVec3("cameraPos", camera.Position);
		// cubes
		glBindVertexArray(cubeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);


		// ИСТОЧНИК
		lampShader.Use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.1f)); // a smaller cube
		lampShader.setMat4("model", model);
		renderQuad();


		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.Use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default


		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//sorting windows by distance
		std::multimap<float, glm::vec3> sortedWindows;
		for (unsigned int i = 0; i < windows.size(); i++)
		{
			float distance = glm::length(camera.Position - windows[i]);
			sortedWindows.insert(std::make_pair(distance, windows[i]));
		}


		//draw windows
		windowShader.Use();
		glBindVertexArray(transparentVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, windowTexture);
		view = camera.GetViewMatrix();
		windowShader.setMat4("viewMat", view);
		windowShader.setMat4("projectionMat", projection);
		for (std::map<float, glm::vec3>::reverse_iterator it = sortedWindows.rbegin(); it != sortedWindows.rend(); ++it)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, it->second);
			windowShader.setMat4("modelMat", model);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		glBindVertexArray(0);


		/*	lightingShader.Use();
			shader.setMat4("projection", projection);
			shader.setMat4("view", view);
			// set light uniforms
			shader.setVec3("viewPos", camera.Position);
			shader.setVec3("lightPos", lightPos);
			shader.setInt("blinn", blinn);
	*/
	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);

	}

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &refractVAO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &skyboxVAO);
	glDeleteBuffers(1, &refractVAO);
	glDeleteBuffers(1, &cubeVAO);

	glfwTerminate();
	return 0;
}




// renders a 1x1 quad in NDC with manually calculated tangent vectors
// ------------------------------------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		// positions
		glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
		glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
		glm::vec3 pos3(1.0f, -1.0f, 0.0f);
		glm::vec3 pos4(1.0f, 1.0f, 0.0f);
		// texture coordinates
		glm::vec2 uv1(0.0f, 1.0f);
		glm::vec2 uv2(0.0f, 0.0f);
		glm::vec2 uv3(1.0f, 0.0f);
		glm::vec2 uv4(1.0f, 1.0f);
		// normal vector
		glm::vec3 nm(0.0f, 0.0f, 1.0f);

		// calculate tangent/bitangent vectors of both triangles
		glm::vec3 tangent1 = glm::vec3(1.0f);
		glm::vec3 bitangent1 = glm::vec3(1.0f);
		glm::vec3 tangent2 = glm::vec3(1.0f);
		glm::vec3 bitangent2 = glm::vec3(1.0f);
		// triangle 1
		// ----------
		glm::vec3 edge1 = pos2 - pos1;
		glm::vec3 edge2 = pos3 - pos1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent1 = glm::normalize(tangent1);

		bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent1 = glm::normalize(bitangent1);

		// triangle 2
		// ----------
		edge1 = pos3 - pos1;
		edge2 = pos4 - pos1;
		deltaUV1 = uv3 - uv1;
		deltaUV2 = uv4 - uv1;

		f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent2 = glm::normalize(tangent2);


		bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent2 = glm::normalize(bitangent2);


		float quadVertices[] = {
			// positions            // normal         // texcoords  // tangent                          // bitangent
			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
		};
		// configure plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Do_Movement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}