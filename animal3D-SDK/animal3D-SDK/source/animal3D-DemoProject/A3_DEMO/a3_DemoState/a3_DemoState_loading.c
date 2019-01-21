//This file was modified by Claire Yeash with permission of the author.

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
	
	a3_DemoState_loading.c/.cpp
	Demo state function implementations.

	****************************************************
	*** THIS IS ONE OF YOUR DEMO'S MAIN SOURCE FILES ***
	*** Implement your demo logic pertaining to      ***
	***     LOADING in this file.                    ***
	****************************************************
*/

//-----------------------------------------------------------------------------

// uncomment this to link extension library (if available)
//#define A3_USER_ENABLE_EXTENSION

// **WARNING: FOR TESTING/COMPARISON ONLY, DO NOT USE IN DELIVERABLE BUILDS**
// uncomment this to allow shader decoding (if available)
//#define A3_USER_ENABLE_SHADER_DECODING


//-----------------------------------------------------------------------------

#ifdef A3_USER_ENABLE_SHADER_DECODING
// override shader loading function name before including
#define a3shaderCreateFromFileList a3shaderCreateFromFileListEncoded
#endif	// A3_USER_ENABLE_SHADER_DECODING


#ifdef _WIN32
#ifdef A3_USER_ENABLE_EXTENSION
// force link extension lib
#pragma comment(lib,"animal3D-A3DX.lib")
#endif	// A3_USER_ENABLE_EXTENSION
#ifdef A3_USER_ENABLE_SHADER_DECODING
// add lib for shader decoding
#pragma comment(lib,"animal3D-UtilityLib.lib")
#endif	// A3_USER_ENABLE_SHADER_DECODING
#endif	// _WIN32


//-----------------------------------------------------------------------------

#include "../a3_DemoState.h"

#include <stdio.h>


//-----------------------------------------------------------------------------
// GENERAL UTILITIES


//-----------------------------------------------------------------------------
// LOADING

