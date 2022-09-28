#include <string>

#ifdef _WIN32
    #define APIENTRY __stdcall
#endif

#include <glad/glad.h>

#include <glfw/glfw3.h>

#ifdef _WINDOWS_
    #error windows.h was included!
#endif

#include <utils/shader.h>
#include <utils/model.h>
#include <utils/camera.h>
#include <utils/physics.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <random>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include "utils/postProcessor.h"
#include "utils/text_Renderer.h"
#include "utils/particleMaster.h"

#define VELOCITY 5
#define MAX_TARGET_SPAWN_DISTANCE 50
#define MOUSE_SENSITIVITY 0.3f
#define ZOOM 20.0f
#define ZOOM_TIME 0.07f
#define ALPHA_DECAY_TIME 0.2f
#define GAME_TIME 30.0f
#define FPS_STEP 0.2f

const glm::vec3 DEFAULT_TARGET_LOCATION = glm::vec3(0.0f, -100.0f, 0.0f);
const float ALPHA_PER_SECOND = 1.0f / ALPHA_DECAY_TIME;

enum render_passes{ SHADOWMAP, RENDER};

GLuint screenWidth = 1920, screenHeight = 1080;

//game state variables
int score, totalShots;
float accuracy;

// callback functions for keyboard and mouse events
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

//gameplay functions
void player_movement();
void zoom(float amout);

//gameEvent Funtions
void endGame();
void updateTargetPosition();
glm::vec3 getTargetSpawnPoint(glm::vec3 wall1_pos, glm::vec3 wall2_pos, glm::vec3 wall_size, 
        glm::vec3 lowWall_pos, glm::vec3 lowWall_size);


//render functions
GLint LoadTextureCube(string path);
void renderObjects(Shader& object_shader, Model& cubeModel, Model& sphereModel, GLint render_pass, GLuint depthMap, Model& targetModel);
void renderSkyBox(Shader& shader, Model& cubeModel);
void renderText(float width, GLfloat currentFrame);

// mouse and keyboard globals
bool keys[1024];
GLfloat lastX, lastY;
double cursorX,cursorY;
bool firstMouse = true;

// parameters for time calculation
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// scene variables
glm::mat4 view, projection;
glm::vec3 dirLight = glm::vec3(0.5f, 1.0f, 0.5f);
GLuint textureCube;

//materials and colors
const glm::vec3 TARGET_DEFAULT_COLOR = glm::vec3(0.26f, 1.0f, 0.01f);
const glm::vec3 TARGET_HIT_COLOR = glm::vec3(1.0f, 0.0f, 0.0f);

Material targetMaterial = {
    {
        TARGET_DEFAULT_COLOR,
        TARGET_DEFAULT_COLOR,
        TARGET_DEFAULT_COLOR,
        3.0f
    },
    {
        0.4f,
        1.0f,
        0.0f
    },
    
    1.0f
};

Material wallMaterial = {
    {
        glm::vec3(0.37f,0.37f,0.37f),
        glm::vec3(0.37f,0.37f,0.37f),
        glm::vec3(0.0f,0.0f,0.37f),
        0.5f
    },
    {
        0.3f,
        0.5f,
        0.3f
    },

    1.0f
};

// helper classes init
Physics physicsEngine;
btRigidBody* target;
Camera camera(glm::vec3(0.0f, 1.7f, 9.0f), GL_TRUE, physicsEngine);
PostProcessor* postEffects;
TextRenderer* Text;
ParticleMaster* particles;

//global variables for game loop
bool hasBeenShot = false;
bool zoomIn = false;
bool playing = false;
bool vSync = true;
float FOV;
GLfloat gameTimer;

//FPS calc variables
int nbFrames;
int FPS;
float timePerFrame;
double lastTime;

