#version 330 core

// uv coord in variable
in vec2 uv_coord;

// add a sampler2D uniform
uniform sampler2D m_texture;
uniform bool postProcessOn;
uniform bool isSharpen;

uniform int image_width;
uniform int image_height;

out vec4 fragColor;

void main()
{
        // set fragColor using the sampler2D at the UV coordinate
        fragColor = texture(m_texture, uv_coord); // sets frag color at each uv point to original picture rgb

        float width_offset = 1.f/image_width;
        float height_offset = 1.f/image_height;

        //float initial_width = -2.f*width_offset;

        float total_r = 0.f;
        float total_g = 0.f;
        float total_b = 0.f;
        float multiple;

        if (postProcessOn){
            if (isSharpen){ // SHARPENING
                float initial_height = uv_coord[0] + height_offset;

                for (int i=0; i < 3; i++){ // row
                    float initial_width = uv_coord[1] - width_offset;

                    for (int j=0; j < 3; j++){ // col
                        vec2 temp_uv = vec2(initial_height, initial_width);

                        // get color at temp_uv
                        vec4 color_sample = texture(m_texture, temp_uv);

                        // now add these color channels to the agregrate
                        if (temp_uv == uv_coord){
                            multiple = 17.f;
                        } else{
                            multiple = -1.f;
                        }

                        total_r += multiple*color_sample[0];
                        total_g += multiple*color_sample[1];
                        total_b += multiple*color_sample[2];

                        // increment width toward 1
                        initial_width += width_offset;
                    }

                    initial_height -= height_offset;
                }

                total_r = (1.f/9.f)*total_r;
                total_g = (1.f/9.f)*total_g;
                total_b = (1.f/9.f)*total_b;

                fragColor = vec4(total_r, total_g, total_b, 1.0);

            } else { // BLURRING

                float initial_height = uv_coord[0] + 2.f*height_offset;

                for (int i=0; i < 5; i++){ // row
                    float initial_width = uv_coord[1]-2.f*width_offset;
                    for (int j=0; j < 5; j++){ // col
                        vec2 temp_uv = vec2(initial_height, initial_width);

                        // get color at temp_uv
                        vec4 color_sample = texture(m_texture, temp_uv);

                        // now add these color channels to the agregrate
                        total_r += color_sample[0];
                        total_g += color_sample[1];
                        total_b += color_sample[2];

                        // increment width toward 1
                        initial_width += width_offset;
                    }

                    initial_height -= height_offset;
                }

                //now average the aggreggates and assign fragColor
                total_r = (1.f/25.f)*total_r;
                total_g = (1.f/25.f)*total_g;
                total_b = (1.f/25.f)*total_b;

                fragColor = vec4(total_r, total_g, total_b, 1.0);
            }
        }
}