// utility to load geometry
void a3demo_loadGeometry(a3_DemoState *demoState)
{
	// static model transformations
	static const a3mat4 downscale20x = {
		+0.05f, 0.0f, 0.0f, 0.0f,
		0.0f, +0.05f, 0.0f, 0.0f,
		0.0f, 0.0f, +0.05f, 0.0f,
		0.0f, 0.0f, 0.0f, +1.0f,
	};

	// pointer to shared vbo/ibo
	a3_VertexBuffer *vbo_ibo;
	a3_VertexArrayDescriptor *vao;
	a3_VertexDrawable *currentDrawable;
	a3ui32 sharedVertexStorage = 0, sharedIndexStorage = 0;
	a3ui32 numVerts = 0;
	a3ui32 i;


	// file streaming (if requested)
	a3_FileStream fileStream[1] = { 0 };
	const a3byte *const geometryStream = "./data/geom_data_gpro_starter.dat";

	// geometry data
	a3_GeometryData sceneShapesData[3] = { 0 };
	a3_GeometryData proceduralShapesData[4] = { 0 };
	a3_GeometryData loadedModelsData[1] = { 0 };
	const a3ui32 sceneShapesCount = sizeof(sceneShapesData) / sizeof(a3_GeometryData);
	const a3ui32 proceduralShapesCount = sizeof(proceduralShapesData) / sizeof(a3_GeometryData);
	const a3ui32 loadedModelsCount = sizeof(loadedModelsData) / sizeof(a3_GeometryData);

	// common index format
	a3_IndexFormatDescriptor sceneCommonIndexFormat[1] = { 0 };
	a3ui32 bufferOffset, *const bufferOffsetPtr = &bufferOffset;


	// procedural scene objects
	// attempt to load stream if requested
	if (demoState->streaming && a3fileStreamOpenRead(fileStream, geometryStream))
	{
		// read from stream

		// static scene objects
		for (i = 0; i < sceneShapesCount; ++i)
			a3fileStreamReadObject(fileStream, sceneShapesData + i, (a3_FileStreamReadFunc)a3geometryLoadDataBinary);

		// procedurally-generated objects
		for (i = 0; i < proceduralShapesCount; ++i)
			a3fileStreamReadObject(fileStream, proceduralShapesData + i, (a3_FileStreamReadFunc)a3geometryLoadDataBinary);

		// loaded model objects
		for (i = 0; i < loadedModelsCount; ++i)
			a3fileStreamReadObject(fileStream, loadedModelsData + i, (a3_FileStreamReadFunc)a3geometryLoadDataBinary);

		// done
		a3fileStreamClose(fileStream);
	}
	// not streaming or stream doesn't exist
	else if (!demoState->streaming || a3fileStreamOpenWrite(fileStream, geometryStream))
	{
		// create new data
		a3_ProceduralGeometryDescriptor sceneShapes[3] = { a3geomShape_none };
		a3_ProceduralGeometryDescriptor proceduralShapes[4] = { a3geomShape_none };
		const a3byte *loadedShapesFile[1] = {
			"../../../../resource/obj/teapot/teapot.obj",
		};
		const a3_ModelLoaderFlag loadedShapesFlag[1] = {
			a3model_calculateVertexNormals_loadTexcoords,
		};
		const a3real *loadedShapesTransform[1] = {
			downscale20x.mm,
		};

		// static scene procedural objects
		//	(grid, axes, skybox)
		a3proceduralCreateDescriptorPlane(sceneShapes + 0, a3geomFlag_wireframe, a3geomAxis_default, 20.0f, 20.0f, 20, 20);
		a3proceduralCreateDescriptorAxes(sceneShapes + 1, a3geomFlag_wireframe, 0.0f, 1);
		a3proceduralCreateDescriptorBox(sceneShapes + 2, a3geomFlag_texcoords, 100.0f, 100.0f, 100.0f, 1, 1, 1);
		for (i = 0; i < sceneShapesCount; ++i)
		{
			a3proceduralGenerateGeometryData(sceneShapesData + i, sceneShapes + i, 0);
			a3fileStreamWriteObject(fileStream, sceneShapesData + i, (a3_FileStreamWriteFunc)a3geometrySaveDataBinary);
		}

		// other procedurally-generated objects
		// **TO-DO (lab 1): SETUP PROCEDURALLY-GENERATED GEOMETRY
		//	-> locate and read documentation for pertinent code in the framework
		//	-> use hints around this area to help
		//	-> initialize descriptors: plane, sphere, cylinder, torus
		//		-> attributes include texture coordinates and normals
		//	-> for each object: 
		//		-> generate geometry data
		//		-> optional: for each object, write data to file for streaming
		
		//copied from assignment
		//Sets up the fun procedural shapes by initializing the geometry data
		//First 4 lines: initializes a small data structure that describes some procedural shape
		a3proceduralCreateDescriptorPlane(proceduralShapes + 0,
			a3geomFlag_texcoords_normals, a3geomAxis_default, 24.0f, 24.0f, 12, 12);
		a3proceduralCreateDescriptorSphere(proceduralShapes + 1,
			a3geomFlag_texcoords_normals, a3geomAxis_default, 1.0f, 32, 24);
		a3proceduralCreateDescriptorCylinder(proceduralShapes + 2,
			a3geomFlag_texcoords_normals, a3geomAxis_x, 1.0f, 2.0f, 32, 1, 1);
		a3proceduralCreateDescriptorTorus(proceduralShapes + 3,
			a3geomFlag_texcoords_normals, a3geomAxis_x, 1.0f, 0.25f, 32, 24);
		for (i = 0; i < proceduralShapesCount; ++i)
		{
			a3proceduralGenerateGeometryData(proceduralShapesData + i,
				proceduralShapes + i, 0);
			a3fileStreamWriteObject(fileStream, proceduralShapesData + i,
				(a3_FileStreamWriteFunc)a3geometrySaveDataBinary);
		}


		// objects loaded from mesh files
		// **TO-DO (lab 1): SETUP LOADED MODEL GEOMETRY
		//	-> locate and read documentation for pertinent code in the framework
		//	-> use hints around this area to help
		//	-> for each object: 
		//		-> load model
		//		-> optional: for each object, write data to file for streaming
		
		//copied from assignment
		//Sets up the loaded models
		//Loops through each model descriptor and initializes accordingly
		for (i = 0; i < loadedModelsCount; ++i)
		{
			a3modelLoadOBJ(loadedModelsData + i,
				loadedShapesFile[i], loadedShapesFlag[i], loadedShapesTransform[i]);
			a3fileStreamWriteObject(fileStream, loadedModelsData + i,
				(a3_FileStreamWriteFunc)a3geometrySaveDataBinary);
		}

		// done
		a3fileStreamClose(fileStream);
	}


	// GPU data upload process: 
	//	- determine storage requirements
	//	- allocate buffer
	//	- create vertex arrays using unique formats
	//	- create drawable and upload data

	// get storage size
	sharedVertexStorage = numVerts = 0;
	for (i = 0; i < sceneShapesCount; ++i)
	{
		sharedVertexStorage += a3geometryGetVertexBufferSize(sceneShapesData + i);
		numVerts += sceneShapesData[i].numVertices;
	}
	for (i = 0; i < proceduralShapesCount; ++i)
	{
		sharedVertexStorage += a3geometryGetVertexBufferSize(proceduralShapesData + i);
		numVerts += proceduralShapesData[i].numVertices;
	}
	for (i = 0; i < loadedModelsCount; ++i)
	{
		sharedVertexStorage += a3geometryGetVertexBufferSize(loadedModelsData + i);
		numVerts += loadedModelsData[i].numVertices;
	}


	// common index format required for shapes that share vertex formats
	a3geometryCreateIndexFormat(sceneCommonIndexFormat, numVerts);
	sharedIndexStorage = 0;
	for (i = 0; i < sceneShapesCount; ++i)
		sharedIndexStorage += a3indexStorageSpaceRequired(sceneCommonIndexFormat, sceneShapesData[i].numIndices);
	for (i = 0; i < proceduralShapesCount; ++i)
		sharedIndexStorage += a3indexStorageSpaceRequired(sceneCommonIndexFormat, proceduralShapesData[i].numIndices);
	for (i = 0; i < loadedModelsCount; ++i)
		sharedIndexStorage += a3indexStorageSpaceRequired(sceneCommonIndexFormat, loadedModelsData[i].numIndices);


	// create shared buffer
	vbo_ibo = demoState->vbo_staticSceneObjectDrawBuffer;
	a3bufferCreateSplit(vbo_ibo, "vbo/ibo:scene", a3buffer_vertex, sharedVertexStorage, sharedIndexStorage, 0, 0);
	sharedVertexStorage = 0;


	// create vertex formats and drawables
	// grid: position attribute only
	// overlay objects are also just position
	vao = demoState->vao_position;
	a3geometryGenerateVertexArray(vao, "vao:pos", sceneShapesData + 0, vbo_ibo, sharedVertexStorage);
	currentDrawable = demoState->draw_grid;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, sceneShapesData + 0, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);

	// axes: position and color
	vao = demoState->vao_position_color;
	a3geometryGenerateVertexArray(vao, "vao:pos+col", sceneShapesData + 1, vbo_ibo, sharedVertexStorage);
	currentDrawable = demoState->draw_axes;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, sceneShapesData + 1, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);

	// skybox: position and texture coordinates
	vao = demoState->vao_position_texcoord;
	a3geometryGenerateVertexArray(vao, "vao:pos+tex", sceneShapesData + 2, vbo_ibo, sharedVertexStorage);
	currentDrawable = demoState->draw_skybox;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, sceneShapesData + 2, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);

	// **TO-DO (lab 1): CREATE VERTEX FORMATS AND DRAWABLES FOR NEW GEOMETRY
	// TWO WAYS TO DO THIS: 
	//	EASIER: self-contained drawable for each shape
	//		-> allocate enough VAOs and VBOs in demo header
	//		-> create drawables one-by-one (find pertinent drawable creation function)
	//	HARDER (BONUS): use the shared buffer
	//		-> create VAO for scene shapes; if all of the geometry has the same attributes 
	//			(see descriptor creation above), you only need to make one VAO
	//		-> generate drawable for each shape (see examples above)
	
	//plane; copied from assignment; uploads the data to the GPU
	a3geometryGenerateDrawableSelfContained(demoState->draw_plane,
		demoState->vao_planeFormat, demoState->vbo_planeDrawBuffer,
		proceduralShapesData + 0); //plus 0 for the first shape

	//sphere; plane code edited for sphere; uploads the data to the GPU
	a3geometryGenerateDrawableSelfContained(demoState->draw_sphere,
		demoState->vao_sphereFormat, demoState->vbo_sphereDrawBuffer,
		proceduralShapesData + 1); //plus 1 for the second shape

	//cylinder; plane code edited for cylinder; uploads the data to the GPU
	a3geometryGenerateDrawableSelfContained(demoState->draw_cylinder,
		demoState->vao_cylinderFormat, demoState->vbo_cylinderDrawBuffer,
		proceduralShapesData + 2); //plus 2 for the third shape

	//torus; plane code edited for torus; uploads the data to the GPU
	a3geometryGenerateDrawableSelfContained(demoState->draw_torus,
		demoState->vao_torusFormat, demoState->vbo_torusDrawBuffer,
		proceduralShapesData + 3); //plus 3 for the fourth shape

	//teapot; plane code edited for teapot; uploads the data to the GPU
	a3geometryGenerateDrawableSelfContained(demoState->draw_teapot,
		demoState->vao_teapotFormat, demoState->vbo_teapotDrawBuffer,
		loadedModelsData + 0); //loadedModelsData because the teapot is not a procedural shape


	// release data when done
	for (i = 0; i < sceneShapesCount; ++i)
		a3geometryReleaseData(sceneShapesData + i);
	for (i = 0; i < proceduralShapesCount; ++i)
		a3geometryReleaseData(proceduralShapesData + i);
	for (i = 0; i < loadedModelsCount; ++i)
		a3geometryReleaseData(loadedModelsData + i);


	// dummy drawable for point drawing: copy any of the existing ones, 
	//	set vertex count to 1 and primitive to points (0x0000)
	// DO NOT RELEASE THIS DRAWABLE; it is a managed stand-in!!!
	*demoState->dummyDrawable = *demoState->draw_axes;
	demoState->dummyDrawable->primitive = 0;
	demoState->dummyDrawable->count = 1;
}


