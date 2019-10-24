#version 450 core
//layout (pixel_center_integer) in vec4 gl_FragCoord;

#define OFFSET vec2(0,0)

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D image;

uniform bool horizontal;
uniform float weight[2] = float[](0.125, 0.5);
uniform float offset = -3.0;
uniform float vx = 1;
uniform float vy = 1;
uniform float rx;
uniform float ry;

// Todo: specify calculation of offset for box filter
vec3 offsets[13] = vec3[](
	vec3(1,1, weight[0]),
	vec3(3,1, weight[0]),
	vec3(5,1, weight[0]),

	vec3(2,2, weight[1]),
	vec3(4,2, weight[1]),
	
	vec3(1,3, weight[0]),
	vec3(3,3, weight[0]),
	vec3(5,3, weight[0]),
	
	vec3(2,4, weight[1]),
	vec3(4,4, weight[1]),

	vec3(1,5, weight[0]),
	vec3(3,5, weight[0]),
	vec3(5,5, weight[0])

);

//vec4 Sample(vec2 uv) {
//	return texture(image, uv);
//}

#define Sample(uv) textureLod(image, uv, 0)

/*
	TexCoords - coordinates current pixel in out framebuffer
	image - n + 1 mip level
	texel_scale - step in relative units in input image -- key of sampling
*/
vec4 downsample(vec2 uv)
{

	vec4 result = vec4(0);
	vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel

	for (int i = 0; i < 13; i++)
	{
		vec2 coord = (2 * gl_FragCoord.xy + (offsets[i].xy + offset));

		if (all(lessThan(vec2(vx,vy)*(gl_FragCoord.xy + 0.5*(offsets[i].xy + offset)), vec2(rx - 3,ry - 3))))
		{
			vec2 texel = clamp((vec2(vx,vy) * coord * tex_offset), vec2(0), vec2(vx,vy));
			result += Sample(texel) * offsets[i].z;
		}
	}
	return result;
}

void main()
{    
	vec2 uv = 2 * (gl_FragCoord.xy) / vec2(textureSize(image, 0));
	FragColor = downsample(uv);
}