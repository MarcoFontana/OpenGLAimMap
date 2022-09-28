//adapted from https://learnopengl.com/In-Practice/2D-Game/Postprocessing

#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.h"

class PostProcessor
{
public:
    Shader PostProcessingShader;
    unsigned int textureID;
    unsigned int Width, Height;
    
    PostProcessor(Shader shader, unsigned int width, unsigned int height);

    void BeginRender();
    void EndRender();
    void Render(bool zoomIn);

private:
    unsigned int MSFBO, FBO;
    unsigned int RBO;
    unsigned int VAO;
    
    void initRenderData();
    void generateTexture();
    void generateMultiSampleTexture();
    void MSFBOSetup();
    void FBOSetup();
};

#endif

