#include <glm/glm.hpp>

struct Material {

    struct Color {
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess;
    }Color;

    struct Light {  
        float ambient;
        float diffuse;
        float specular;
    }Light;

    float alpha;

};