//object variables
glm::vec3 wall1_pos = glm::vec3(20.0f, 3.0f, -20.0f);
glm::vec3 wall1_size = glm::vec3(1.0f, 6.0f, 70.0f);
glm::vec3 wall1_rot = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 wall2_pos = glm::vec3(-20.0f, 3.0f, -20.0f);
glm::vec3 wall2_size = glm::vec3(1.0f, 6.0f, 70.0f);
glm::vec3 wall2_rot = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 backWall_pos = glm::vec3(0.0f, 3.0f, 30.0f);
glm::vec3 backWall_size = glm::vec3(40.0f, 6.0f, 1.0f);
glm::vec3 backWall_rot = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 frontWall_pos = glm::vec3(0.0f, 3.0f, -80.0f);
glm::vec3 frontWall_size = glm::vec3(40.0f, 6.0f, 1.0f);
glm::vec3 frontWall_rot = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 lowWall_pos = glm::vec3(0.0f, 0.5f, -5.0f);
glm::vec3 lowWall_size = glm::vec3(30.0f, 1.0f, 0.3f);
glm::vec3 lowWall_rot = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 target_pos = glm::vec3(0.0f, -100.0f, 0.0f);
glm::vec3 target_size = glm::vec3(0.5f, 0.5f, 0.5f);

glm::vec3 plane_pos = glm::vec3(0.0f, 0.0f, -20.0f);
glm::vec3 plane_size = glm::vec3(40.0f, 0.1f, 70.0f);
glm::vec3 plane_rot = glm::vec3(0.0f, 0.0f, 0.0f);


// Model and Normal transformation matrices for the objects in the scene
glm::mat4 targetModelMatrix = glm::mat4(1.0f);
glm::mat3 targetNormalMatrix = glm::mat3(1.0f);

glm::mat4 planeModelMatrix = glm::mat4(1.0f);
glm::mat3 planeNormalMatrix = glm::mat3(1.0f);

glm::mat4 wall1ModelMatrix = glm::mat4(1.0f);
glm::mat3 wall1NormalMatrix = glm::mat3(1.0f);

glm::mat4 wall2ModelMatrix = glm::mat4(1.0f);
glm::mat3 wall2NormalMatrix = glm::mat3(1.0f);

glm::mat4 lowWallModelMatrix = glm::mat4(1.0f);
glm::mat3 lowWallNormalMatrix = glm::mat3(1.0f);

glm::mat4 backWallModelMatrix = glm::mat4(1.0f);
glm::mat3 backWallNormalMatrix = glm::mat3(1.0f);

glm::mat4 frontWallModelMatrix = glm::mat4(1.0f);
glm::mat3 frontWallNormalMatrix = glm::mat3(1.0f);

//current index of the target model
int currentTargetModelIndex;

GLFWwindow* windowInit()
{
    //glfw and window setup
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Aim_Map", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    //disable the mouse cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    // GLAD tries to load the context set by GLFW
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
    }

    return window;
}