// utility to load shaders
void a3demo_loadShaders(a3_DemoState *demoState)
{
	// direct to demo programs
	a3_DemoStateShaderProgram *currentDemoProg;
	a3i32 *currentUnif, uLocation, flag;
	a3ui32 i, j;

	// maximum uniform buffer size
	const a3ui32 uBlockSzMax = a3shaderUniformBlockMaxSize();

	// list of uniform names: align with uniform list in demo struct!
	const a3byte *uniformNames[demoStateMaxCount_shaderProgramUniform] = {
		// common vertex
		"uMVP",
		"uMV",
		"uP",
		"uMV_nrm",
		"uAtlas",

		// common fragment
		"uLightCt",
		"uLightPos",
		"uLightCol",
		"uLightSz",
		"uTex_dm",
		"uTex_sm",
		"uTex_dm_ramp",
		"uTex_sm_ramp",
		"uColor",
	};


	// some default uniform values
	const a3f32 defaultColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const a3i32 defaultTexUnits[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
	const a3f32 defaultFloat[] = { 0.0f };
	const a3i32 defaultInt[] = { 0 };


	// list of all unique shaders
	// this is a good idea to avoid multi-loading 
	//	those that are shared between programs
	union {
		struct {
			// vertex shaders
			// base
			a3_DemoStateShader passthru_transform_vs[1];
			a3_DemoStateShader passColor_transform_vs[1];
			a3_DemoStateShader passthru_transform_instanced_vs[1];
			a3_DemoStateShader passColor_transform_instanced_vs[1];
			// 02-shading
			a3_DemoStateShader passTexcoord_transform_vs[1];
			a3_DemoStateShader passPhongAttribs_transform_vs[1];

			// fragment shaders
			// base
			a3_DemoStateShader drawColorUnif_fs[1];
			a3_DemoStateShader drawColorAttrib_fs[1];
			// 02-shading
			a3_DemoStateShader drawTexture_fs[1];
			a3_DemoStateShader drawPhongMulti_fs[1];
			a3_DemoStateShader drawNonPhotoMulti_fs[1];
		};
	} shaderList = {
		{
			// ****REMINDER: 'Encoded' shaders are available for proof-of-concept
			//	testing ONLY!  Insert /e before file names.
			// DO NOT SUBMIT WORK USING ENCODED SHADERS OR YOU WILL GET ZERO!!!

			// vs
			// base
			{ { { 0 },	"shdr-vs:passthru",				a3shader_vertex  ,	1,{ "../../../../resource/glsl/4x/vs/passthru_transform_vs4x.glsl" } } },
			{ { { 0 },	"shdr-vs:pass-col",				a3shader_vertex  ,	1,{ "../../../../resource/glsl/4x/vs/passColor_transform_vs4x.glsl" } } },
			{ { { 0 },	"shdr-vs:passthru-inst",		a3shader_vertex  ,	1,{ "../../../../resource/glsl/4x/vs/passthru_transform_instanced_vs4x.glsl" } } },
			{ { { 0 },	"shdr-vs:pass-col-inst",		a3shader_vertex  ,	1,{ "../../../../resource/glsl/4x/vs/passColor_transform_instanced_vs4x.glsl" } } },
			// 02-shading
			{ { { 0 },	"shdr-vs:pass-tex",				a3shader_vertex  ,	1,{ "../../../../resource/glsl/4x/vs/02-shading/passTexcoord_transform_vs4x.glsl" } } },
			{ { { 0 },	"shdr-vs:pass-Phong",			a3shader_vertex  ,	1,{ "../../../../resource/glsl/4x/vs/02-shading/passPhongAttribs_transform_vs4x.glsl" } } },

			// fs
			// base
			{ { { 0 },	"shdr-fs:draw-col-unif",		a3shader_fragment,	1,{ "../../../../resource/glsl/4x/fs/drawColorUnif_fs4x.glsl" } } },
			{ { { 0 },	"shdr-fs:draw-col-attr",		a3shader_fragment,	1,{ "../../../../resource/glsl/4x/fs/drawColorAttrib_fs4x.glsl" } } },
			// 02-shading
			{ { { 0 },	"shdr-fs:draw-tex",				a3shader_fragment,	1,{ "../../../../resource/glsl/4x/fs/02-shading/drawTexture_fs4x.glsl" } } },
			{ { { 0 },	"shdr-fs:draw-Phong-multi",		a3shader_fragment,	1,{ "../../../../resource/glsl/4x/fs/02-shading/drawPhongMulti_fs4x.glsl" } } },
			{ { { 0 },	"shdr-fs:draw-nonphoto-multi",	a3shader_fragment,	1,{ "../../../../resource/glsl/4x/fs/02-shading/drawNonPhotoMulti_fs4x.glsl" } } },
		}
	};
	a3_DemoStateShader *const shaderListPtr = (a3_DemoStateShader *)(&shaderList), *shaderPtr;
	const a3ui32 numUniqueShaders = sizeof(shaderList) / sizeof(a3_DemoStateShader);


	printf("\n\n---------------- LOAD SHADERS STARTED  ---------------- \n");


	// load unique shaders: 
	//	- load file contents
	//	- create and compile shader object
	//	- release file contents
	for (i = 0; i < numUniqueShaders; ++i)
	{
		shaderPtr = shaderListPtr + i;
		flag = a3shaderCreateFromFileList(shaderPtr->shader,
			shaderPtr->shaderName, shaderPtr->shaderType,
			shaderPtr->filePath, shaderPtr->srcCount);
		if (flag == 0)
			printf("\n ^^^^ SHADER %u '%s' FAILED TO COMPILE \n\n", i, shaderPtr->shader->handle->name);
	}


	// setup programs: 
	//	- create program object
	//	- attach shader objects

	// base programs

	// uniform color program
	// **TO-DO (lab 1): SETUP THIS PROGRAM
	
	//uniform color program - used to draw an entire renderable object using a solid color
	currentDemoProg = demoState->prog_drawColorUnif; //sets a temporary pointer to the demo's shader object for convenience
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-col-unif"); //initializes the shader object on the GPU and assigns it a programmer-defined name
	a3shaderProgramAttachShader(currentDemoProg->program, //attaches a shader object to a program object //vertex shader
		shaderList.passthru_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, //attaches a shader object to a program object //fragment shader
		shaderList.drawColorUnif_fs->shader);		

	
	// color attrib program
	// **TO-DO (lab 1): SETUP THIS PROGRAM

	//attribut color program - used to draw an object that has a color attribute for each vertex
	currentDemoProg = demoState->prog_drawColorAttrib;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-col-attr");
	a3shaderProgramAttachShader(currentDemoProg->program,
		shaderList.passColor_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program,
		shaderList.drawColorAttrib_fs->shader);


	// uniform color program with instancing
	// (optional)

	// color attrib program with instancing
	// (optional)


	// 02-shading programs

	// texturing
	// ****TO-DO: SETUP THIS PROGRAM

	// Phong shading
	// ****TO-DO: SETUP THIS PROGRAM

	// non-photorealistic shading
	// ****TO-DO: SETUP THIS PROGRAM (bonus)


	// activate a primitive for validation
	// makes sure the specified geometry can draw using programs
	// good idea to activate the drawable with the most attributes
	a3vertexDrawableActivate(demoState->draw_axes);

	// link and validate all programs
	for (i = 0; i < demoStateMaxCount_shaderProgram; ++i)
	{
		currentDemoProg = demoState->shaderProgram + i;
		flag = a3shaderProgramLink(currentDemoProg->program);
		if (flag == 0)
			printf("\n ^^^^ PROGRAM %u '%s' FAILED TO LINK \n\n", i, currentDemoProg->program->handle->name);

		flag = a3shaderProgramValidate(currentDemoProg->program);
		if (flag == 0)
			printf("\n ^^^^ PROGRAM %u '%s' FAILED TO VALIDATE \n\n", i, currentDemoProg->program->handle->name);
	}

	// if linking fails, contingency plan goes here
	// otherwise, release shaders
	for (i = 0; i < numUniqueShaders; ++i)
	{
		shaderPtr = shaderListPtr + i;
		a3shaderRelease(shaderPtr->shader);
	}


	// prepare uniforms algorithmically instead of manually for all programs
	for (i = 0; i < demoStateMaxCount_shaderProgram; ++i)
	{
		// activate program
		currentDemoProg = demoState->shaderProgram + i;
		a3shaderProgramActivate(currentDemoProg->program);

		// get uniform and uniform block locations
		currentUnif = currentDemoProg->uniformLocation;
		for (j = 0; j < demoStateMaxCount_shaderProgramUniform; ++j)
			currentUnif[j] = a3shaderUniformGetLocation(currentDemoProg->program, uniformNames[j]);


		// set default values for all programs that have a uniform that will 
		//	either never change or is consistent for all programs

		// common VS
		if ((uLocation = currentDemoProg->uMVP) >= 0)
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, uLocation, 1, a3identityMat4.mm);
		if ((uLocation = currentDemoProg->uMV) >= 0)
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, uLocation, 1, a3identityMat4.mm);
		if ((uLocation = currentDemoProg->uP) >= 0)
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, uLocation, 1, a3identityMat4.mm);
		if ((uLocation = currentDemoProg->uMV_nrm) >= 0)
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, uLocation, 1, a3identityMat4.mm);
		if ((uLocation = currentDemoProg->uAtlas) >= 0)
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, uLocation, 1, a3identityMat4.mm);

		// common FS
		if ((uLocation = currentDemoProg->uLightCt) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultInt);
		if ((uLocation = currentDemoProg->uLightPos) >= 0)
			a3shaderUniformSendFloat(a3unif_vec4, uLocation, 1, a3wVec4.v);
		if ((uLocation = currentDemoProg->uLightCol) >= 0)
			a3shaderUniformSendFloat(a3unif_vec4, uLocation, 1, defaultColor);
		if ((uLocation = currentDemoProg->uLightSz) >= 0)
			a3shaderUniformSendFloat(a3unif_single, uLocation, 1, defaultFloat);
		if ((uLocation = currentDemoProg->uTex_dm) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 0);
		if ((uLocation = currentDemoProg->uTex_sm) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 1);
		if ((uLocation = currentDemoProg->uTex_dm_ramp) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 4);
		if ((uLocation = currentDemoProg->uTex_sm_ramp) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 5);
		if ((uLocation = currentDemoProg->uColor) >= 0)
			a3shaderUniformSendFloat(a3unif_vec4, uLocation, 1, defaultColor);
	}


	printf("\n\n---------------- LOAD SHADERS FINISHED ---------------- \n");

	//done
	a3shaderProgramDeactivate();
	a3vertexDrawableDeactivate();
}


