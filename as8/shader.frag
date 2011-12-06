/**
	Paolo Surricchio - CS 541 assignment 2a
*/

uniform int mode;           // Set by the application to an integer 0-9.

uniform sampler2D earthSampler;           // Set by the application to 0.
uniform sampler2D effectSampler;           // Set by the application to 1.

uniform sampler2D tileSampler;           // Set by the application to 2.
uniform sampler2D tileEffectSampler;           // Set by the application to 3.

uniform sampler2D reflectionSampler;           // Set by the application to 4.

varying vec3 normalVec, lightVec, eyeVec, pU;// Remember to normalize before using!


vec4 PhongLighting()
{
	
	if(mode == 0 || mode == 1){
		// Lookup the Light parameters
		vec4 Ia = gl_LightSource[0].ambient + gl_LightModel.ambient;
		vec4 Id = gl_LightSource[0].diffuse;
		vec4 Is = gl_LightSource[0].specular;
   
		// Lookup the material properties in effect for this pixel
		vec4 Ie = gl_FrontMaterial.emission;
		vec4 Ka = gl_FrontMaterial.ambient;
		vec4 Kd = gl_FrontMaterial.diffuse;
		vec4 Ks = gl_FrontMaterial.specular;
		float n = gl_FrontMaterial.shininess;
    
		//here I am normalizing normalVec, lightVec and eyeVec
		vec3 normNormalVec = normalize(normalVec);
		vec3 normLightVec = normalize(lightVec);
		vec3 normEyeVec = normalize(eyeVec);

		/*
			Here I calculate the dot product between the normal of the polygon 
			(the normal interpolated in the pixel (fragment) shader)
			with the light vector. The value must be in the range: [0.0, 1.0]
		*/
		float nDotL = max( 0.0, dot(normNormalVec, normLightVec));
    
		/*
			The reflection vector is the vector used to calculate the spot of the light.
			The formula is:

			R = (2(N dot L) * N) - L
		*/
		vec3 reflectionVector = ( 2.0 * ( dot(normNormalVec, normLightVec) ) * normNormalVec ) - normLightVec;

		/*
			Now that I have the reflection vector, I can calculate the dot product between the
			reflection vector and the eye vector to calculate the spot on the surface
		*/
		float vDotR =  max( 0.0, dot(normEyeVec, reflectionVector) );

		//Return the color of the pixel as the result of the Phong calculation
		return ( ( Ie ) + ( Ia * Ka ) + ( Id * nDotL * Kd ) + ( Is * pow( vDotR, n ) * Ks ) );

    
	}else{
		if(mode == 2){
			//Lookup effect texture
			vec4 effects = texture2D(effectSampler, gl_TexCoord[0].st);

			// Lookup the Light parameters
			vec4 Ia = gl_LightSource[0].ambient + gl_LightModel.ambient;
			vec4 Id = gl_LightSource[0].diffuse;
			vec4 Is = gl_LightSource[0].specular;


			// Lookup the material properties in effect for this pixel
			vec4 Ka = texture2D(earthSampler, gl_TexCoord[0].st);
			vec4 Kd = texture2D(earthSampler, gl_TexCoord[0].st);
			vec4 Ks = gl_FrontMaterial.specular;
			float n = gl_FrontMaterial.shininess;
    
			//here I am normalizing normalVec, lightVec and eyeVec
			vec3 normNormalVec = normalize(normalVec);
			vec3 normLightVec = normalize(lightVec);
			vec3 normEyeVec = normalize(eyeVec);

			/*
				Here I calculate the dot product between the normal of the polygon 
				(the normal interpolated in the pixel (fragment) shader)
				with the light vector. The value must be in the range: [0.0, 1.0]
			*/
			float nDotL = max( 0.0, dot(normNormalVec, normLightVec));

		
			vec4 ligths = vec4(effects.g * (max(0.0, 0.5 - nDotL)));

			Ka += (effects.b * nDotL);
			Kd += (effects.b * nDotL);
		
    
			/*
				The reflection vector is the vector used to calculate the spot of the light.
				The formula is:

				R = (2(N dot L) * N) - L
			*/
			vec3 reflectionVector = ( 2.0 * ( dot(normNormalVec, normLightVec) ) * normNormalVec ) - normLightVec;

			/*
				Now that I have the reflection vector, I can calculate the dot product between the
				reflection vector and the eye vector to calculate the spot on the surface
			*/
			float vDotR =  max( 0.0, dot(normEyeVec, reflectionVector) );

			if(effects.r <= 0.3)
			//Return the color of the pixel as the result of the Phong calculation
				return ( (ligths) + ( Ia * Ka ) + ( Id * nDotL * Kd ) );
			else
				return ( ( Ia * Ka ) + ( Id * nDotL * Kd ) + ( Is * pow( vDotR, n ) * Ks ) );
		}else{
			if(mode == 3){

				const float tileFactor = 12.0;
				float scale = 25.0;
    
				// Lookup the Light parameters
				vec4 Ia = gl_LightSource[0].ambient + gl_LightModel.ambient;
				vec4 Id = gl_LightSource[0].diffuse;
				vec4 Is = gl_LightSource[0].specular;

				// Lookup the material properties in effect for this pixel
				vec4 Ka = texture2D(tileSampler, gl_TexCoord[0].st * tileFactor);
				vec4 Kd = texture2D(tileSampler, gl_TexCoord[0].st * tileFactor);
				vec4 Ks = gl_FrontMaterial.specular;
				float n = gl_FrontMaterial.shininess;
    
				//here I am normalizing normalVec, lightVec and eyeVec
				vec3 normNormalVec = normalize(normalVec);
				vec3 normLightVec = normalize(lightVec);
				vec3 normEyeVec = normalize(eyeVec);

				//calculate PV and normalize PV and PU
				vec3 pV = cross(normNormalVec, pU);
				vec3 normPV = normalize(pV);
				vec3 normPU = normalize(pU);

				//do calculation and change the normal before the lightining calculations
				float u0, v0, u1, v1;
				float i, j, uw, vh;

				uw = gl_TexCoord[0].s * 256.0 * tileFactor;
				vh = gl_TexCoord[0].t * 256.0 * tileFactor;

				i = floor(uw);
				j = floor(vh);

				u0 = 1.0 + i - uw;
				v0 = 1.0 + j - vh;
				u1 = uw - i;
				v1 = vh - j;

				vec2 ij, i1j, ij1, i1j1;
				float fu, fv;

				ij   = vec2(i, j);
				i1j  = vec2(i + 1.0, j);
				ij1  = vec2(i, j + 1.0);
				i1j1 = vec2(i + 1.0, j + 1.0);

				fu = 
					v0 * ( texture2D(tileEffectSampler, i1j / 256.0).r - texture2D(tileEffectSampler, ij / 256.0).r ) + 
					v1 * ( texture2D(tileEffectSampler, i1j1 / 256.0).r - texture2D(tileEffectSampler, ij1 / 256.0).r );

				fv =  
					u0 * ( texture2D(tileEffectSampler, ij1 / 256.0).r - texture2D(tileEffectSampler, ij / 256.0).r ) + 
					u1 * ( texture2D(tileEffectSampler, i1j1 / 256.0).r - texture2D(tileEffectSampler, i1j / 256.0).r);

				vec3 d = scale * cross( ((-fv * normPU) - (fu * normPV)), normNormalVec);

				normNormalVec += d;
	
				//done, now normalize the normal again
				normNormalVec = normalize(normNormalVec);

				//now I can do lightning calculations
				/*
					Here I calculate the dot product between the normal of the polygon 
					(the normal interpolated in the pixel (fragment) shader)
					with the light vector. The value must be in the range: [0.0, 1.0]
				*/
				float nDotL = max( 0.0, dot(normNormalVec, normLightVec));
    
				/*
					The reflection vector is the vector used to calculate the spot of the light.
					The formula is:

					R = (2(N dot L) * N) - L
				*/
				vec3 reflectionVector = ( 2.0 * ( dot(normNormalVec, normLightVec) ) * normNormalVec ) - normLightVec;

				/*
					Now that I have the reflection vector, I can calculate the dot product between the
					reflection vector and the eye vector to calculate the spot on the surface
				*/
				float vDotR =  max( 0.0, dot(normEyeVec, reflectionVector) );

				return ( ( Ia * Ka ) + ( Id * nDotL * Kd ) + ( Is * pow( vDotR, n ) * Ks ) );
			
			}else{
				//here I am normalizing normalVec, lightVec and eyeVec
				vec3 normNormalVec = normalize(normalVec);
				vec3 normLightVec = normalize(lightVec);
				vec3 normEyeVec = normalize(eyeVec);

				// Lookup the Light parameters
				vec4 Ia = gl_LightSource[0].ambient + gl_LightModel.ambient;
				vec4 Id = gl_LightSource[0].diffuse;
				vec4 Is = gl_LightSource[0].specular;
   
				float u,v;

				u = (normNormalVec.x / (length(normNormalVec))) * 0.5 + 0.5;
				v = (normNormalVec.y / (length(normNormalVec))) * 0.5 + 0.5;

				vec2 uv = vec2(u,v);

				// Lookup the material properties in effect for this pixel
				vec4 Ka = texture2D(reflectionSampler, uv);
				vec4 Kd = texture2D(reflectionSampler, uv);
				vec4 Ks = gl_FrontMaterial.specular;
				float n = gl_FrontMaterial.shininess;
    
				

				/*
					Here I calculate the dot product between the normal of the polygon 
					(the normal interpolated in the pixel (fragment) shader)
					with the light vector. The value must be in the range: [0.0, 1.0]
				*/
				float nDotL = max( 0.0, dot(normNormalVec, normLightVec));
    
				/*
					The reflection vector is the vector used to calculate the spot of the light.
					The formula is:

					R = (2(N dot L) * N) - L
				*/
				vec3 reflectionVector = ( 2.0 * ( dot(normNormalVec, normLightVec) ) * normNormalVec ) - normLightVec;

				/*
					Now that I have the reflection vector, I can calculate the dot product between the
					reflection vector and the eye vector to calculate the spot on the surface
				*/
				float vDotR =  max( 0.0, dot(normEyeVec, reflectionVector) );

				//Return the color of the pixel as the result of the Phong calculation
				return ( ( Ia * Ka ) + ( Id * nDotL * Kd ) + ( Is * pow( vDotR, n ) * Ks ) );
			}
		}
	}

}

void main()
{
	gl_FragColor = PhongLighting();
}
