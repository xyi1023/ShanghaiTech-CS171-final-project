#version 400
in vec3 EntryPoint;

uniform sampler2D Depth;
uniform sampler3D RawData;
uniform sampler3D hashTable;
uniform sampler1D TfFunc;
uniform sampler3D offset;
uniform float     StepSize;
uniform vec2      ScreenSize;
// uniform sampler2D BVHTexture; // store BVH in a 2D texture
// uniform int       topBVHIndex; // index of the root node of BVH

layout (location = 0) out vec4 FragColor;



void main()
{
    vec3 exitPoint = texture(Depth, gl_FragCoord.st/ScreenSize).xyz;
    vec3 ray = exitPoint - EntryPoint;

    // use BVH to find EntryPoint and exitpoint
    // traverseBVH(EntryPoint, exitPoint);

    float ray_length = length(ray);
    vec3 step_vector = StepSize * (ray / ray_length);

    vec3 position = EntryPoint;
    vec4 color = vec4(0.0);
    int prime1 = 37;
    int prime2 = 47;
    int m = 138;
    int n = 72;
    int sum = int(position.x) + int(position.y) * m + int(position.z) * m * m;
    int hashsum = (sum * prime1) / (m * m * m);
    vec3 hashpos = vec3(hashsum % m, (hashsum / m) % m, hashsum / (m * m));


    float acum_length = 0.0;
    while (acum_length <= ray_length && color.a < 0.95) {
        
        float intensity = texture(RawData, position).r;

        // float intensity = texture(hashTable, position).r;

        vec4 c = texture(TfFunc, intensity);

        // use two different step size for opaque and transparent
        float dynamicStepSize = (c.a > 0.9) ? StepSize : StepSize * 2.0;

        // ray casting cal rgba
        color.rgb = c.a * c.rgb + (1 - c.a) * color.a * color.rgb;
        color.a = c.a + (1 - c.a) * color.a;
        acum_length += StepSize;
        position += step_vector;
    }

    color.xyz = color.a * color.rgb + (1 - color.a) * vec3(1.0);
    color.w = 1.0;
    FragColor = color;
}