// utility to load textures
void a3demo_loadTextures(a3_DemoState *demoState)
{
	// utilities
	const a3ui16 atlasSceneWidth = 2048, atlasSceneHeight = 2048, atlasSceneBorderPad = 8, atlasSceneAdditionalPad = 8;

	// indexing
	a3_Texture *tex;
	a3ui32 i;

	// structure for texture loading
	typedef struct a3_TAG_DEMOSTATETEXTURE {
		a3_Texture *texture;
		a3byte textureName[32];
		const a3byte *filePath;
	} a3_DemoStateTexture;

	// texture objects
	union {
		struct {
			a3_DemoStateTexture texSkyClouds[1];
			a3_DemoStateTexture texSkyWater[1];
			a3_DemoStateTexture texStoneDM[1];
			a3_DemoStateTexture texEarthDM[1];
			a3_DemoStateTexture texEarthSM[1];
			a3_DemoStateTexture texMarsDM[1];
			a3_DemoStateTexture texMarsSM[1];
			a3_DemoStateTexture texRampDM[1];
			a3_DemoStateTexture texRampSM[1];
			a3_DemoStateTexture texChecker[1];
		};
	} textureList = {
		{
			{ demoState->tex_skybox_clouds,	"tex:sky-clouds",	"../../../../resource/tex/bg/sky_clouds.png" },
			{ demoState->tex_skybox_water,	"tex:sky-water",	"../../../../resource/tex/bg/sky_water.png" },
			{ demoState->tex_stone_dm,		"tex:stone-dm",		"../../../../resource/tex/stone/stone_dm.png" },
			{ demoState->tex_earth_dm,		"tex:earth-dm",		"../../../../resource/tex/earth/2k/earth_dm_2k.png" },
			{ demoState->tex_earth_sm,		"tex:earth-sm",		"../../../../resource/tex/earth/2k/earth_sm_2k.png" },
			{ demoState->tex_mars_dm,		"tex:mars-dm",		"../../../../resource/tex/mars/1k/mars_1k_dm.png" },
			{ demoState->tex_mars_sm,		"tex:mars-sm",		"../../../../resource/tex/mars/1k/mars_1k_sm.png" },
			{ demoState->tex_ramp_dm,		"tex:ramp-dm",		"../../../../resource/tex/sprite/celRamp_dm.png" },
			{ demoState->tex_ramp_sm,		"tex:ramp-sm",		"../../../../resource/tex/sprite/celRamp_sm.png" },
			{ demoState->tex_checker,		"tex:checker",		"../../../../resource/tex/sprite/checker.png" },
		}
	};
	const a3ui32 numTextures = sizeof(textureList) / sizeof(a3_DemoStateTexture);
	a3_DemoStateTexture *const textureListPtr = (a3_DemoStateTexture *)(&textureList), *texturePtr;

	// load all textures
	for (i = 0; i < numTextures; ++i)
	{
		texturePtr = textureListPtr + i;
		a3textureCreateFromFile(texturePtr->texture, texturePtr->textureName, texturePtr->filePath);
		a3textureActivate(texturePtr->texture, a3tex_unit00);
		a3textureDefaultSettings();
	}

	// change settings on a per-texture or per-type basis
	tex = demoState->texture;
	// skyboxes and stone
	for (i = 0; i < 3; ++i, ++tex)
	{
		a3textureActivate(tex, a3tex_unit00);
		a3textureChangeFilterMode(a3tex_filterLinear);	// linear pixel blending
	}
	// planets
	for (i = 0; i < 4; ++i, ++tex)
	{
		a3textureActivate(tex, a3tex_unit00);
		a3textureChangeFilterMode(a3tex_filterLinear);
	}
	// ramps
	for (i = 0; i < 2; ++i, ++tex)
	{
		a3textureActivate(tex, a3tex_unit00);
		a3textureChangeRepeatMode(a3tex_repeatClamp, a3tex_repeatClamp);	// clamp both axes
	}

	// done
	a3textureDeactivate(a3tex_unit00);
}


