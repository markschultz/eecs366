// gl_Normal: Model space normal
// gl_NormalMatrix: Transforms normals Model-space ==> Eye-space

// gl_Vertex: Model space vertex 
// gl_ModelViewMatrix: Transforms vertex Model-space ==> Eye-space
// gl_ProjectionMatrix: Transforms vertex Eye-space ==> Projection-space
// ftransform(): Equiv to gl_ProjectionMatrix*gl_ModelViewMatrix*gl_Vertex

// gl_LightSource[0].position: Light position, transformed into Eye-space

varying vec3 normalVec, lightVec, eyeVec, pU;

attribute vec3 vertexTangent;

void main()
{
	//transform the vertex from model space to projection space
	gl_Position = ftransform();
  
	/*
		calculation of the light vector:

		gl_LightSource[0].position is in eye space so to calculate the vector I need
		to use the vertex in eye space.

		After transforming the vertex, I can calculate the light position in view space
	*/
	lightVec = (gl_LightSource[0].position - ( gl_ModelViewMatrix *  gl_Vertex )).xyz;
	
	/*
		This is just the normal in view space
	*/
	normalVec = gl_NormalMatrix * gl_Normal;

	/*
		Pass PU to the pixel shader
	*/
	pU = gl_NormalMatrix * vertexTangent;

	/*
		The eye vector is, in eye space, (0, 0, 0, 1);
		to find the vector it would be:

		(0, 0, 0, 1) - (gl_ModelViewMatrix * gl_Vertex)
		
		that is

		- (gl_ModelViewMatrix * gl_Vertex)
	*/
	eyeVec = - (gl_ModelViewMatrix * gl_Vertex).xyz;

	/*
		Propagate texture coordinate, if available
	*/
	gl_TexCoord[0].s = gl_MultiTexCoord0.s;
	gl_TexCoord[0].t = 1.0 - gl_MultiTexCoord0.t;

}
