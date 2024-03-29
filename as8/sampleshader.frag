//Shader for Phong Illuminations and Phong shading

uniform vec3 AmbientContribution,DiffuseContribution,SpecularContribution;
uniform float exponent;
uniform sampler2D textureID;
varying vec3 vNormal, vLight, vView, vHalfway;

vec3 AmbientComponent(void)
{
   return vec3(AmbientContribution + 0.1);
}

vec3 DiffuseComponent(void)
{
   return vec3(DiffuseContribution * max(0.0, dot(vNormal, vLight)));
}

vec3 SpecularComponent(void)
{   
      // Approximation to the specular reflection using the halfway vector
      
      return vec3(SpecularContribution * pow(max(0.0, dot(vNormal, vHalfway)), exponent));  
}

/*
===============================================================================
   Phong Shading: Fragment Program
===============================================================================
*/

void main(void)
{
      
   // Phong Illumination Model
   
   vec3 color = (AmbientComponent() + DiffuseComponent()) +
                SpecularComponent();  
   // Final color
   
   vec4 color2 = texture2D(textureID, gl_TexCoord[0].st);
   gl_FragColor = vec4(color, 1.0);
   gl_FragColor += color2;  
}