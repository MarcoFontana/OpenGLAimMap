#ifndef ParticleRenderer_header
#define ParticleRenderer_header

#include <glm/glm.hpp>
#include <glfw/glfw3.h>
#include <glad/glad.h>
#include <List>
#include "utils/particle.h"

#include "shader.h"

//Class handling the rendering of the particles
class ParticleRenderer
{
private:
    GLfloat vertices[12] = { -0.5f, 0.5f, 0.0f,
                            -0.5f, -0.5f, 0.0f,
                            0.5f, 0.5f, 0.0f,
                            0.5f, -0.5f, 0.0f };
    GLuint BillboardVBO;

    int MaxParticles = 10000;

    Shader shader;
    GLuint quadVAO;

    void init();
    void prepareForRender();
    void endRender();
public:
    GLuint posBuffer;
    GLuint colorBuffer;

    ParticleRenderer(Shader particleShader);

    void updateBuffers(int particlesCount, GLfloat* newPosData, GLubyte* newColorData);
    void render(int particlesCount);
};

//init needed buffers for rendering
void ParticleRenderer::init()
{
    glGenVertexArrays(1, &this->quadVAO);
    glBindVertexArray(this->quadVAO);

    glGenBuffers(1, &this->BillboardVBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->BillboardVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(this->vertices), this->vertices, GL_STATIC_DRAW);

    // The VBO containing the positions and sizes of the particles
    glGenBuffers(1, &this->posBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, this->posBuffer);
    // Initialize at null
    glBufferData(GL_ARRAY_BUFFER, this->MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

    // The VBO containing the colors of the particles
    glGenBuffers(1, &this->colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, this->colorBuffer);
    // Initialize at null
    glBufferData(GL_ARRAY_BUFFER, this->MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
}

//Update GPU buffers with data from CPU
void ParticleRenderer::updateBuffers(int particlesCount, GLfloat* newPosData, GLubyte* newColorData)
{
    glBindBuffer(GL_ARRAY_BUFFER, this->posBuffer);
    glBufferData(GL_ARRAY_BUFFER, this->MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particlesCount * sizeof(GLfloat) * 4, newPosData);

    glBindBuffer(GL_ARRAY_BUFFER, this->colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, this->MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particlesCount * sizeof(GLubyte) * 4, newColorData);
}

ParticleRenderer::ParticleRenderer(Shader particleShader) 
    : shader(particleShader)

{
    this->init();
}

//render all particles in the scene
void ParticleRenderer::render(int particlesCount)
{
    glBindVertexArray(this->quadVAO);
    this->prepareForRender();

    glVertexAttribDivisor(0, 0); // same values for all particles (vertex data)
    glVertexAttribDivisor(1, 1); // one value for each particle (position)
    glVertexAttribDivisor(2, 1); // one value for each particle (color)

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particlesCount);

    this->endRender();
}

//bind necessary buffers
void ParticleRenderer::prepareForRender()
{
    
    this->shader.Use();
    // vertex buffer
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, this->BillboardVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // position and scale buffer
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, this->posBuffer);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // color buffer
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, this->colorBuffer);
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void*)0 );

}

void ParticleRenderer::endRender()
{

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glBindVertexArray(0);

}

#endif