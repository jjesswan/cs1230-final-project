#version 330 core

// uv coord in variable
in vec2 uv_coord;

// Task 8: Add a sampler2D uniform
uniform sampler2D m_texture;
uniform bool postProcessOn;
uniform bool isGrayScale;

out vec4 fragColor;

void main()
{
        // set fragColor using the sampler2D at the UV coordinate
        fragColor = texture(m_texture, uv_coord);

        // invert fragColor's r, g, and b color channels if your bool is true
        if (postProcessOn){
            if (isGrayScale){ // GRAYSCALE
                float grayPixel = (.299*fragColor[0]) + (.587*fragColor[1]) + (.114*fragColor[2]);
                fragColor = vec4(grayPixel, grayPixel, grayPixel, 1.0);
            } else { // INVERT
                float r = 1.f-fragColor[0];
                float g = 1.f-fragColor[1];
                float b = 1.f-fragColor[2];
                fragColor = vec4(r,g,b, 1.f);
            }
        }
}
