/*
	Copyright 2011-2019 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	By Daniel S. Buckstein
	
	passPhongAttribs_transform_atlas_vs4x.glsl
	Transform position attribute and pass attributes related to Phong shading 
		model down the pipeline. Also transforms texture coordinates.
*/

#version 410

// ****TO-DO: 
//	1) declare transformation uniforms
//	2) declare varyings (attribute data) to send to fragment shader
//	3) transform input data appropriately
//	4) copy transformed data to varyings

// (1) declare transformation uniforms
uniform mat4 uMV; // model-view matrix (object -> view)
uniform mat4 uP;
uniform mat4 uMV_nrm; // model-view matrix for normals (object -> view)
//uniform mat4 uP_inv; // clip to view
uniform mat2 uAtlas; // atlas matrix for texture coordinates

// (2) declare varyings (attribute data) to send to fragment shader
layout(location = 0) in vec4 aPosition;
layout(location = 2) in vec4 aNormal;
layout(location = 8) in vec2 aTexcoord;

out vec4 vPosition;
out vec4 vNormal;
out vec2 vTexcoord;

// Resources: https://learnopengl.com/Advanced-Lighting/Deferred-Shading
void main()
{
	vPosition = uMV * aPosition; // object to view
	gl_Position = vPosition; 
	vNormal = uMV_nrm * aNormal; // object to view
	vTexcoord = uAtlas * aTexcoord;

	/*
	vec4 pos_eye = uMV * aPosition;
	gl_Position = uP * pos_eye;

	vec4 nrm_eye = uMV_nrm * aNormal;
	*/
	
	// DUMMY OUTPUT: directly assign input position to output position
	//gl_Position = aPosition;
}
