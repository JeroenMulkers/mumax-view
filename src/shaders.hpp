#pragma once

#include <string>

const std::string vertexshader = R"(#version 100

precision highp float;

#define PI 3.1415926

attribute vec3 aPos;
attribute vec3 aNormal;
attribute vec3 aInstancePos;
attribute vec3 aInstanceVector;
 
varying vec3 FragPos;
varying vec3 Normal; 
varying vec3 Color;

uniform ivec3 gridsize;

uniform mat4 view;
uniform mat4 projection;
uniform float arrowScalingsFactor;
uniform float cuboidScalingsFactor;
uniform mat3 colorGradient;
uniform bool useColorGradient;
uniform bool arrowGlyph;

mat3 transInverse(mat3 a)  {
    mat3 b;
    float det = 0.0;
    det += a[0][0] * a[1][1] * a[2][2];
    det += a[1][0] * a[2][1] * a[0][2];
    det += a[2][0] * a[0][1] * a[1][2];
    det -= a[2][0] * a[1][1] * a[0][2];
    det -= a[0][0] * a[2][1] * a[1][2];
    det -= a[1][0] * a[0][1] * a[2][2];
    b[0][0] =  (a[1][1] * a[2][2] - a[2][1] * a[1][2]);
    b[1][0] = -(a[0][1] * a[2][2] - a[2][1] * a[0][2]);
    b[2][0] =  (a[0][1] * a[1][2] - a[1][1] * a[0][2]);
    b[0][1] = -(a[1][0] * a[2][2] - a[2][0] * a[1][2]);
    b[1][1] =  (a[0][0] * a[2][2] - a[2][0] * a[0][2]);
    b[2][1] = -(a[0][0] * a[1][2] - a[1][0] * a[0][2]);
    b[0][2] =  (a[1][0] * a[2][1] - a[2][0] * a[1][1]);
    b[1][2] = -(a[0][0] * a[2][1] - a[2][0] * a[0][1]);
    b[2][2] =  (a[0][0] * a[1][1] - a[1][0] * a[0][1]);
    b = b/det;
    return b;
}

vec3 vec2rgb_gradient(vec3 vec) {
    vec = normalize(vec);

    vec3 colorPos = colorGradient[0];
    vec3 colorNul = colorGradient[1];
    vec3 colorNeg = colorGradient[2];

    if (vec.z > 0.) {
        return colorNul + vec.z*(colorPos-colorNul);
    } else {
        return colorNul - vec.z*(colorNeg-colorNul);
    }
}

vec3 vec2rgb_mumax(vec3 vec) {
    float H = atan(vec.y, vec.x);
    float S = 1.0;
    float L = 0.5+0.5*vec.z;
    float Hp = H/(PI/3.);
    if (Hp < 0.) 
        Hp += 6.;
    if (Hp >= 6. ) 
        Hp -= 6.;
    float C;
    if (L<=0.5) {
        C = 2.*L*S;
    } else {
        C = 2.*(1.0-L)*S;
    }
    float X = C*(1.-abs(mod(Hp,2.)-1.));
    float m = L - C/2.;
    if (Hp < 0.)
        return vec3(0.,0.,0.) + m;
    if (Hp <= 1.)
        return vec3(C ,X ,0.) + m;
    if (Hp <= 2.)
        return vec3(X ,C ,0.) + m;
    if (Hp <= 3.)
        return vec3(0.,C , X) + m;
    if (Hp <= 4.)
        return vec3(0.,X ,C ) + m;
    if (Hp <= 5.)
        return vec3(X ,0.,C ) + m;
    if (Hp <= 6.)
        return vec3(C ,0.,X ) + m;
    return vec3(0.,0.,0.);
}

void main() {
    vec4 local = vec4(aPos, 1.0);

    mat4 translation = mat4(1.0);
    translation[3][0] = aInstancePos[0];
    translation[3][1] = aInstancePos[1];
    translation[3][2] = aInstancePos[2];

    vec3 vecDir = normalize(aInstanceVector);
    mat4 skew = mat4(0.0);
    skew[0][2] = -vecDir.x;
    skew[1][2] = -vecDir.y;
    skew[2][0] =  vecDir.x;
    skew[2][1] =  vecDir.y;
    mat4 rotation = mat4(1.0);
    rotation = mat4(1.0) + skew + skew*skew*(1./(1.+vecDir.z));


    mat4 model;
    if (arrowGlyph) {
        float L = arrowScalingsFactor * length(aInstanceVector);
        mat4 scale = mat4(1.0);
        scale[0][0] = L;
        scale[1][1] = L;
        scale[2][2] = L;
        model = translation * rotation * scale;
    } else {
        mat4 scale = mat4(0.0);
        float eps = 1e-35;
        if( length(aInstanceVector) > eps ) {
            scale[0][0] = cuboidScalingsFactor;
            scale[1][1] = cuboidScalingsFactor;
            scale[2][2] = cuboidScalingsFactor;
            scale[3][3] = 1.0;
        }
        model = translation * scale; 
    }

    FragPos = vec3(model * local);
    Normal = transInverse(mat3(model)) * aNormal;  
 	gl_Position =  projection * view * model * local;

    if (useColorGradient) {
        Color = vec2rgb_gradient(vecDir);
    } else {
        Color = vec2rgb_mumax(vecDir);
    }
}
)";

const std::string fragmentshader = R"(#version 100

precision highp float;

varying vec3 Normal;  
varying vec3 FragPos;  
varying vec3 Color;  

uniform vec3 viewPos;
uniform float ambientLight;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    float diffuse = max(dot(norm, viewDir), 0.0);
    vec3 fragColor = (ambientLight + (1.0-ambientLight)*diffuse) * Color;
    gl_FragColor = vec4(fragColor, 1.0);
}
)";