//-----------------------------------------------------------------------------

// the handle release callbacks are no longer valid; since the library was 
//	reloaded, old function pointers are out of scope!
// could reload everything, but that would mean rebuilding GPU data...
//	...or just set new function pointers!
void a3demo_refresh(a3_DemoState *demoState)
{
	a3_BufferObject *currentBuff = demoState->drawDataBuffer,
		*const endBuff = currentBuff + demoStateMaxCount_drawDataBuffer;
	a3_VertexArrayDescriptor *currentVAO = demoState->vertexArray,
		*const endVAO = currentVAO + demoStateMaxCount_vertexArray;
	a3_DemoStateShaderProgram *currentProg = demoState->shaderProgram,
		*const endProg = currentProg + demoStateMaxCount_shaderProgram;
	a3_Texture *currentTex = demoState->texture,
		*const endTex = currentTex + demoStateMaxCount_texture;

	while (currentBuff < endBuff)
		a3bufferHandleUpdateReleaseCallback(currentBuff++);
	while (currentVAO < endVAO)
		a3vertexArrayHandleUpdateReleaseCallback(currentVAO++);
	while (currentProg < endProg)
		a3shaderProgramHandleUpdateReleaseCallback((currentProg++)->program);
	while (currentTex < endTex)
		a3textureHandleUpdateReleaseCallback(currentTex++);
}


//-----------------------------------------------------------------------------
