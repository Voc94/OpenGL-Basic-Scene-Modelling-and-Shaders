#if defined (__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
    #define GL_SILENCE_DEPRECATION
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"
#include <iostream>


struct CameraPoint {
    glm::vec3 position;
    glm::vec3 target;
};
CameraPoint camPoints[] = {
    { glm::vec3(-19.105f, 22.9624f, 17.7986f), glm::vec3(0.820272f, -0.54024f, -0.187867f) },
    { glm::vec3(-52.7893f, 49.0611f, -35.7142f), glm::vec3(0.672188f, -0.541708f, 0.504694f) },
    { glm::vec3(-50.687f, 40.1714f, -46.7778f), glm::vec3(0.62725f, -0.45865f, 0.629443f) },
    { glm::vec3(-13.6005f, 39.9785f, -107.231f), glm::vec3(-0.238226f, -0.238533f, 0.941462f) },
    { glm::vec3(-13.6005f, 39.9785f, -107.231f), glm::vec3(-0.238226f, -0.238533f, 0.941462f) },
    { glm::vec3(53.5569f, 12.7501f, 6.67029f), glm::vec3(-0.970137f, -0.175367f, 0.167572f) },
    { glm::vec3(3.06586f, 16.4153f, 18.8653f), glm::vec3(-0.605985f, -0.455545f, 0.65212f) },
    { glm::vec3(43.0532f, 2.22801f, 58.9534f), glm::vec3(-0.466044f, -0.0383878f, -0.883928f) },
    { glm::vec3(17.9015f, 2.09016f, 58.2091f), glm::vec3(0.330506f, -0.0069812f, -0.943778f) },
    { glm::vec3(46.0227f, 2.72005f, -25.3341f), glm::vec3(-0.90015f, -0.0610485f, 0.431281f) }
};


int glWindowWidth = 1280;
int glWindowHeight = 1080;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// light parameters



// Sun (Directional Light) Parameters
glm::vec3 sunLightColor = glm::vec3(0.2f, 0.2f, 0.2f); // Sun color (white)
glm::vec3 sunLightDirection = glm::vec3(-0.2f, -1.0f, -0.3f); // Sun direction
float sunLightAmbientStrength = 0.3f; // Ambient strength of sun
float sunLightSpecularStrength = 0.5f; // Specular strength of sun

// Point Light Parameters (already defined in your code)
 //glm::vec3 lightColor = glm::vec3(0.8f, 0.5f, 0.0f);
//light 1
glm::vec3 lightColor1 = glm::vec3(1.0f, 1.0f, 1.0f);
 glm::vec3 lightPos1 = glm::vec3(8.30244, 2.0565605, 14.9901);
float pointLightAmbientStrength1 = 1.0f; // Adjust as needed
float pointLightSpecularStrength1 = 1.0f; // Adjust as needed
//light 2
glm::vec3 lightColor2 = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 lightPos2 = glm::vec3(34.6905, 15.9348, -31.542);
float pointLightAmbientStrength2 = 1.0f; // Adjust as needed
float pointLightSpecularStrength2 = 1.0f; // Adjust as needed


// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;

// camera
gps::Camera myCamera(
    glm::vec3(0.0f, 2.0f, 5.5f), // Position
    glm::vec3(0.0f, 0.0f, -1.0f), // Orientation (pointing towards -Z initially)
    glm::vec3(0.0f, 1.0f, 0.0f),  // Up direction
    1.0f                           // Movement speed
);
GLfloat cameraSpeed = 0.1f;

GLboolean pressedKeys[1024];

// models
gps::Model3D lightSource;
gps::SkyBox mySkyBox;
gps::Model3D streetLamp;
gps::Model3D grass;
gps::Model3D tree1;
gps::Model3D tree2;
gps::Model3D castle;
gps::Model3D grass2;
gps::Model3D water;
gps::Model3D comet;
gps::Model3D birchtree;
gps::Model3D fairy_ball;
gps::Model3D fairy_wing;
gps::Model3D chest;
GLfloat angle;

// shaders
gps::Shader skyboxShader;
gps::Shader myBasicShader;
gps::Shader lightCubeShader;
//delta
float mouseDeltaX = 0.0f;
float mouseDeltaY = 0.0f;

