#ifndef ParticleMaster_header
#define ParticleMaster_header

#include <List>

#include "utils/particle.h"
#include "utils/particleRender.h"
#include "glm/gtx/norm.hpp"
#include "shader.h"

#define PARTICLES_PER_HIT 70
#define PI 3.14f

//Class describing the whole particle system
class ParticleMaster
{
private:
    const int MaxParticles = 10000;
    Particle ParticlesContainer[10000];
    ParticleRenderer particleRenderer;
    int LastUsedParticle = 0;
    GLfloat posSizeData[10000 * 4];
	GLubyte colorData[10000 * 4];

    //sort particles by camera distance
    void SortParticles(){ std::sort(&this->ParticlesContainer[0], &this->ParticlesContainer[MaxParticles]); };
    int FindUnusedParticle();
    
public:
    ParticleMaster(Shader particleShader);
    void Render(GLfloat deltaTime, glm::mat4 viewMatrix);
    void generateParticles(glm::vec3 origin);
};

int ParticleMaster::FindUnusedParticle()
{

    for(int i=this->LastUsedParticle; i<this->MaxParticles; i++){
        if (!this->ParticlesContainer[i].isAlive()){
            this->LastUsedParticle = i;
            return i;
        }
    }

    for(int i=0; i<this->LastUsedParticle; i++){
        if (!this->ParticlesContainer[i].isAlive()){
            this->LastUsedParticle = i;
            return i;
        }
    }

    return 0; //if all particles are taken override the first one
}

ParticleMaster::ParticleMaster(Shader particleShader)
    : particleRenderer(ParticleRenderer(particleShader))
{
}

void ParticleMaster::Render(GLfloat deltaTime, glm::mat4 viewMatrix)
{

    int ParticlesCount = 0;
    glm::vec3 cameraPos = glm::inverse(viewMatrix)[3];

    //check all particles, if one is alive add it's values to the appropriate arrays
    for(int i=0; i<this->MaxParticles; i++){

        Particle& p = this->ParticlesContainer[i];

        if(p.isAlive())
        {
            if (p.update(deltaTime))
            {

                glm::vec3 pos = p.getPosition();
                p.setcameraDistance(glm::length2( pos - cameraPos ));

                this->posSizeData[4*ParticlesCount+0] = pos.x;
                this->posSizeData[4*ParticlesCount+1] = pos.y;
                this->posSizeData[4*ParticlesCount+2] = pos.z;

                this->posSizeData[4*ParticlesCount+3] = p.getScale();

                this->colorData[4*ParticlesCount+0] = p.r;
                this->colorData[4*ParticlesCount+1] = p.g;
                this->colorData[4*ParticlesCount+2] = p.b;
                this->colorData[4*ParticlesCount+3] = p.a;

            } else
            {
                //particles that die this frame will be put at the end of the buffer in SortParticles();
                p.setcameraDistance(-1.0f);
            }

            ParticlesCount++;

        }
    }

    //to ensure correct blending
    this->SortParticles();
    this->particleRenderer.updateBuffers(ParticlesCount, this->posSizeData, this->colorData);
    this->particleRenderer.render(ParticlesCount);
}

//finds a random point in a 3D sphere
glm::vec3 RandomDir()
{
    float phi = (rand() % (int)(2.0f * PI * 100)) / 100.0f; //random(0,2pi)
    float costheta = (((rand() % 1000) - 500) / 500.0f); //random(-1,1)
    float u = ((rand() % 1000) / 1000.0f); //random(0,1)

    float theta = acos( costheta );

    float r = u;

    return glm::vec3( 
        r * sin( theta ) * cos( phi ),
        r * sin( theta ) * sin( phi ),
        r * cos( theta )
        );
}

//generates a set number of particles in a 3D point with random velocity
void ParticleMaster::generateParticles(glm::vec3 origin)
{

    //for each particle to be generated find an index in the container with a dead particle and init its attributes
    for(int i=0; i<PARTICLES_PER_HIT; i++)
    {
        
        int particleIndex = this->FindUnusedParticle();

        ParticlesContainer[particleIndex].setlifeLength(3.0f);
        ParticlesContainer[particleIndex].resetElapsedTime();
        ParticlesContainer[particleIndex].setPosition(origin);

        float spread = 10.5f; //how far particles spread
        glm::vec3 maindir = glm::vec3(0.0f, 3.0f, 0.0f); //direction bias for all particles generated

        glm::vec3 randomdir = RandomDir();
        
        ParticlesContainer[particleIndex].setVelocity(maindir + randomdir*spread);

        //set color with random alpha
        ParticlesContainer[particleIndex].r = 51;
        ParticlesContainer[particleIndex].g = 204;
        ParticlesContainer[particleIndex].b = 51;
        ParticlesContainer[particleIndex].a = (rand() % 256) / 3;

        ParticlesContainer[particleIndex].setScale((rand()%1000)/2000.0f + 0.1f);
        
    }
}

#endif