int main()
{

    GLFWwindow* window = windowInit();

    //define the viewport dimensions
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    //enable Z test, face culling, and blend
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.26f, 0.46f, 0.98f, 1.0f);

    //shaders init
    Shader ui_shader = Shader("shaders/UI.vert", "shaders/UI.frag");
    Shader effectsShader = Shader("shaders/postProcess.vert", "shaders/postProcess.frag");
    Shader shadow_shader("shaders/shadowMap.vert", "shaders/shadowMap.frag");
    Shader illumination_shader = Shader("shaders/shadows.vert", "shaders/shadows.frag");
    Shader particleShader = Shader("shaders/paticle.vert", "shaders/paticle.frag");
    
    Shader skyboxShader("shaders/SkyBox.vert", "shaders/SkyBox.frag");
    textureCube = LoadTextureCube("textures/cube/Maskonaive2/");

    //buffer for shadows init
    const GLuint SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
    
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    
    GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Projection matrix of the camera: FOV angle, aspect ratio, near and far planes
    projection = glm::perspective(45.0f, (float)screenWidth/(float)screenHeight, 0.1f, 10000.0f);

    //post and text renderers instances
    postEffects = &PostProcessor(effectsShader, width, height);
    Text = new TextRenderer(width, height);
    Text->Load("Fonts/arial.ttf", 24);

    particles = &ParticleMaster(particleShader);

    //load the models
    Model cubeModel("models/cube.obj");
    Model sphereModel("models/sphere.obj");
    Model randomShape1Model("models/randomShape1.obj");
    Model pyramidModel("models/pyramid.obj");

    //array to pick at random from for the target model
    Model* modelRefArray[4] = {&cubeModel, &sphereModel, &randomShape1Model, &pyramidModel};
    currentTargetModelIndex = rand() % 4;

    //rigidBody for all solid objects
    btRigidBody* plane = physicsEngine.createRigidBody(BOX,plane_pos,plane_size,plane_rot,0.0f,0.3f,0.0f);
    btRigidBody* wall1 = physicsEngine.createRigidBody(BOX,wall1_pos,wall1_size,wall1_rot,0.0f,0.3f,0.0f);
    btRigidBody* wall2 = physicsEngine.createRigidBody(BOX,wall2_pos,wall2_size,wall2_rot,0.0f,0.3f,0.0f);
    btRigidBody* lowWall = physicsEngine.createRigidBody(BOX,lowWall_pos,lowWall_size,lowWall_rot,0.0f,0.3f,0.0f);
    btRigidBody* backWall = physicsEngine.createRigidBody(BOX,backWall_pos,backWall_size,backWall_rot,0.0f,0.3f,0.0f);
    btRigidBody* frontWall = physicsEngine.createRigidBody(BOX,frontWall_pos,frontWall_size,frontWall_rot,0.0f,0.3f,0.0f);
    if(currentTargetModelIndex == 1)
        target = physicsEngine.createRigidBody(SPHERE,target_pos,target_size,glm::vec3(0.0f, 0.0f, 0.0f),0.0f,0.3f,0.0f);
    else
        target = physicsEngine.createRigidBody(BOX,target_pos,target_size,glm::vec3(0.0f, 0.0f, 0.0f),0.0f,0.3f,0.0f);

    //maximum delta time for the physical simulation
    GLfloat maxSecPerFrame = 1.0f / 60.0f;

    // Projection matrix: FOV angle, aspect ratio, near and far planes
    FOV = 45.0f;
    projection = glm::perspective(glm::radians(FOV), (float)screenWidth/(float)screenHeight, 0.1f, 10000.0f);

    //crosshair buffers init
    GLfloat crosshair[] = {  -0.015f, 0,
                            0.015f, 0,
                            0, -0.02f,
                            0,  0.02f};
    
    GLuint UI_VBO, UI_VAO;

    glGenVertexArrays(1, &UI_VAO);
    glGenBuffers(1, &UI_VBO);
    glBindVertexArray(UI_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, UI_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(crosshair), crosshair, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

    //game variables init
    score = 0;
    totalShots = 0;
    gameTimer = GAME_TIME;
    nbFrames = 0;
    FPS = 0;
    timePerFrame = 1.0;
    lastTime = glfwGetTime();

    glfwSwapInterval(vSync);

    // Rendering loop: this code is executed at each frame
    while(!glfwWindowShouldClose(window))
    {

        //elapsed time calc
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check is an I/O event is happening
        glfwPollEvents();
        
        //zoom over time
        if (FOV != 45.0f || zoomIn)
        {
            if ((zoomIn && FOV > 45.0f - ZOOM) || !zoomIn)
            {
                float amount = deltaTime * (ZOOM / ZOOM_TIME);
                zoom(amount);
            }
        }

        //apply FPS camera movements
        player_movement();
        camera.updateCameraPos();
        // View matrix (=camera): position, view direction, camera "up" vector
        view = camera.GetViewMatrix();

        //"match" state handling
        if(playing) 
        {
            gameTimer -= deltaTime;

            if (gameTimer > 0.0f)
            {
                //target fade away and respwan
                if (hasBeenShot) 
                {
                    if(targetMaterial.alpha > 0.0f)
                    {
                        targetMaterial.alpha -= ALPHA_PER_SECOND * deltaTime;
                    }
                    else
                    {
                        updateTargetPosition();
                        hasBeenShot = false;
                    }
                }
            }
            else
                endGame();
            
        }

        //advance physics simulation by a step
        physicsEngine.dynamicsWorld->stepSimulation((deltaTime < maxSecPerFrame ? deltaTime : maxSecPerFrame),10);

        //Shadow map creation
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightPOV;
        GLfloat near_plane = -100.0f, far_plane = 100.0f;
        GLfloat frustumSize = 100.0f;

        //scene from light POV
        lightProjection = glm::ortho(-frustumSize, frustumSize, -frustumSize, frustumSize, near_plane, far_plane);    
        lightView = glm::lookAt(dirLight, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));       
        lightPOV = lightProjection * lightView;
        
        shadow_shader.Use();
        glUniformMatrix4fv(glGetUniformLocation(shadow_shader.Program, "lightPOV"), 1, GL_FALSE, glm::value_ptr(lightPOV));
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        renderObjects(shadow_shader,cubeModel,sphereModel, SHADOWMAP, depthMap, *modelRefArray[currentTargetModelIndex]);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, width, height);

        //start rendering to texture for post processing
        postEffects->BeginRender();

        //skybox first to ensure correct blending
        renderSkyBox(skyboxShader, cubeModel);

        //use shadow map with illumination shaders to render the scene
        illumination_shader.Use();

        glUniformMatrix4fv(glGetUniformLocation(illumination_shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(illumination_shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(illumination_shader.Program, "lightPOV"), 1, GL_FALSE, glm::value_ptr(lightPOV));
        GLint lightDirLocation = glGetUniformLocation(illumination_shader.Program, "lightVector");
        glUniform3fv(lightDirLocation, 1, glm::value_ptr(dirLight));
        
    
        renderObjects(illumination_shader, cubeModel, sphereModel, RENDER, depthMap, *modelRefArray[currentTargetModelIndex]);

        //render alive particles
        particleShader.Use();

        glUniform3f(glGetUniformLocation(particleShader.Program, "cameraRightVector"), view[0][0], view[1][0], view[2][0]);
		glUniform3f(glGetUniformLocation(particleShader.Program, "cameraUpVector")   , view[0][1], view[1][1], view[2][1]);
        glm::mat4 ViewProjectionMatrix = projection * view;
		glUniformMatrix4fv(glGetUniformLocation(particleShader.Program, "ViewProjection"), 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

        particles->Render(deltaTime, view);

        //stop rendering to texture
        postEffects->EndRender();
        //apply post processing and render
        postEffects->Render(zoomIn);

        //render UI and text on top of post processed texture
        ui_shader.Use();
        glBindVertexArray(UI_VAO);
        glDrawArrays(GL_LINES, 0, 4);

        glBindVertexArray(0);

        renderText(width, currentFrame);
        
        glfwSwapBuffers(window);
    }
}

//reset game variables and pick a stating target position
void startNewGame()
{
    score = 0;
    totalShots = 0;
    gameTimer = GAME_TIME;
    playing = true;
    updateTargetPosition();
}

//move target rigidbody outside the scene and reset material variables
void endGame()
{
    playing = false;
    btTransform newPos;
    newPos.setIdentity();
    newPos.setOrigin(btVector3(DEFAULT_TARGET_LOCATION.x, DEFAULT_TARGET_LOCATION.y, DEFAULT_TARGET_LOCATION.z));
    target->getMotionState()->setWorldTransform(newPos);
    target->setWorldTransform(newPos);
    targetMaterial.Color.diffuse = TARGET_DEFAULT_COLOR;
    targetMaterial.Color.specular = TARGET_DEFAULT_COLOR;
    targetMaterial.Color.ambient = TARGET_DEFAULT_COLOR;
    targetMaterial.alpha = 1.0f;
    hasBeenShot = false;
}

//move target model and rigidbody to a new position and pick a new random model to render
void updateTargetPosition()
{
    target_pos = getTargetSpawnPoint(wall1_pos, wall2_pos, wall1_size, lowWall_pos, lowWall_size);
    btTransform newPos;
    newPos.setIdentity();
    newPos.setOrigin(btVector3(target_pos.x, target_pos.y, target_pos.z));
    currentTargetModelIndex = rand() % 4;

    if(currentTargetModelIndex == 1)
        target = physicsEngine.createRigidBody(SPHERE,target_pos,target_size,glm::vec3(0.0f, 0.0f, 0.0f),0.0f,0.3f,0.0f);
    else
        target = physicsEngine.createRigidBody(BOX,target_pos,target_size,glm::vec3(0.0f, 0.0f, 0.0f),0.0f,0.3f,0.0f);

    target->getMotionState()->setWorldTransform(newPos);
    target->setWorldTransform(newPos);
    targetMaterial.Color.diffuse = TARGET_DEFAULT_COLOR;
    targetMaterial.Color.specular = TARGET_DEFAULT_COLOR;
    targetMaterial.Color.ambient = TARGET_DEFAULT_COLOR;
    targetMaterial.alpha = 1.0f;
}

//called by the player pressing left-click, implements shooting mechanics
void hitScanShoot()
{
    totalShots++;

    //use raycasting to find if the target was hit
    btVector3 btFrom(camera.Position.x, camera.Position.y, camera.Position.z);
    glm::vec3 rayEnd = camera.Position + (camera.Front * 200.0f);
    btVector3 btTo(rayEnd.x, rayEnd.y, rayEnd.z);

    btCollisionWorld::ClosestRayResultCallback res(btFrom, btTo);

    physicsEngine.dynamicsWorld->rayTest(btFrom, btTo, res);

    if(res.hasHit())
    {
        //target hit response
        if(res.m_collisionObject->getWorldTransform() == target->getWorldTransform())
        {
            hasBeenShot = true;
            targetMaterial.Color.diffuse = TARGET_HIT_COLOR;
            targetMaterial.Color.specular = TARGET_HIT_COLOR;
            targetMaterial.Color.ambient = TARGET_HIT_COLOR;
            particles->generateParticles(target_pos);
            btTransform newPos;
            newPos.setIdentity();
            //move the hitbox out of the way until the fade away is done
            newPos.setOrigin(btVector3(DEFAULT_TARGET_LOCATION.x, DEFAULT_TARGET_LOCATION.y, DEFAULT_TARGET_LOCATION.z));
            target->getMotionState()->setWorldTransform(newPos);
            target->setWorldTransform(newPos);
            score++;
        }
    }

}

//pick a point in the 3D space delimited by the specified geometry
glm::vec3 getTargetSpawnPoint(glm::vec3 wall1_pos, glm::vec3 wall2_pos, glm::vec3 wall_size, 
        glm::vec3 lowWall_pos, glm::vec3 lowWall_size)
{
    float spawnRangeX = wall1_pos.x - wall2_pos.x - (wall_size.x * 2) - 1;
    float spawnRangeY = 2.0f;

    float spawnWorldOffsetX = wall2_pos.x + wall_size.x + 0.5;
    float spawnWorldOffsetZ = lowWall_pos.z - (lowWall_size.z / 2) - 10;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distX(0, spawnRangeX);
    std::uniform_real_distribution<> distY(0, spawnRangeY);
    std::uniform_real_distribution<> distZ(0, MAX_TARGET_SPAWN_DISTANCE);

    float x = distX(gen) + spawnWorldOffsetX;
    float y = distY(gen) + 1.0f;
    float z = -distZ(gen) + spawnWorldOffsetZ;

    return glm::vec3(x, y, z);
}

//zoom by changing the FOV
void zoom(float amout)
{
    zoomIn? FOV -= amout : FOV += amout;
    if (FOV < 45.0f - ZOOM) FOV = 45.0f - ZOOM;
    if (FOV > 45.0f) FOV = 45.0f;

    projection = glm::perspective(glm::radians(FOV), (float)screenWidth/(float)screenHeight, 0.1f, 10000.0f);
}

//Move by setting the camera's rigigidBody linear velocity, y is taken from existing velocity to allow jumping with addForce
void player_movement()
{
    btVector3 direction = btVector3(0, camera.playerBody->getLinearVelocity().getY(), 0);
    if(camera.isJumping)
    {
        if (camera.Position.y <= 1.8) camera.isJumping = false;
    }
    if(keys[GLFW_KEY_W])
        direction += btVector3(camera.WorldFront.x * VELOCITY, camera.WorldFront.y, camera.WorldFront.z * VELOCITY);
    if(keys[GLFW_KEY_S])
        direction += btVector3(-camera.WorldFront.x * VELOCITY, camera.WorldFront.y, -camera.WorldFront.z * VELOCITY);
    if(keys[GLFW_KEY_A])
        direction += btVector3(-camera.Right.x * VELOCITY, camera.WorldFront.y, -camera.Right.z * VELOCITY);
    if(keys[GLFW_KEY_D])
        direction += btVector3(camera.Right.x * VELOCITY, camera.WorldFront.y, camera.Right.z * VELOCITY);

    camera.playerBody->setLinearVelocity(direction);
}

void toggleVsync()
{
    vSync = !vSync;
    glfwSwapInterval(vSync);
}

//mouse keybinds
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        if(playing) hitScanShoot();

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        zoomIn = true;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        zoomIn = false;
    }
}