//transparency
//view modes
bool isWireframe = false;
bool isPoint = false;
//fairy dust stuff
float flutterAngle = 1.0f;
float fairyBallRotationAngle = 0.0f;
glm::vec3 fairyRotationDir = glm::vec3(0.0f,1.0f,0.0f);
GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)
void interpolateCamera(CameraPoint& start, CameraPoint& end, float t, glm::vec3& position, glm::vec3& target) {
    position = glm::mix(start.position, end.position, t);
    target = glm::mix(start.target, end.target, t);
}
bool isInterpolating = false;
float totalTime = 5.0f; // Total time to move from one point to the next
float lastTime = 0.0f;
float currentTime = 0.0f;
int currentPoint = 0;
void updateCamera(float deltaTime) {
    if (isInterpolating) {
        currentTime += deltaTime;
        float t = currentTime / totalTime;

        if (t >= 1.0f) {
            t = 0.0f;
            currentPoint++;
            currentTime = 0.0f;

            if (currentPoint >= sizeof(camPoints) / sizeof(CameraPoint) - 1) {
                isInterpolating = false; // Stop interpolation
                currentPoint = 0;
                return;
            }
        }

        glm::vec3 interpolatedPosition, interpolatedTarget;
        interpolateCamera(camPoints[currentPoint], camPoints[currentPoint + 1], t, interpolatedPosition, interpolatedTarget);

        myCamera.setPosition(interpolatedPosition);
        myCamera.setOrientation(interpolatedTarget);
    }
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
	//TODO
}
bool playback = false;
void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        isWireframe = !isWireframe;
    }
    if (key == GLFW_KEY_G && action == GLFW_PRESS) {
        isPoint = !isPoint;
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        isInterpolating = true;
        currentTime = 0.0f;
        currentPoint = 0;
    }
	if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    static float lastX = glWindowWidth / 2.0;
    static float lastY = glWindowHeight / 2.0;
    static bool firstMouse = true;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // Adjust as needed
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    static float pitch = 0.0f;
    static float yaw = -90.0f; // Initialized to -90 degrees since a yaw of 0 results in a direction vector pointing to the right

    pitch += yoffset;
    yaw += xoffset;

    // Constrain the pitch value to prevent screen flipping
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // Update camera orientation
    myCamera.rotate(pitch, yaw);

}

void processMovement()
{
    if (pressedKeys[GLFW_KEY_W]) {
        myCamera.move(gps::MOVE_FORWARD);
        
    }
    if (pressedKeys[GLFW_KEY_S]) {
        myCamera.move(gps::MOVE_BACKWARD);
    }

    if (pressedKeys[GLFW_KEY_A]) {
        myCamera.move(gps::MOVE_LEFT);
    }

    if (pressedKeys[GLFW_KEY_D]) {
        myCamera.move(gps::MOVE_RIGHT);
    }
    if (pressedKeys[GLFW_KEY_I]) {
        lightPos1.z += 1;
    }
    if (pressedKeys[GLFW_KEY_J]) {
        lightPos1.z -= 1;
    }
}

void initOpenGLWindow() {
    myWindow.Create(1280, 1080, "OpenGL Project Core");
}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

}

