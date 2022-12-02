#version 330 core

in vec4 world_space_pos;
in vec4 world_space_normal;

out vec4 fragColor;

// ShaderLight struct that contains all data for a given light
struct ShaderLight{
  vec3 lightPos;
  vec3 lightDir;
  vec3 lightColor;
  vec3 function; // for attenuation

  float penumbra;      // Only applicable to spot lights, in RADIANS
  float angle;         // Only applicable to spot lights, in RADIANS

  int lightType; // 0: directional 1: spot 2: point
};

// coefficients uniforms
uniform float ka;
uniform float kd;
uniform float ks;

// shape material coeff
uniform float shininess;
uniform vec4 shape_a;
uniform vec4 shape_d;
uniform vec4 shape_s;

// lights array
uniform ShaderLight lightVector[8];
uniform int numLights;

uniform vec4 world_camera_pos;

/**
 * @brief Falloff function for SPOT lighting
 * @param float x
 * @param float theta_inner
 * @param float theta_outer
 */
float falloff(float x, float theta_inner, float theta_outer){
    float x_term = ((x-theta_inner)/(theta_outer-theta_inner));
    // clamp pow
    if (x_term <= 0){
        return 0;
    } else {
        return (-2.0*pow(x_term, 3) + 3.0*pow(x_term,2));
    }
}

void main() {

    // initialize output fragColor
    fragColor = vec4(0.0);
    vec4 n = normalize(world_space_normal);
    vec4 dirToCamera = normalize(world_camera_pos-world_space_pos);

    // ambient term same for all lights
    vec4 ambient_term = ka*shape_a;
    vec4 diffuse_term = vec4(0.f, 0.f, 0.f, 1.0);
    vec4 specular_term = vec4(0.f, 0.f, 0.f, 1.0);

    float f_att;
    float fatt_dist;
    float spotIntensity = 1.f; // only relevant for SPOT light

    //initialization
    vec4 lightDir;
    vec4 lightPos;
    vec4 lightColor;
    vec4 surface_to_light;
    vec4 light_to_intersection; // for position
    vec4 R;

    float normal_dot_prod;
    float dot_prod;
    float theta_outer, theta_inner;
    float x;

    for (int i=0; i<numLights; i++){
        lightDir = normalize(vec4(lightVector[i].lightDir, 0.0f)); // only if light is DIRECTIONL
        surface_to_light = (-lightDir);

        light_to_intersection = normalize(vec4(lightVector[i].lightPos, 1.f) - world_space_pos);

        lightColor = vec4(lightVector[i].lightColor, 1.0);

        fatt_dist = distance(vec4(lightVector[i].lightPos, 0.f), world_space_pos);
        f_att = min(1.0, 1.f/(lightVector[i].function[0] + fatt_dist*lightVector[i].function[1] + pow(fatt_dist, 2)*lightVector[i].function[2]));

        switch (lightVector[i].lightType){
            case 0: // DIRECTIONAL
                f_att = 1.f;

                normal_dot_prod = dot(n, surface_to_light);
                if (normal_dot_prod < 0.f){
                    normal_dot_prod = 0.f;
                }

                R = -surface_to_light - 2.0*(-normal_dot_prod)*n;
                break;
            case 1: // SPOT LIGHT
                theta_outer = lightVector[i].angle;
                theta_inner = theta_outer - lightVector[i].penumbra;

                /// IMPORTANT TO KEEP NEG light_to_intersection for spot light to work!!!
                dot_prod = dot(lightDir, -light_to_intersection);
                x = acos(dot_prod);

                if (x <= theta_inner){
                    spotIntensity = 1.f;
                } else if (theta_inner < x && x <= theta_outer) {
                    spotIntensity = 1.0f-falloff(x, theta_inner, theta_outer);
                } else {
                    spotIntensity = 0.f;
                }

                //DIFFUSE
                normal_dot_prod = dot(n, light_to_intersection);
                if (normal_dot_prod < 0.f){
                    normal_dot_prod = 0.f;
                }

                R = -light_to_intersection - 2.0*(-normal_dot_prod)*n;
                break;
            case 2: // POINT LIGHT
                normal_dot_prod = dot(n, light_to_intersection);
                if (normal_dot_prod < 0.f){
                    normal_dot_prod = 0.f;
                }

                R = -light_to_intersection - 2.0*(-normal_dot_prod)*n;
                break;
        }

        vec4 diffuse_color = kd*shape_d;
        diffuse_term += spotIntensity*f_att*lightColor*(diffuse_color*normal_dot_prod);

        // SPECULAR TERM
        float RV = dot((R), dirToCamera);
        if (RV < 0.f){
            RV = 0.f;
        }

        // clamp pow
        if (shininess <= 0){
            RV = 1.f;
        } else {
            RV = pow(RV, shininess);
        }

        specular_term += spotIntensity*f_att*lightColor*(ks*shape_s*RV);
    }

    fragColor = vec4(vec3(ambient_term + diffuse_term + specular_term), 1.0);
}

