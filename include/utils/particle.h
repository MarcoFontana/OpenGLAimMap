#ifndef Particle_header
#define Particle_header

#include <glm/glm.hpp>
#include <glfw/glfw3.h>

#define GRAVITY 50

//Class describing the single particle attributes
class Particle
{
private:
    glm::vec3 position;
    glm::vec3 velocity;
    float gravityPercent;
    float lifeLength;
    float rotation;
    float scale;
    float cameraDistance;

    float elapsedTime;

public:

    unsigned char r,g,b,a;

    Particle(glm::vec3 &position, glm::vec3 velocity, float gravity, float lifeLength, float rotation, float scale);
    ~Particle();
    Particle();

    //update particle velocity, position and elapsed time values, returns false if the particle elapsed time is over it's lifetime
    bool update(GLfloat deltaTime)
    {
        this->velocity.y -= GRAVITY * this->gravityPercent * deltaTime;
        glm::vec3 delta = this->velocity * deltaTime;
        this->position += delta;
        this->elapsedTime += deltaTime;

        return this->elapsedTime < this->lifeLength;
    }

    //operator < implementation to allow sorting by camera distance
    bool operator<(const  Particle &other)
    {
        return this->cameraDistance > other.cameraDistance;
    }

    glm::vec3 getPosition() { return this->position; };
    float getRotation() { return this->rotation; };
    float getScale() { return this->scale; };
    bool isAlive() { return this->elapsedTime < this->lifeLength; };
    float getCameraDistance() { return this->cameraDistance; };
    void setcameraDistance(float newdist) { this->cameraDistance = newdist; };
    void setlifeLength(float length) { this->lifeLength = length; };
    void setPosition(glm::vec3 newPosition) { this->position = newPosition; };
    void setVelocity(glm::vec3 newVelocity) { this->velocity = newVelocity; };
    void setScale(float newScale) { this->scale = newScale; };
    void resetElapsedTime() { this->elapsedTime = 0.0f; };

};

Particle::Particle(glm::vec3 &position, glm::vec3 velocity, float gravityPercent, float lifeLength, float rotation, float scale)
    : position(position), velocity(velocity), gravityPercent(gravityPercent), lifeLength(lifeLength), rotation(rotation), scale(scale)
{
    this->elapsedTime = 0.0f;
    this->cameraDistance = -1.0f;
    
}
Particle::Particle()
    : position(glm::vec3(0.0f)), velocity(glm::vec3(0.0f)), gravityPercent(0.3f), lifeLength(0.0f), rotation(0.0f), scale(1.0f)
{
    this->elapsedTime = 0.0f;
    this->cameraDistance = -1.0f;
}
Particle::~Particle()
{
}

#endif