void initOpenGLState() {
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() {
    streetLamp.LoadModel("models/campfire/Campfire.obj");
    grass.LoadModel("models/grass/grass2.obj");
    tree1.LoadModel("models/tree1/Spruce.obj");
    tree2.LoadModel("models/tree2/tree_small_02_1k.obj");
    castle.LoadModel("models/castle/tower.obj");
    grass2.LoadModel("models/trees/untitled.obj");
    water.LoadModel("models/water/grass2.obj");
    comet.LoadModel("models/comet/comet.obj");
    fairy_ball.LoadModel("models/fairy_ball/fairy_ball.obj");
    fairy_wing.LoadModel("models/fairy_right_wing/fairy_right_wing.obj");
    chest.LoadModel("models/chest/chest.obj");
    //grass2.LoadModel("models/maple/maple.obj");
    //light source
    lightSource.LoadModel("models/cube/cube.obj");
    // Initialize SkyBox 
    std::vector<const GLchar*> faces;
    faces.push_back("skybox/posx.jpg");
    faces.push_back("skybox/negx.jpg");
    faces.push_back("skybox/posy.jpg");
    faces.push_back("skybox/negy.jpg");
    faces.push_back("skybox/posz.jpg");
    faces.push_back("skybox/negz.jpg");
    mySkyBox.Load(faces);
}

void initShaders() {
	myBasicShader.loadShader(
        "shaders/basic.vert",
        "shaders/basic.frag");
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
}
//projection matrix nearplane farplane
float near_plane = 0.1f;
float far_plane = 1000.0f;
void initUniforms() {
	myBasicShader.useShaderProgram();

    // create common object model matrix
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

	// get view matrix for current camera
	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
	// send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for common objects
    normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

	// create projection matrix
     projection = glm::perspective(glm::radians(45.0f),
        (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
        near_plane, far_plane);
    projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
    // send projection matrix to shader
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Set Sun (Directional Light) uniforms
    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "sunLightColor"), 1, glm::value_ptr(sunLightColor));
    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "sunLightDirection"), 1, glm::value_ptr(sunLightDirection));
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "sunLightAmbientStrength"), sunLightAmbientStrength);
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "sunLightSpecularStrength"), sunLightSpecularStrength);

    // Set Point Light uniforms
    //1
    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightColor1"), 1, glm::value_ptr(lightColor1));
    glm::vec4 pointLightPosEye1 = (view * model * glm::vec4(lightPos1, 1.0f));
    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightPosEye1"), 1, glm::value_ptr(pointLightPosEye1));
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightAmbientStrength1"), pointLightAmbientStrength1);
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightSpecularStrength1"), pointLightSpecularStrength1);
    //2
    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightColor2"), 1, glm::value_ptr(lightColor2));
    glm::vec4 pointLightPosEye2 = (view * model * glm::vec4(lightPos2, 1.0f));
    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightPosEye2"), 1, glm::value_ptr(pointLightPosEye2));
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightAmbientStrength2"), pointLightAmbientStrength2);
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightSpecularStrength2"), pointLightSpecularStrength2);
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "alpha"), 1.0f); // Reset alpha to 1.0 for other objects

}