//keyboard keybinds
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{

    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);


    if(key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        camera.jump();
    }

    if(key == GLFW_KEY_E && action == GLFW_PRESS)
    {
        startNewGame();
    }
    
    if(key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS)
    {
        endGame();
    }

    if(key == GLFW_KEY_TAB && action == GLFW_PRESS)
    {
        toggleVsync();
    }

    if(action == GLFW_PRESS)
        keys[key] = true;
    else if(action == GLFW_RELEASE)
        keys[key] = false;
}

//mouse movement callback
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    //save the current cursor position in 2 global variables, in order to use the values in the keyboard callback function
    cursorX = xpos;
    cursorY = ypos;

    //offset of mouse cursor position
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;

    //the new position will be the previous one for the next frame
    lastX = xpos;
    lastY = ypos;

    //pass the offset to the Camera class instance in order to update the rendering
    camera.ProcessMouseMovement(xoffset * MOUSE_SENSITIVITY, yoffset * MOUSE_SENSITIVITY);

}

//load one side of the cube texture
void LoadTextureCubeSide(string path, string side_image, GLuint side_name)
{
    int w, h;
    unsigned char* image;
    string fullname;

    fullname = path + side_image;

    image = stbi_load(fullname.c_str(), &w, &h, 0, STBI_rgb);
    if (image == nullptr)
        std::cout << "Failed to load texture!" << std::endl;
    glTexImage2D(side_name, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

    stbi_image_free(image);
}

GLint LoadTextureCube(string path)
{
    GLuint textureImage;

    //create and bind cube texture
    glGenTextures(1, &textureImage);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureImage);

    //load each side of the cube texture
    LoadTextureCubeSide(path, std::string("posx.jpg"), GL_TEXTURE_CUBE_MAP_POSITIVE_X);
    LoadTextureCubeSide(path, std::string("negx.jpg"), GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
    LoadTextureCubeSide(path, std::string("posy.jpg"), GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
    LoadTextureCubeSide(path, std::string("negy.jpg"), GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
    LoadTextureCubeSide(path, std::string("posz.jpg"), GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
    LoadTextureCubeSide(path, std::string("negz.jpg"), GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

    //set texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureImage;

}

//skybox rendering with a cube map
void renderSkyBox(Shader& skyboxShader, Model& cubeModel)
{
    //disable face culling and set the depth test to use the correct function 
    glDisable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);

    skyboxShader.Use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureCube);
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    //rotation only part of view matrix to avoid moving the skybox with the camera translation
    glm::mat4 rotationOnlyView = glm::mat4(glm::mat3(view));
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(rotationOnlyView));
    GLint textureLocation = glGetUniformLocation(skyboxShader.Program, "skybox");
    glUniform1i(textureLocation, 0);

    cubeModel.Draw();

    //reenable face culling and change depth test function
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
}

//screen text renderer function, also calculates FPS
void renderText(float width, GLfloat currentFrame)
{
    Text->RenderText("Hits: " + to_string(score), 20.0f, 20.0f, 1.0f);
    Text->RenderText("Shots: " + to_string(totalShots), 20.0f, 50.0f, 1.0f);
    
    if (totalShots > 0)
    {
        string accString = to_string(((float)score/(float)totalShots) * 100);
        Text->RenderText("Accuracy: " + accString.substr(0, accString.find(".")+3) + "%", 20.0f, 80.0f, 1.0f);
    }

    if(playing)
    {
        string timeString = to_string(gameTimer);
        Text->RenderText(timeString.substr(0, timeString.find(".")+2), (width/2.0f) - 30.0f, 100.0f, 1.0f);
        Text->RenderText("Press BACKSPACE to stop", 20.0f, 110.0f, 1.0f);
    }
    else
    {
        Text->RenderText("press E to play", (width/2.0f) - 80.0f, 20.0f, 1.0f);
    }

    //FPS counter
    nbFrames++;
    if ( currentFrame - lastTime >= FPS_STEP)
    {
        timePerFrame = (FPS_STEP * 1000.0f)/float(nbFrames);
        FPS = nbFrames * (1.0f/FPS_STEP);
        nbFrames = 0;
        lastTime += FPS_STEP;
    }
    Text->RenderText("FPS: " + to_string(FPS), (width) - 300.0f, 50.0f, 1.0f);
    Text->RenderText("Time per frame: " + to_string(timePerFrame), (width) - 300.0f, 80.0f, 1.0f);

    Text->RenderText("press TAB to switch vSync mode", (width) - 400.0f, 20.0f, 1.0f);
}

//main objects render function, called once for shadow mapping and once for rendering to screen
void renderObjects(Shader& shader, Model& cubeModel, Model& sphereModel, GLint render_pass, GLuint depthMap, Model& targetModel)
{
    
    if (render_pass == RENDER)
    {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        GLint shadowLocation = glGetUniformLocation(shader.Program, "shadowMap");
        glUniform1i(shadowLocation, 2);
        shader.updateMaterial(wallMaterial);
    }

    planeModelMatrix = glm::mat4(1.0f);
    planeNormalMatrix = glm::mat3(1.0f);
    planeModelMatrix = glm::translate(planeModelMatrix, plane_pos);
    planeModelMatrix = glm::scale(planeModelMatrix, plane_size);
    planeNormalMatrix = glm::inverseTranspose(glm::mat3(view*planeModelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(planeModelMatrix));
    glUniformMatrix3fv(glGetUniformLocation(shader.Program, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(planeNormalMatrix));

    cubeModel.Draw();

    wall1ModelMatrix = glm::mat4(1.0f);
    wall1NormalMatrix = glm::mat3(1.0f);
    wall1ModelMatrix = glm::translate(wall1ModelMatrix, wall1_pos);
    wall1ModelMatrix = glm::scale(wall1ModelMatrix, wall1_size);
    wall1NormalMatrix = glm::inverseTranspose(glm::mat3(view*wall1ModelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(wall1ModelMatrix));
    glUniformMatrix3fv(glGetUniformLocation(shader.Program, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(wall1NormalMatrix));
    
    cubeModel.Draw();

    wall2ModelMatrix = glm::mat4(1.0f);
    wall2NormalMatrix = glm::mat3(1.0f);
    wall2ModelMatrix = glm::translate(wall2ModelMatrix, wall2_pos);
    wall2ModelMatrix = glm::scale(wall2ModelMatrix, wall2_size);
    wall2NormalMatrix = glm::inverseTranspose(glm::mat3(view*wall2ModelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(wall2ModelMatrix));
    glUniformMatrix3fv(glGetUniformLocation(shader.Program, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(wall2NormalMatrix));

    cubeModel.Draw();

    lowWallModelMatrix = glm::mat4(1.0f);
    lowWallNormalMatrix = glm::mat3(1.0f);
    lowWallModelMatrix = glm::translate(lowWallModelMatrix, lowWall_pos);
    lowWallModelMatrix = glm::scale(lowWallModelMatrix, lowWall_size);
    lowWallNormalMatrix = glm::inverseTranspose(glm::mat3(view*lowWallModelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(lowWallModelMatrix));
    glUniformMatrix3fv(glGetUniformLocation(shader.Program, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(lowWallNormalMatrix));

    cubeModel.Draw();

    backWallModelMatrix = glm::mat4(1.0f);
    backWallNormalMatrix = glm::mat3(1.0f);
    backWallModelMatrix = glm::translate(backWallModelMatrix, backWall_pos);
    backWallModelMatrix = glm::scale(backWallModelMatrix, backWall_size);
    backWallNormalMatrix = glm::inverseTranspose(glm::mat3(view*backWallModelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(backWallModelMatrix));
    glUniformMatrix3fv(glGetUniformLocation(shader.Program, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(backWallNormalMatrix));

    cubeModel.Draw();

    frontWallModelMatrix = glm::mat4(1.0f);
    frontWallNormalMatrix = glm::mat3(1.0f);
    frontWallModelMatrix = glm::translate(frontWallModelMatrix, frontWall_pos);
    frontWallModelMatrix = glm::scale(frontWallModelMatrix, frontWall_size);
    frontWallNormalMatrix = glm::inverseTranspose(glm::mat3(view*frontWallModelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(frontWallModelMatrix));
    glUniformMatrix3fv(glGetUniformLocation(shader.Program, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(frontWallNormalMatrix));

    cubeModel.Draw();

    planeModelMatrix = glm::mat4(1.0f);
    wall1ModelMatrix = glm::mat4(1.0f);
    wall2ModelMatrix = glm::mat4(1.0f);
    lowWallModelMatrix = glm::mat4(1.0f);
    backWallModelMatrix = glm::mat4(1.0f);
    frontWallModelMatrix = glm::mat4(1.0f);


    if(playing) 
    {
        targetModelMatrix = glm::mat4(1.0f);
        targetNormalMatrix = glm::mat3(1.0f);
        targetModelMatrix = glm::translate(targetModelMatrix, target_pos);
        targetModelMatrix = glm::scale(targetModelMatrix, target_size);
        targetNormalMatrix = glm::inverseTranspose(glm::mat3(view*targetModelMatrix));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(targetModelMatrix));
        glUniformMatrix3fv(glGetUniformLocation(shader.Program, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(targetNormalMatrix));

        if(render_pass == RENDER) shader.updateMaterial(targetMaterial);

        targetModel.Draw();

        targetModelMatrix = glm::mat4(1.0f);
    }

    targetModelMatrix = glm::mat4(1.0f);

}