void renderObjects(gps::Shader shader) {
    shader.useShaderProgram();
    // Render the street lamp
    glm::mat4 campfireModel = glm::mat4(1.0f); // Set the model matrix for the street lamp
    campfireModel = glm::translate(campfireModel,lightPos1 - glm::vec3(0.0f,2.0f,0.0f)); // Position the street lamp
    campfireModel = glm::scale(campfireModel, glm::vec3(0.3f, 0.3f, 0.3f)); // Scale the street lamp
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(campfireModel));
    streetLamp.Draw(shader);
    shader.useShaderProgram();

    // Render the grass ground
    glm::mat4 grassModel = glm::mat4(1.0f); // Set the model matrix for the street lamp
    grassModel = glm::translate(grassModel, glm::vec3(0.0f, 0.0f, 0.0f)); // Position the street lamp
    grassModel = glm::scale(grassModel, glm::vec3(5.0f, 5.0f, 5.0f)); // Scale the street lamp
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(grassModel));
    grass.Draw(shader);

    glUniform1f(glGetUniformLocation(shader.shaderProgram, "alpha"), 0.4f); // Set transparency for water
    glm::mat4 waterModel = glm::mat4(1.0f);
    waterModel = glm::translate(waterModel, glm::vec3(0.0f, -2.0f, 0.0f));
    waterModel = glm::scale(waterModel, glm::vec3(2.3f, 1.0f, 2.3f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(waterModel));
    water.Draw(shader);

    glUniform1f(glGetUniformLocation(shader.shaderProgram, "alpha"), 1.0f); // Reset alpha to 1.0 for other objects
    glm::mat4 cometModel = glm::mat4(1.0f);
    cometModel = glm::translate(cometModel, glm::vec3(-37.1837, 40.7352, -53.3822));
    cometModel = glm::rotate(cometModel, glm::radians(3.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    cometModel = glm::scale(cometModel, glm::vec3(5.0f, 5.0f, 5.0f)); // Scale the grass
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cometModel));
    comet.Draw(shader);
    
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "alpha"), 1.0f); // Reset alpha to 1.0 for other objects
    glm::mat4 fairy_ballModel = glm::mat4(1.0f);
    fairy_ballModel = glm::translate(fairy_ballModel, lightPos2);
    fairy_ballModel = glm::rotate(fairy_ballModel, glm::radians(3.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    fairy_ballModel = glm::rotate(fairy_ballModel, glm::radians(fairyBallRotationAngle),fairyRotationDir);
    fairy_ballModel = glm::scale(fairy_ballModel, glm::vec3(0.5f, 0.5f, 0.5f)); // Scale the grass
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(fairy_ballModel));
    fairy_ball.Draw(shader);

    glUniform1f(glGetUniformLocation(shader.shaderProgram, "alpha"), 0.4f); // Set transparency for wings
    glm::mat4 fairy_ballModel1 = fairy_ballModel;
    fairy_ballModel1 = glm::rotate(fairy_ballModel1, glm::radians(flutterAngle), fairyRotationDir);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(fairy_ballModel1));
    fairy_wing.Draw(shader);

    glm::mat4 fairy_ballModel2 = fairy_ballModel;
    fairy_ballModel2  = glm::translate(fairy_ballModel2, glm::vec3(-3.5, 0.2f, -0.25f));
    fairy_ballModel2 = glm::rotate(fairy_ballModel2, glm::radians(-flutterAngle), fairyRotationDir);
    fairy_ballModel2 = glm::rotate(fairy_ballModel2, glm::radians(305.0f), fairyRotationDir);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(fairy_ballModel2));
    fairy_wing.Draw(shader);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "alpha"), 1.0f); // Reset alpha to 1.0 for other objects
    //glm::vec3 pivotOffset = glm::vec3(-0.3f, 0.2f, -0.5f); // Adjust this to your model's pivot
    //glUniform1f(glGetUniformLocation(shader.shaderProgram, "alpha"), 1.0f); // Reset alpha to 1.0 for other objects
    //glm::mat4 fairy_wingModel1 = glm::mat4(1.0f);
    //fairy_wingModel1 = glm::translate(fairy_wingModel1, lightPos2 + glm::vec3(-0.3f, 0.2f, -0.5f));
    //fairy_wingModel1 = glm::translate(fairy_wingModel1, pivotOffset);
    //fairy_wingModel1 = glm::rotate(fairy_wingModel1, glm::radians(3.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    //fairy_wingModel1 = glm::rotate(fairy_wingModel1, glm::radians(flutterAngle), fairyRotationDir);
    //fairy_wingModel1 = glm::rotate(fairy_wingModel1, glm::radians(fairyBallRotationAngle), fairyRotationDir);
    //fairy_wingModel1 = glm::translate(fairy_wingModel1, -pivotOffset);
    //fairy_wingModel1 = glm::scale(fairy_wingModel1, glm::vec3(0.5f, 0.5f, 0.5f));
    //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(fairy_wingModel1));
    //fairy_wing.Draw(shader);

    //glUniform1f(glGetUniformLocation(shader.shaderProgram, "alpha"), 1.0f); // Reset alpha to 1.0 for other objects
    //// Update fairy wing model 2 - possibly with an opposite phase
    //glm::mat4 fairy_wingModel2 = glm::mat4(1.0f);
    //fairy_wingModel2 = glm::translate(fairy_wingModel2, lightPos2 + glm::vec3(-2.5f, 0.2f, 0.1f));
    //fairy_wingModel2 = glm::translate(fairy_wingModel2, -pivotOffset);
    //fairy_wingModel2 = glm::rotate(fairy_wingModel2, glm::radians(305.0f - flutterAngle), fairyRotationDir);
    //fairy_wingModel2 = glm::rotate(fairy_wingModel2, glm::radians(fairyBallRotationAngle), fairyRotationDir);
    //fairy_wingModel2 = glm::translate(fairy_wingModel2, pivotOffset);
    //fairy_wingModel2 = glm::scale(fairy_wingModel2, glm::vec3(0.5f, 0.5f, 0.5f));
    //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(fairy_wingModel2));
    //fairy_wing.Draw(shader);
}
void renderFoliage(gps::Shader shader) {
    const int numTrees = 70; // Number of tre
    const float radius = 55.0f; // Radius of the circle

    // Render the trees in a circle
    for (int i = 0; i < numTrees; ++i) {
        glm::mat4 treeModel = glm::mat4(1.0f); // Set the model matrix for each tree instance

        // Calculate the position for each tree instance in a circle
        float angle = glm::radians(static_cast<float>(i) / numTrees * 360.0f); // Angle around the circle
        float xPos = radius * glm::cos(angle);
        float zPos = radius * glm::sin(angle);
        if (i<63 && i > 15 || i < 7) {
            treeModel = glm::translate(treeModel, glm::vec3(xPos, 0.0f, zPos)); // Position the tree instance
            treeModel = glm::rotate(treeModel, angle, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate the tree instance around y-axis
            treeModel = glm::rotate(treeModel, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate the tree instance 90 degrees around x-axis to stand upright
            treeModel = glm::scale(treeModel, glm::vec3(1.0f, 1.0f, 1.0f)); // Scale the tree instance

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(treeModel));
            tree1.Draw(shader);
         }
    }
    // Number of rows and columns in the grid for trees
    const int numRowsTrees = 2; // Adjust as needed
    const int numColsTrees = 2; // Adjust as needed

    // Spacing between tree instances
    const float spacingXTrees = 10.0f; // Adjust as needed
    const float spacingZTrees = 10.0f; // Adjust as needed

    // Initial position for trees
    float startXTrees = -numColsTrees * spacingXTrees / 2.0f;
    float startZTrees = -numRowsTrees * spacingZTrees / 2.0f;
    float signMultiplier = 1.0f; // Start with either 1.0f or -1.0f

    // Render the trees in a grid
    for (int row = 0; row < numRowsTrees; ++row) {
        for (int col = 0; col < numColsTrees; ++col) {
            glm::mat4 treeModel = glm::mat4(1.0f);
            // Calculate the position for each tree instance
            float xPosTree = startXTrees + col * spacingXTrees + cos(col * row) * 10.0f;
            float zPosTree = startZTrees + row * spacingZTrees + sin(col * row) * 10.0f;


            treeModel = glm::translate(treeModel, glm::vec3(xPosTree, row*col, zPosTree));
            treeModel = glm::scale(treeModel, glm::vec3(2.0f, 2.0f, 2.0f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(treeModel));
            tree2.Draw(shader);
        }
        std::vector<glm::vec3> grassMatrix;

        // Position 1: Somewhat near the center
        grassMatrix.push_back(glm::vec3(-25.0f, 6.0f, -3.0f));

        // Position 2: Towards the top-right corner
        grassMatrix.push_back(glm::vec3(-10.0f, -0.2f, 30.0f));

            glm::mat4 grass2Model1 = glm::mat4(1.0f); // Set the model matrix for the grass
            signMultiplier *= -1.0f;
            grass2Model1 = glm::translate(grass2Model1, grassMatrix[0]);
            grass2Model1 = glm::rotate(grass2Model1, glm::radians(345.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            grass2Model1 = glm::scale(grass2Model1, glm::vec3(75.0f, 45.0f, 90.0f)); // Scale the grass
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(grass2Model1));
            grass2.Draw(shader);
   
            glm::mat4 grass2Model2 = glm::mat4(1.0f); // Set the model matrix for the grass
            signMultiplier *= -1.0f;
            grass2Model2 = glm::translate(grass2Model2, grassMatrix[1]);
            grass2Model2 = glm::rotate(grass2Model2, glm::radians(3.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            grass2Model2 = glm::scale(grass2Model2, glm::vec3(75.0f, 25.0f, 90.0f)); // Scale the grass
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(grass2Model2));
            grass2.Draw(shader);


    }

    // Initialize an array of positions for the four corners for castles
    glm::vec3 cornerPositions[] = {
        glm::vec3(-49.1743, 9.5f, 20.8129),  // Top-right corner
        glm::vec3(-7.00101, -0.5586104, 27.3699), // Top-left corner
        glm::vec3(49.3638, 2.10864, 8.44362), // Bottom-right corner
        glm::vec3(-32.6986, 7.2, -11.8585) // Bottom-left corner
    };

    for (int i = 0; i < 4; i++) {
        glm::mat4 castleModel = glm::mat4(1.0f); // Set the model matrix for the castle
        castleModel = glm::translate(castleModel, cornerPositions[i]); // Position the castle
        castleModel = glm::scale(castleModel, glm::vec3(10.0f, 10.0f, 10.0f)); // Scale the castle
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(castleModel));
        castle.Draw(shader);

        glUniform1f(glGetUniformLocation(shader.shaderProgram, "alpha"), 1.0f); // Reset alpha to 1.0 for other objects
        glm::mat4 chestModel = glm::mat4(1.0f);
        chestModel = glm::translate(chestModel, cornerPositions[i] + glm::vec3(0.0f, 8.5f, 0.0f));
        chestModel = glm::rotate(chestModel, glm::radians(3.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        chestModel = glm::scale(chestModel, glm::vec3(1.0f, 1.0f, 1.0f)); // Scale the grass
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(chestModel));
        chest.Draw(shader);
    }

}

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //render movement
    myBasicShader.useShaderProgram();
    glm::vec4 pointLightPosEye1 = (view * model * glm::vec4(lightPos1, 1.0f));
    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightPosEye1"), 1, glm::value_ptr(pointLightPosEye1));
    glm::vec4 pointLightPosEye2 = (view * model * glm::vec4(lightPos2, 1.0f));
    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightPosEye2"), 1, glm::value_ptr(pointLightPosEye2));
    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightColor2"), 1, glm::value_ptr(lightColor2));
    // create common object model matrix
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

    // get current matrix for current camera
     // create common object model matrix
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

    // get view matrix for current camera
    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
    // send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for common objects
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

    // create projection matrix
    projection = glm::perspective(glm::radians(45.0f),
        (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
        near_plane, far_plane);
    projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
    // send projection matrix to shader
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


    // Render the skybox first
    glDepthFunc(GL_LEQUAL); // Change depth function so depth test passes when values are equal to depth buffer's content
    skyboxShader.useShaderProgram();
    mySkyBox.Draw(skyboxShader, myCamera.getViewMatrix(), projection);
    glDepthFunc(GL_LESS); // Set depth function back to default

    if (isWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else if (isPoint) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    myBasicShader.useShaderProgram();
	// render all objects
	renderObjects(myBasicShader);
    renderFoliage(myBasicShader);

    
}

void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}

int main(int argc, const char* argv[]) {
    initOpenGLWindow(); // Initializes myWindow
    setWindowCallbacks(); // Sets callbacks for myWindow
    initOpenGLState();
    initModels();
    initShaders();
    initUniforms();
    while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processMovement();
        //rainbow
        float timeValue = glfwGetTime(); // Get the current time value
        float deltaTime = 0.0f;
        deltaTime = timeValue - lastTime;
        lastTime = timeValue;
        updateCamera(deltaTime);


        //fairy flutterfly
        flutterAngle = sin(timeValue * 10.0f) * 5.0f; // Adjust the multiplier for speed and the amplitude for range of motion

        // Rotate that fairy carl
        float radius = 50.0f;
        float speed = 1.0f; // Adjust this for faster or slower rotation

        lightPos2.x = sin(timeValue * speed) * -radius;// ca idee eu nus mare matecian, am facut asta prin trial-error
        lightPos2.z = cos(timeValue * speed)* -radius;
        lightPos2.y += sin(timeValue * speed*10) * 0.1;

        // Assuming fairyBallRotationAngle is in degrees
        fairyBallRotationAngle = (timeValue * speed * 180.0f / (3.14159));

        // Creating a rainbow effect for pointLightColor1
        float red = sin(timeValue * 2.0f) * 0.5f + 0.5f;
        float green = sin(timeValue * 0.7f) * 0.5f + 0.5f;
        float blue = sin(timeValue * 1.3f) * 0.5f + 0.5f;

        lightColor2 = glm::vec3(red, green, blue);
        if (mouseDeltaX != 0.0f || mouseDeltaY != 0.0f) {
            float sensitivity = 0.1f; // Adjust as needed
            myCamera.rotate(mouseDeltaY * sensitivity, mouseDeltaX * sensitivity);

            // Reset deltas
            mouseDeltaX = 0.0f;
            mouseDeltaY = 0.0f;
        }
        renderScene();
        glfwPollEvents();
        glfwSwapBuffers(myWindow.getWindow());
    }

    cleanup();
    return 0;
}
