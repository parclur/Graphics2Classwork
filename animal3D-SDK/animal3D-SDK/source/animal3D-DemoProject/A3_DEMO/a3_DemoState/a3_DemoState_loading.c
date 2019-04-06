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
#define A3_USER_ENABLE_SHADER_DECODING


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

inline a3real4x4r a3demo_setAtlasTransform_internal(a3real4x4p m_out,
	const a3ui16 atlasWidth, const a3ui16 atlasHeight,
	const a3ui16 subTexturePosX, const a3ui16 subTexturePosY,
	const a3ui16 subTextureWidth, const a3ui16 subTextureHeight,
	const a3ui16 subTextureBorderPadding, const a3ui16 subTextureAdditionalPadding)
{
	a3real4x4SetIdentity(m_out);
	m_out[0][0] = (a3real)(subTextureWidth) / (a3real)(atlasWidth);
	m_out[1][1] = (a3real)(subTextureHeight) / (a3real)(atlasHeight);
	m_out[3][0] = (a3real)(subTexturePosX + subTextureBorderPadding + subTextureAdditionalPadding) / (a3real)(atlasWidth);
	m_out[3][1] = (a3real)(subTexturePosY + subTextureBorderPadding + subTextureAdditionalPadding) / (a3real)(atlasHeight);
	return m_out;
}


// initialize dummy drawable
inline void a3demo_initDummyDrawable_internal(a3_DemoState *demoState)
{
	// dummy drawable for point drawing: copy any of the existing ones, 
	//	set vertex count to 1 and primitive to points (0x0000)
	// DO NOT RELEASE THIS DRAWABLE; it is a managed stand-in!!!
	*demoState->dummyDrawable = *demoState->draw_grid;
	demoState->dummyDrawable->primitive = 0;
	demoState->dummyDrawable->count = 1;
}


//-----------------------------------------------------------------------------
// LOADING

// utility to load geometry
void a3demo_loadGeometry(a3_DemoState *demoState)
{
	// tmp descriptor for loaded model
	typedef struct a3_TAG_DEMOSTATELOADEDMODEL {
		const a3byte *filePath;
		const a3real *transform;
		a3_ModelLoaderFlag flag;
	} a3_DemoStateLoadedModel;

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
	a3ui32 i, j;


	// file streaming (if requested)
	a3_FileStream fileStream[1] = { 0 };
	const a3byte *const geometryStream = "./data/geom_data_gpro_skeletal.dat";

	// geometry data
	a3_GeometryData displayShapesData[6] = { 0 };
	a3_GeometryData hiddenShapesData[1] = { 0 };
	a3_GeometryData proceduralShapesData[4] = { 0 };
	a3_GeometryData loadedModelsData[1] = { 0 };
	a3_GeometryData morphTargetData[1][demoStateMaxCount_morphTargetPerModel] = { 0 };
	const a3ui32 displayShapesCount = sizeof(displayShapesData) / sizeof(a3_GeometryData);
	const a3ui32 hiddenShapesCount = sizeof(hiddenShapesData) / sizeof(a3_GeometryData);
	const a3ui32 proceduralShapesCount = sizeof(proceduralShapesData) / sizeof(a3_GeometryData);
	const a3ui32 loadedModelsCount = sizeof(loadedModelsData) / sizeof(a3_GeometryData);
	const a3ui32 morphTargetCount = sizeof(morphTargetData) / sizeof(a3_GeometryData);
	const a3ui32 morphModelsCount = morphTargetCount / demoStateMaxCount_morphTargetPerModel;

	// common index format
	a3_IndexFormatDescriptor sceneCommonIndexFormat[1] = { 0 };
	a3ui32 bufferOffset, *const bufferOffsetPtr = &bufferOffset;

	// vertex format and descriptors for morphing models
	a3_VertexFormatDescriptor morphVertexFormat[1] = { 0 };
	a3_VertexAttributeDescriptor morphVertexAttrib[a3attrib_nameMax] = { 0 };
	a3_VertexAttributeDataDescriptor morphVertexAttribData[a3attrib_nameMax] = { 0 };


	// procedural scene objects
	// attempt to load stream if requested
	if (demoState->streaming && a3fileStreamOpenRead(fileStream, geometryStream))
	{
		// read from stream

		// static display objects
		for (i = 0; i < displayShapesCount; ++i)
			a3fileStreamReadObject(fileStream, displayShapesData + i, (a3_FileStreamReadFunc)a3geometryLoadDataBinary);

		// hidden volume objects
		for (i = 0; i < hiddenShapesCount; ++i)
			a3fileStreamReadObject(fileStream, hiddenShapesData + i, (a3_FileStreamReadFunc)a3geometryLoadDataBinary);

		// procedurally-generated objects
		for (i = 0; i < proceduralShapesCount; ++i)
			a3fileStreamReadObject(fileStream, proceduralShapesData + i, (a3_FileStreamReadFunc)a3geometryLoadDataBinary);

		// loaded model objects
		for (i = 0; i < loadedModelsCount; ++i)
			a3fileStreamReadObject(fileStream, loadedModelsData + i, (a3_FileStreamReadFunc)a3geometryLoadDataBinary);

		// morphing model objects
		for (i = 0; i < morphTargetCount; ++i)
			a3fileStreamReadObject(fileStream, morphTargetData + i, (a3_FileStreamReadFunc)a3geometryLoadDataBinary);

		// done
		a3fileStreamClose(fileStream);
	}
	// not streaming or stream doesn't exist
	else if (!demoState->streaming || a3fileStreamOpenWrite(fileStream, geometryStream))
	{
		// create new data
		a3_ProceduralGeometryDescriptor displayShapes[6] = { a3geomShape_none };
		a3_ProceduralGeometryDescriptor hiddenShapes[1] = { a3geomShape_none };
		a3_ProceduralGeometryDescriptor proceduralShapes[4] = { a3geomShape_none };
		const a3_DemoStateLoadedModel loadedShapes[1] = {
			{ "../../../../resource/obj/teapot/teapot.obj", downscale20x.mm, a3model_calculateVertexTangents }
		};
		const a3_DemoStateLoadedModel morphShapes[1][demoStateMaxCount_morphTargetPerModel] = {
			{
				{ "../../../../resource/obj/teapot_morph/teapot.obj", downscale20x.mm, a3model_calculateVertexTangents },
				{ "../../../../resource/obj/teapot_morph/teapot0.obj", downscale20x.mm, a3model_calculateVertexTangents },
				{ "../../../../resource/obj/teapot_morph/teapot1.obj", downscale20x.mm, a3model_calculateVertexTangents },
				{ "../../../../resource/obj/teapot_morph/teapot2.obj", downscale20x.mm, a3model_calculateVertexTangents },
				{ "../../../../resource/obj/teapot_morph/teapot3.obj", downscale20x.mm, a3model_calculateVertexTangents },
			},
		};

		const a3ubyte lightVolumeSlices = 8, lightVolumeStacks = 6;
		const a3real lightVolumeRadius = a3realOne;

		// static scene procedural objects
		//	(display shapes, axes, grid, bones, skybox, unit quad)
		a3proceduralCreateDescriptorAxes(displayShapes + 0, a3geomFlag_wireframe, 0.0f, 1);
		a3proceduralCreateDescriptorPlane(displayShapes + 1, a3geomFlag_wireframe, a3geomAxis_default, 20.0f, 20.0f, 20, 20);
		a3proceduralCreateDescriptorCone(displayShapes + 2, a3geomFlag_wireframe, a3geomAxis_default, 0.1f, 1.0f, 4, 1, 1);
		a3proceduralCreateDescriptorDiamond(displayShapes + 3, a3geomFlag_wireframe, a3geomAxis_default, 0.5f, 1.0f, 4, 1);
		a3proceduralCreateDescriptorBox(displayShapes + 4, a3geomFlag_texcoords, 100.0f, 100.0f, 100.0f, 1, 1, 1);
		a3proceduralCreateDescriptorPlane(displayShapes + 5, a3geomFlag_texcoords, a3geomAxis_default, 2.0f, 2.0f, 1, 1);
		for (i = 0; i < displayShapesCount; ++i)
		{
			a3proceduralGenerateGeometryData(displayShapesData + i, displayShapes + i, 0);
			a3fileStreamWriteObject(fileStream, displayShapesData + i, (a3_FileStreamWriteFunc)a3geometrySaveDataBinary);
		}

		// hidden volumes and shapes
		//	(light volumes)
		a3proceduralCreateDescriptorSphere(hiddenShapes + 0, a3geomFlag_vanilla, a3geomAxis_default, 
			lightVolumeRadius * a3trigFaceToPointRatio(a3realThreeSixty, a3realOneEighty, lightVolumeSlices, lightVolumeStacks),
			lightVolumeSlices, lightVolumeStacks);
		for (i = 0; i < hiddenShapesCount; ++i)
		{
			a3proceduralGenerateGeometryData(hiddenShapesData + i, hiddenShapes + i, 0);
			a3fileStreamWriteObject(fileStream, hiddenShapesData + i, (a3_FileStreamWriteFunc)a3geometrySaveDataBinary);
		}

		// other procedurally-generated objects
		a3proceduralCreateDescriptorPlane(proceduralShapes + 0, a3geomFlag_tangents, a3geomAxis_default, 24.0f, 24.0f, 12, 12);
		a3proceduralCreateDescriptorSphere(proceduralShapes + 1, a3geomFlag_tangents, a3geomAxis_default, 1.0f, 32, 24);
		a3proceduralCreateDescriptorCylinder(proceduralShapes + 2, a3geomFlag_tangents, a3geomAxis_x, 1.0f, 2.0f, 32, 1, 1);
		a3proceduralCreateDescriptorTorus(proceduralShapes + 3, a3geomFlag_tangents, a3geomAxis_x, 1.0f, 0.25f, 32, 24);
		for (i = 0; i < proceduralShapesCount; ++i)
		{
			a3proceduralGenerateGeometryData(proceduralShapesData + i, proceduralShapes + i, 0);
			a3fileStreamWriteObject(fileStream, proceduralShapesData + i, (a3_FileStreamWriteFunc)a3geometrySaveDataBinary);
		}

		// objects loaded from mesh files
		for (i = 0; i < loadedModelsCount; ++i)
		{
			a3modelLoadOBJ(loadedModelsData + i, loadedShapes[i].filePath, loadedShapes[i].flag, loadedShapes[i].transform);
			a3fileStreamWriteObject(fileStream, loadedModelsData + i, (a3_FileStreamWriteFunc)a3geometrySaveDataBinary);
		}

		// morphing objects loaded from mesh files
		for (i = 0; i < morphModelsCount; ++i)
			for (j = 0; j < demoStateMaxCount_morphTargetPerModel; ++j)
			{
				a3modelLoadOBJ(morphTargetData[i] + j, morphShapes[i][j].filePath, morphShapes[i][j].flag, morphShapes[i][j].transform);
				a3fileStreamWriteObject(fileStream, morphTargetData[i] + j, (a3_FileStreamWriteFunc)a3geometrySaveDataBinary);
			}
		
		// done
		a3fileStreamClose(fileStream);
	}


	// manually set up additional vertex formats (e.g. morphing models)
	// manually set up vertex format data (e.g. pull data from morphing models)
	//	- position, normal, tangent x5; last one is texcoord
	//	- keep like attributes in sequence so they can be read as array in VS
	for (i = 0, j = demoStateMaxCount_morphTargetPerModel; i < j; ++i)
	{
		// attrib descriptors
		a3vertexAttribCreateDescriptor(morphVertexAttrib + i, (a3_VertexAttributeName)(i), a3attrib_vec3);
		a3vertexAttribCreateDescriptor(morphVertexAttrib + i + j, (a3_VertexAttributeName)(i + j), a3attrib_vec3);
		a3vertexAttribCreateDescriptor(morphVertexAttrib + i + j * 2, (a3_VertexAttributeName)(i + j * 2), a3attrib_vec3);

		// data descriptors
		a3vertexAttribDataCreateDescriptor(morphVertexAttribData + i, (a3_VertexAttributeName)(i), morphTargetData[0][i].attribData[a3attrib_geomPosition]);
		a3vertexAttribDataCreateDescriptor(morphVertexAttribData + i + j, (a3_VertexAttributeName)(i + j), morphTargetData[0][i].attribData[a3attrib_geomNormal]);
		a3vertexAttribDataCreateDescriptor(morphVertexAttribData + i + j * 2, (a3_VertexAttributeName)(i + j * 2), morphTargetData[0][i].attribData[a3attrib_geomTangent]);
	}
	// common attributes (e.g. texcoord)
	a3vertexAttribCreateDescriptor(morphVertexAttrib + j * 3, (a3_VertexAttributeName)(j * 3), a3attrib_vec2);
	a3vertexAttribDataCreateDescriptor(morphVertexAttribData + j * 3, (a3_VertexAttributeName)(j * 3), morphTargetData[0][0].attribData[a3attrib_geomTexcoord]);

	// create format
	a3vertexCreateFormatDescriptor(morphVertexFormat, morphVertexAttrib, a3attrib_nameMax);


	// GPU data upload process: 
	//	- determine storage requirements
	//	- allocate buffer
	//	- create vertex arrays using unique formats
	//	- create drawable and upload data

	// get storage size
	sharedVertexStorage = numVerts = 0;
	for (i = 0; i < displayShapesCount; ++i)
	{
		sharedVertexStorage += a3geometryGetVertexBufferSize(displayShapesData + i);
		numVerts += displayShapesData[i].numVertices;
	}
	for (i = 0; i < hiddenShapesCount; ++i)
	{
		sharedVertexStorage += a3geometryGetVertexBufferSize(hiddenShapesData + i);
		numVerts += hiddenShapesData[i].numVertices;
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
	for (i = 0; i < morphModelsCount; ++i)
	{
		sharedVertexStorage += a3vertexStorageSpaceRequired(morphVertexFormat, morphTargetData[i]->numVertices);
		numVerts += morphTargetData[i]->numVertices;
	}


	// common index format required for shapes that share vertex formats
	a3geometryCreateIndexFormat(sceneCommonIndexFormat, numVerts);
	sharedIndexStorage = 0;
	for (i = 0; i < displayShapesCount; ++i)
		sharedIndexStorage += a3indexStorageSpaceRequired(sceneCommonIndexFormat, displayShapesData[i].numIndices);
	for (i = 0; i < hiddenShapesCount; ++i)
		sharedIndexStorage += a3indexStorageSpaceRequired(sceneCommonIndexFormat, hiddenShapesData[i].numIndices);
	for (i = 0; i < proceduralShapesCount; ++i)
		sharedIndexStorage += a3indexStorageSpaceRequired(sceneCommonIndexFormat, proceduralShapesData[i].numIndices);
	for (i = 0; i < loadedModelsCount; ++i)
		sharedIndexStorage += a3indexStorageSpaceRequired(sceneCommonIndexFormat, loadedModelsData[i].numIndices);
	for (i = 0; i < morphModelsCount; ++i)
		sharedIndexStorage += a3indexStorageSpaceRequired(sceneCommonIndexFormat, morphTargetData[i]->numIndices);

	// create shared buffer
	vbo_ibo = demoState->vbo_staticSceneObjectDrawBuffer;
	a3bufferCreateSplit(vbo_ibo, "vbo/ibo:scene", a3buffer_vertex, sharedVertexStorage, sharedIndexStorage, 0, 0);
	sharedVertexStorage = 0;


	// create vertex formats and drawables
	// grid: position attribute only
	// overlay objects are also just position
	vao = demoState->vao_position;
	a3geometryGenerateVertexArray(vao, "vao:pos", displayShapesData + 1, vbo_ibo, sharedVertexStorage);
	currentDrawable = demoState->draw_grid;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, displayShapesData + 1, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);
	currentDrawable = demoState->draw_bone;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, displayShapesData + 2, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);
	currentDrawable = demoState->draw_joint;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, displayShapesData + 3, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);
	currentDrawable = demoState->draw_pointlight;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, hiddenShapesData + 0, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);

	// axes: position and color
	vao = demoState->vao_position_color;
	a3geometryGenerateVertexArray(vao, "vao:pos+col", displayShapesData + 0, vbo_ibo, sharedVertexStorage);
	currentDrawable = demoState->draw_axes;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, displayShapesData + 0, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);

	// skybox, unit quad: position and texture coordinates
	vao = demoState->vao_position_texcoord;
	a3geometryGenerateVertexArray(vao, "vao:pos+tex", displayShapesData + 4, vbo_ibo, sharedVertexStorage);
	currentDrawable = demoState->draw_skybox;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, displayShapesData + 4, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);
	currentDrawable = demoState->draw_unitquad;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, displayShapesData + 5, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);

	// scene objects: position, texture coordinates and normal
	//	alternatively, complete tangent basis
//	vao = demoState->vao_position_texcoord_normal;
	vao = demoState->vao_tangentBasis;
//	a3geometryGenerateVertexArray(vao, "vao:pos+tex+nrm", proceduralShapesData + 0, vbo_ibo, sharedVertexStorage);
	a3geometryGenerateVertexArray(vao, "vao:tangentBasis", proceduralShapesData + 0, vbo_ibo, sharedVertexStorage);
	currentDrawable = demoState->draw_plane;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, proceduralShapesData + 0, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);
	currentDrawable = demoState->draw_sphere;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, proceduralShapesData + 1, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);
	currentDrawable = demoState->draw_cylinder;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, proceduralShapesData + 2, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);
	currentDrawable = demoState->draw_torus;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, proceduralShapesData + 3, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);
	currentDrawable = demoState->draw_teapot;
	sharedVertexStorage += a3geometryGenerateDrawable(currentDrawable, loadedModelsData + 0, vao, vbo_ibo, sceneCommonIndexFormat, 0, 0);

	// morphing objects: manually configure with partial tangent basis and duplicate attributes
	vao = demoState->vao_tangentBasis_morph5;
	a3vertexArrayCreateDescriptor(vao, "vao:tangentBasis-morph5", vbo_ibo, morphVertexFormat, sharedVertexStorage);
	currentDrawable = demoState->draw_teapot_morph;
	a3indexBufferStore(vbo_ibo, sceneCommonIndexFormat, morphTargetData[0]->indexData, morphTargetData[0]->numIndices, 0, &sharedIndexStorage, 0);
	sharedVertexStorage += a3vertexBufferStore(vbo_ibo, morphVertexFormat, morphVertexAttribData, morphTargetData[0]->numVertices, 0);
	a3vertexDrawableCreateIndexed(currentDrawable, vao, vbo_ibo, sceneCommonIndexFormat, a3prim_triangles, sharedIndexStorage, morphTargetData[0]->numIndices);


	// release data when done
	for (i = 0; i < displayShapesCount; ++i)
		a3geometryReleaseData(displayShapesData + i);
	for (i = 0; i < hiddenShapesCount; ++i)
		a3geometryReleaseData(hiddenShapesData + i);
	for (i = 0; i < proceduralShapesCount; ++i)
		a3geometryReleaseData(proceduralShapesData + i);
	for (i = 0; i < loadedModelsCount; ++i)
		a3geometryReleaseData(loadedModelsData + i);


	// dummy
	a3demo_initDummyDrawable_internal(demoState);
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
		"uP_inv",
		"uMV_nrm",
		"uMVPB",
		"uMVPB_proj",
		"uAtlas",

		// common fragment
		"uLightCt",
		"uLightPos",
		"uLightCol",
		"uLightSz",
		"uPixelSz",
		"uColor",

		// named textures
		"uTex_dm","uTex_sm","uTex_nm","uTex_hm","uTex_dm_ramp","uTex_sm_ramp","uTex_proj","uTex_shadow",

		// generic images
		"uImage0","uImage1","uImage2","uImage3","uImage4","uImage5","uImage6","uImage7",

		// common general
		"uIndex",
		"uTime",
	};

	// list of uniform block names: align with uniform block list in demo struct!
	const a3byte *uniformBlockNames[demoStateMaxCount_shaderProgramUniformBlock] = {
		// transformation uniform blocks
		"ubTransformLMVPB",
		"ubTransformLMVP",
		"ubTransformMVPB",
		"ubTransformMVP",

		// lighting uniform blocks
		"ubPointLight",

		// curve uniform blocks
		"ubCurveWaypoint",
		"ubCurveHandle",

		// skeletal uniform blocks
		"ubTransformBindPoseToCurrentPose",
	};


	// some default uniform values
	const a3f32 defaultColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const a3i32 defaultTexUnits[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
	const a3f64 defaultDouble[] = { 0.0 };
	const a3f32 defaultFloat[] = { 0.0f };
	const a3i32 defaultInt[] = { 0 };


	// list of all unique shaders
	// this is a good idea to avoid multi-loading 
	//	those that are shared between programs
	union {
		struct {
			// vertex shaders
			// base
			a3_DemoStateShader
				passthru_vs[1],
				passthru_transform_vs[1],
				passColor_transform_vs[1],
				passthru_transform_instanced_vs[1],
				passColor_transform_instanced_vs[1];
			// 02-shading
			a3_DemoStateShader
				passTexcoord_transform_vs[1],
				passPhongAttribs_transform_vs[1];
			// 04-shadow
			a3_DemoStateShader
				passPhongAttribs_shadowCoord_transform_vs[1];
			// 06-deferred
			a3_DemoStateShader
				passPhongAttribs_transform_atlas_vs[1],
				passBiasClipCoord_transform_instanced_vs[1];
			// 07-curves
			a3_DemoStateShader
				passTangentBasis_transform_vs[1],
				passInstanceID_vs[1];
			// 08-morphing
			a3_DemoStateShader
				passTangentBasis_transform_morph_vs[1];
			// 09-skeletal
			a3_DemoStateShader
				passTangentBasis_transform_skin_vs[1];

			// geometry shaders
			// 07-curves
			a3_DemoStateShader
				manipTriangle_gs[1],
				drawTangentBasis_gs[1],
				drawCurveSegment_gs[1],
				drawCurveHandles_gs[1];

			// fragment shaders
			// base
			a3_DemoStateShader
				drawColorUnif_fs[1],
				drawColorAttrib_fs[1];
			// 02-shading
			a3_DemoStateShader
				drawTexture_fs[1],
				drawPhongMulti_fs[1],
				drawNonPhotoMulti_fs[1];
			// 03-framebuffer
			a3_DemoStateShader
				drawPhongMulti_mrt_fs[1],
				drawCustom_mrt_fs[1];
			// 04-shadow
			a3_DemoStateShader
				drawPhongMulti_projtex_fs[1],
				drawPhongMulti_shadowmap_fs[1],
				drawPhongMulti_shadowmap_projtex_fs[1],
				drawCustom_post_fs[1];
			// 05-bloom
			a3_DemoStateShader
				drawBrightPass_fs[1],
				drawBlurGaussian_fs[1],
				drawBlendComposite_fs[1];
			// 06-deferred
			a3_DemoStateShader
				drawGBuffers_fs[1],
				drawPhongMulti_deferred_fs[1],
				drawPhong_volume_fs[1],
				drawDeferredLightingComposite_fs[1];
			// 08-morphing
			a3_DemoStateShader
				drawPhongMulti_nm_fs[1];
		};
	} shaderList = {
		{
			// ****REMINDER: 'Encoded' shaders are available for proof-of-concept
			//	testing ONLY!  Insert /e before file names.
			// DO NOT SUBMIT WORK USING ENCODED SHADERS OR YOU WILL GET ZERO!!!

			// vs
			// base
			{ { { 0 },	"shdr-vs:passthru",				a3shader_vertex  ,	1,{ "../../../../resource/glsl/4x/vs/e/passthru_vs4x.glsl" } } },
			{ { { 0 },	"shdr-vs:passthru-trans",		a3shader_vertex  ,	1,{ "../../../../resource/glsl/4x/vs/e/passthru_transform_vs4x.glsl" } } },
			{ { { 0 },	"shdr-vs:pass-col-trans",		a3shader_vertex  ,	1,{ "../../../../resource/glsl/4x/vs/e/passColor_transform_vs4x.glsl" } } },
			{ { { 0 },	"shdr-vs:passthru-trans-inst",	a3shader_vertex  ,	1,{ "../../../../resource/glsl/4x/vs/passthru_transform_instanced_vs4x.glsl" } } },
			{ { { 0 },	"shdr-vs:pass-col-trans-inst",	a3shader_vertex  ,	1,{ "../../../../resource/glsl/4x/vs/passColor_transform_instanced_vs4x.glsl" } } },
			// 02-shading
			{ { { 0 },	"shdr-vs:pass-tex",				a3shader_vertex  ,	1,{ "../../../../resource/glsl/4x/vs/02-shading/e/passTexcoord_transform_vs4x.glsl" } } },
			{ { { 0 },	"shdr-vs:pass-Phong",			a3shader_vertex  ,	1,{ "../../../../resource/glsl/4x/vs/02-shading/e/passPhongAttribs_transform_vs4x.glsl" } } },
			// 04-shadow
			{ { { 0 },	"shdr-vs:pass-Phong-shadow",	a3shader_vertex  ,	1,{ "../../../../resource/glsl/4x/vs/04-shadow/e/passPhongAttribs_passShadowCoord_transform_vs4x.glsl" } } },
			// 06-deferred
			{ { { 0 },	"shdr-vs:pass-Phong-atlas",		a3shader_vertex  ,	1,{ "../../../../resource/glsl/4x/vs/06-deferred/e/passPhongAttribs_transform_atlas_vs4x.glsl" } } },
			{ { { 0 },	"shdr-vs:pass-biasclip",		a3shader_vertex  ,	1,{ "../../../../resource/glsl/4x/vs/06-deferred/e/passBiasClipCoord_transform_instanced_vs4x.glsl" } } },
			// 07-curves
			{ { { 0 },	"shdr-vs:pass-tangent-basis",	a3shader_vertex  ,	1,{ "../../../../resource/glsl/4x/vs/07-curves/e/passTangentBasis_transform_vs4x.glsl" } } },
			{ { { 0 },	"shdr-vs:pass-instance-id",		a3shader_vertex  ,	1,{ "../../../../resource/glsl/4x/vs/07-curves/e/passInstanceID_vs4x.glsl" } } },
			// 08-morphing
			{ { { 0 },	"shdr-vs:pass-tangent-morph",	a3shader_vertex  ,	1,{ "../../../../resource/glsl/4x/vs/08-morphing/e/passTangentBasis_transform_morph_vs4x.glsl" } } },
			// 09-skeletal
			{ { { 0 },	"shdr-vs:pass-tangent-skin",	a3shader_vertex  ,	1,{ "../../../../resource/glsl/4x/vs/09-skeletal/passTangentBasis_transform_skin_vs4x.glsl" } } },

			// gs
			// 07-curves
			{ { { 0 },	"shdr-gs:manip-triangle",		a3shader_geometry,	1,{ "../../../../resource/glsl/4x/gs/07-curves/e/manipTriangle_gs4x.glsl" } } },
			{ { { 0 },	"shdr-gs:draw-tangent-basis",	a3shader_geometry,	1,{ "../../../../resource/glsl/4x/gs/07-curves/e/drawTangentBasis_gs4x.glsl" } } },
			{ { { 0 },	"shdr-gs:draw-curve-segment",	a3shader_geometry,	1,{ "../../../../resource/glsl/4x/gs/07-curves/e/drawCurveSegment_gs4x.glsl" } } },
			{ { { 0 },	"shdr-gs:draw-curve-handles",	a3shader_geometry,	1,{ "../../../../resource/glsl/4x/gs/07-curves/e/drawCurveHandles_gs4x.glsl" } } },

			// fs
			// base
			{ { { 0 },	"shdr-fs:draw-col-unif",		a3shader_fragment,	1,{ "../../../../resource/glsl/4x/fs/e/drawColorUnif_fs4x.glsl" } } },
			{ { { 0 },	"shdr-fs:draw-col-attr",		a3shader_fragment,	1,{ "../../../../resource/glsl/4x/fs/e/drawColorAttrib_fs4x.glsl" } } },
			// 02-shading
			{ { { 0 },	"shdr-fs:draw-tex",				a3shader_fragment,	1,{ "../../../../resource/glsl/4x/fs/02-shading/e/drawTexture_fs4x.glsl" } } },
			{ { { 0 },	"shdr-fs:draw-Phong-multi",		a3shader_fragment,	1,{ "../../../../resource/glsl/4x/fs/02-shading/e/drawPhongMulti_fs4x.glsl" } } },
			{ { { 0 },	"shdr-fs:draw-nonphoto-multi",	a3shader_fragment,	1,{ "../../../../resource/glsl/4x/fs/02-shading/e/drawNonPhotoMulti_fs4x.glsl" } } },
			// 03-framebuffer
			{ { { 0 },	"shdr-fs:draw-Phong-multi-mrt",	a3shader_fragment,	1,{ "../../../../resource/glsl/4x/fs/03-framebuffer/e/drawPhongMulti_mrt_fs4x.glsl" } } },
			{ { { 0 },	"shdr-fs:draw-custom-mrt",		a3shader_fragment,	1,{ "../../../../resource/glsl/4x/fs/03-framebuffer/e/drawCustom_mrt_fs4x.glsl" } } },
			// 04-shadow
			{ { { 0 },	"shdr-fs:draw-Phong-projtex",	a3shader_fragment,	1,{ "../../../../resource/glsl/4x/fs/04-shadow/e/drawPhongMulti_projective_fs4x.glsl" } } },
			{ { { 0 },	"shdr-fs:draw-Phong-shadow",	a3shader_fragment,	1,{ "../../../../resource/glsl/4x/fs/04-shadow/e/drawPhongMulti_shadowmap_fs4x.glsl" } } },
			{ { { 0 },	"shdr-fs:draw-Phong-shadproj",	a3shader_fragment,	1,{ "../../../../resource/glsl/4x/fs/04-shadow/e/drawPhongMulti_shadowmap_projective_fs4x.glsl" } } },
			{ { { 0 },	"shdr-fs:draw-custom-post",		a3shader_fragment,	1,{ "../../../../resource/glsl/4x/fs/04-shadow/e/drawCustom_post_fs4x.glsl" } } },
			// 05-bloom
			{ { { 0 },	"shdr-fs:draw-brightpass",		a3shader_fragment,	1,{ "../../../../resource/glsl/4x/fs/05-bloom/e/drawBrightPass_fs4x.glsl" } } },
			{ { { 0 },	"shdr-fs:draw-blur-Gaussian",	a3shader_fragment,	1,{ "../../../../resource/glsl/4x/fs/05-bloom/e/drawBlurGaussian_fs4x.glsl" } } },
			{ { { 0 },	"shdr-fs:draw-blend-composite",	a3shader_fragment,	1,{ "../../../../resource/glsl/4x/fs/05-bloom/e/drawBlendComposite_fs4x.glsl" } } },
			// 06-deferred
			{ { { 0 },	"shdr-fs:draw-gbuffers",		a3shader_fragment,	1,{ "../../../../resource/glsl/4x/fs/06-deferred/e/drawGBuffers_fs4x.glsl" } } },
			{ { { 0 },	"shdr-fs:draw-Phong-deferred",	a3shader_fragment,	1,{ "../../../../resource/glsl/4x/fs/06-deferred/e/drawPhongMulti_deferred_fs4x.glsl" } } },
			{ { { 0 },	"shdr-fs:draw-Phong-volume",	a3shader_fragment,	1,{ "../../../../resource/glsl/4x/fs/06-deferred/e/drawPhong_volume_fs4x.glsl" } } },
			{ { { 0 },	"shdr-fs:draw-deferltcomp",		a3shader_fragment,	1,{ "../../../../resource/glsl/4x/fs/06-deferred/e/drawDeferredLightingComposite_fs4x.glsl" } } },
			// 08-morphing
			{ { { 0 },	"shdr-fs:draw-Phong-nm",		a3shader_fragment,	1,{ "../../../../resource/glsl/4x/fs/08-morphing/e/drawPhongMulti_nm_fs4x.glsl" } } },
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

	// base programs: 
	// uniform color program
	currentDemoProg = demoState->prog_drawColorUnif;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-col-unif");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passthru_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawColorUnif_fs->shader);
	// color attrib program
	currentDemoProg = demoState->prog_drawColorAttrib;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-col-attr");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passColor_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawColorAttrib_fs->shader);
	// uniform color program with instancing
	currentDemoProg = demoState->prog_drawColorUnif_instanced;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-col-unif-inst");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passthru_transform_instanced_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawColorUnif_fs->shader);
	// color attrib program with instancing
	currentDemoProg = demoState->prog_drawColorAttrib_instanced;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-col-attr-inst");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passColor_transform_instanced_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawColorAttrib_fs->shader);

	// 02-shading programs: 
	// texturing
	currentDemoProg = demoState->prog_drawTexture;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-tex");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passTexcoord_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawTexture_fs->shader);
	// Phong shading
	currentDemoProg = demoState->prog_drawPhongMulti;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-Phong-multi");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passPhongAttribs_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawPhongMulti_fs->shader);
	// non-photorealistic shading
	currentDemoProg = demoState->prog_drawNonPhotoMulti;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-nonphoto-multi");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passPhongAttribs_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawNonPhotoMulti_fs->shader);

	// 03-framebuffer programs: 
	// Phong shading MRT
	currentDemoProg = demoState->prog_drawPhongMulti_mrt;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-Phong-multi-mrt");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passPhongAttribs_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawPhongMulti_mrt_fs->shader);
	// custom effects MRT
	currentDemoProg = demoState->prog_drawCustom_mrt;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-custom-mrt");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passPhongAttribs_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawCustom_mrt_fs->shader);

	// 04-shadow programs: 
	// transform only
	currentDemoProg = demoState->prog_transform;
	a3shaderProgramCreate(currentDemoProg->program, "prog:transform");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passthru_transform_vs->shader);
	// projective texturing
	currentDemoProg = demoState->prog_drawPhongMulti_projtex;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-Phong-projtex");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passPhongAttribs_shadowCoord_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawPhongMulti_projtex_fs->shader);
	// shadow mapping
	currentDemoProg = demoState->prog_drawPhongMulti_shadowmap;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-Phong-shadow");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passPhongAttribs_shadowCoord_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawPhongMulti_shadowmap_fs->shader);
	// shadow mapping and projective texturing
	currentDemoProg = demoState->prog_drawPhongMulti_shadowmap_projtex;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-Phong-shadproj");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passPhongAttribs_shadowCoord_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawPhongMulti_shadowmap_projtex_fs->shader);
	// custom post-processing
	currentDemoProg = demoState->prog_drawCustom_post;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-custom-post");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passTexcoord_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawCustom_post_fs->shader);

	// 05-bloom programs: 
	// bright pass
	currentDemoProg = demoState->prog_drawBrightPass;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-brightpass");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passTexcoord_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawBrightPass_fs->shader);
	// Gaussian blur
	currentDemoProg = demoState->prog_drawBlurGaussian;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-blur-Gaussian");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passTexcoord_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawBlurGaussian_fs->shader);
	// blending and composition
	currentDemoProg = demoState->prog_drawBlendComposite;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-blend-composite");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passTexcoord_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawBlendComposite_fs->shader);

	// 06-deferred programs: 
	// g-buffers
	currentDemoProg = demoState->prog_drawGBuffers;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-gbuffers");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passPhongAttribs_transform_atlas_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawGBuffers_fs->shader);
	// deferred Phong shading
	currentDemoProg = demoState->prog_drawPhongMulti_deferred;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-Phong-deferred");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passTexcoord_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawPhongMulti_deferred_fs->shader);
	// Phong light volume
	currentDemoProg = demoState->prog_drawPhong_volume;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-Phong-volume");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passBiasClipCoord_transform_instanced_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawPhong_volume_fs->shader);
	// deferred lighting composite
	currentDemoProg = demoState->prog_drawDeferredLightingComposite;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-deferltcomp");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passTexcoord_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawDeferredLightingComposite_fs->shader);

	// 07-curves programs: 
	// Phong shading with triangle manipulation
	currentDemoProg = demoState->prog_drawPhongMulti_manip;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-Phong-multi-manip");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passPhongAttribs_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.manipTriangle_gs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawPhongMulti_fs->shader);
	// draw tangent basis
	currentDemoProg = demoState->prog_drawTangentBasis;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-tangent-basis");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passTangentBasis_transform_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawTangentBasis_gs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawColorAttrib_fs->shader);
	// draw curve segment
	currentDemoProg = demoState->prog_drawCurveSegment;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-curve-segment");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passInstanceID_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawCurveSegment_gs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawColorUnif_fs->shader);
	// draw curve handles
	currentDemoProg = demoState->prog_drawCurveHandles;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-curve-handles");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passInstanceID_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawCurveHandles_gs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawColorUnif_fs->shader);

	// 08-morphing programs: 
	// Phong shading with morph animation
	currentDemoProg = demoState->prog_drawPhongMulti_morph;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-Phong-morph");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passTangentBasis_transform_morph_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawPhongMulti_nm_fs->shader);
	// tangent basis with morph animation
	currentDemoProg = demoState->prog_drawTangentBasis_morph;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-tangent-morph");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passTangentBasis_transform_morph_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawTangentBasis_gs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawColorAttrib_fs->shader);
/*
	// 09-skeletal programs: 
	// Phong shading with skeletal animation
	currentDemoProg = demoState->prog_drawPhongMulti_skin;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-Phong-skin");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passTangentBasis_transform_skin_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawPhongMulti_nm_fs->shader);
	// tangent basis with skeletal animation
	currentDemoProg = demoState->prog_drawTangentBasis_skin;
	a3shaderProgramCreate(currentDemoProg->program, "prog:draw-tangent-skin");
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.passTangentBasis_transform_skin_vs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawTangentBasis_gs->shader);
	a3shaderProgramAttachShader(currentDemoProg->program, shaderList.drawColorAttrib_fs->shader);
*/

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
		currentUnif = currentDemoProg->uniformBlockLocation;
		for (j = 0; j < demoStateMaxCount_shaderProgramUniformBlock; ++j)
			currentUnif[j] = a3shaderUniformBlockGetLocation(currentDemoProg->program, uniformBlockNames[j]);


		// set default values for all programs that have a uniform that will 
		//	either never change or is consistent for all programs

		// common VS
		if ((uLocation = currentDemoProg->uMVP) >= 0)
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, uLocation, 1, a3identityMat4.mm);
		if ((uLocation = currentDemoProg->uMV) >= 0)
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, uLocation, 1, a3identityMat4.mm);
		if ((uLocation = currentDemoProg->uP) >= 0)
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, uLocation, 1, a3identityMat4.mm);
		if ((uLocation = currentDemoProg->uP_inv) >= 0)
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, uLocation, 1, a3identityMat4.mm);
		if ((uLocation = currentDemoProg->uMV_nrm) >= 0)
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, uLocation, 1, a3identityMat4.mm);
		if ((uLocation = currentDemoProg->uMVPB) >= 0)
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, uLocation, 1, a3identityMat4.mm);
		if ((uLocation = currentDemoProg->uMVPB_proj) >= 0)
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
		if ((uLocation = currentDemoProg->uPixelSz) >= 0)
			a3shaderUniformSendFloat(a3unif_vec2, uLocation, 1, a3oneVec2.v);
		if ((uLocation = currentDemoProg->uColor) >= 0)
			a3shaderUniformSendFloat(a3unif_vec4, uLocation, 1, defaultColor);

		// named textures
		if ((uLocation = currentDemoProg->uTex_dm) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 0);
		if ((uLocation = currentDemoProg->uTex_sm) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 1);
		if ((uLocation = currentDemoProg->uTex_nm) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 2);
		if ((uLocation = currentDemoProg->uTex_hm) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 3);
		if ((uLocation = currentDemoProg->uTex_dm_ramp) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 4);
		if ((uLocation = currentDemoProg->uTex_sm_ramp) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 5);
		if ((uLocation = currentDemoProg->uTex_proj) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 6);
		if ((uLocation = currentDemoProg->uTex_shadow) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 7);

		// generic images
		if ((uLocation = currentDemoProg->uImage0) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 0);
		if ((uLocation = currentDemoProg->uImage1) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 1);
		if ((uLocation = currentDemoProg->uImage2) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 2);
		if ((uLocation = currentDemoProg->uImage3) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 3);
		if ((uLocation = currentDemoProg->uImage4) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 4);
		if ((uLocation = currentDemoProg->uImage5) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 5);
		if ((uLocation = currentDemoProg->uImage6) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 6);
		if ((uLocation = currentDemoProg->uImage7) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultTexUnits + 7);

		// common general
		if ((uLocation = currentDemoProg->uIndex) >= 0)
			a3shaderUniformSendInt(a3unif_single, uLocation, 1, defaultInt);
		if ((uLocation = currentDemoProg->uTime) >= 0)
			a3shaderUniformSendDouble(a3unif_single, uLocation, 1, defaultDouble);

		// transformation uniform blocks
		if ((uLocation = currentDemoProg->ubTransformLMVPB) >= 0)
			a3shaderUniformBlockBind(currentDemoProg->program, uLocation, 3);
		if ((uLocation = currentDemoProg->ubTransformLMVP) >= 0)
			a3shaderUniformBlockBind(currentDemoProg->program, uLocation, 2);
		if ((uLocation = currentDemoProg->ubTransformMVPB) >= 0)
			a3shaderUniformBlockBind(currentDemoProg->program, uLocation, 1);
		if ((uLocation = currentDemoProg->ubTransformMVP) >= 0)
			a3shaderUniformBlockBind(currentDemoProg->program, uLocation, 0);

		// lighting uniform blocks
		if ((uLocation = currentDemoProg->ubPointLight) >= 0)
			a3shaderUniformBlockBind(currentDemoProg->program, uLocation, 2);

		// curve uniform blocks
		if ((uLocation = currentDemoProg->ubCurveHandle) >= 0)
			a3shaderUniformBlockBind(currentDemoProg->program, uLocation, 1);
		if ((uLocation = currentDemoProg->ubCurveWaypoint) >= 0)
			a3shaderUniformBlockBind(currentDemoProg->program, uLocation, 0);

		// skeletal uniform blocks
		if ((uLocation = currentDemoProg->ubTransformBindPoseToCurrentPose) >= 0)
			a3shaderUniformBlockBind(currentDemoProg->program, uLocation, 3);
	}


	// set up lighting uniform buffers
	for (i = 0; i < demoStateMaxCount_lightVolumeBlock; ++i)
	{
		a3bufferCreate(demoState->ubo_transformMVP_light + i, "ubo:transform-mvp", a3buffer_uniform, a3index_countMaxShort, 0);
		a3bufferCreate(demoState->ubo_transformMVPB_light + i, "ubo:transform-mvpb", a3buffer_uniform, a3index_countMaxShort, 0);
		a3bufferCreate(demoState->ubo_pointLight + i, "ubo:pointlight", a3buffer_uniform, a3index_countMaxShort, 0);
	}

	// set up curve uniform buffers
	a3bufferCreate(demoState->ubo_curveWaypoint, "ubo:curve-waypoint", a3buffer_uniform, a3index_countMaxShort, 0);
	a3bufferCreate(demoState->ubo_curveHandle, "ubo:curve-handle", a3buffer_uniform, a3index_countMaxShort, 0);

	// set up skeletal uniform buffers
	a3bufferCreate(demoState->ubo_transformLMVP_bone, "ubo:transform-lmvp-bone", a3buffer_uniform, a3index_countMaxShort, 0);
	a3bufferCreate(demoState->ubo_transformLMVP_joint, "ubo:transform-lmvp-joint", a3buffer_uniform, a3index_countMaxShort, 0);
	a3bufferCreate(demoState->ubo_transformBindPoseToCurrentPose_joint, "ubo:transform-bind2curr-joint", a3buffer_uniform, a3index_countMaxShort, 0);


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
			a3_DemoStateTexture texAtlasDM[1];
			a3_DemoStateTexture texAtlasSM[1];
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
			{ demoState->tex_atlas_dm,		"tex:atlas-dm",		"../../../../resource/tex/atlas/atlas_scene_dm.png" },
			{ demoState->tex_atlas_sm,		"tex:atlas-sm",		"../../../../resource/tex/atlas/atlas_scene_sm.png" },
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
	// skyboxes
	for (i = 0; i < 2; ++i, ++tex)
	{
		a3textureActivate(tex, a3tex_unit00);
		a3textureChangeFilterMode(a3tex_filterLinear);	// linear pixel blending
	}
	// atlases
	for (i = 0; i < 2; ++i, ++tex)
	{
		a3textureActivate(tex, a3tex_unit00);
		a3textureChangeFilterMode(a3tex_filterLinear);
	}
	// stone and planets
	for (i = 0; i < 5; ++i, ++tex)
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


	// set up texture atlas transforms
	a3demo_setAtlasTransform_internal(demoState->atlas_stone->m, atlasSceneWidth, atlasSceneHeight,
		1600, 0, 256, 256, atlasSceneBorderPad, atlasSceneAdditionalPad);
	a3demo_setAtlasTransform_internal(demoState->atlas_earth->m, atlasSceneWidth, atlasSceneHeight,
		0, 0, 1024, 512, atlasSceneBorderPad, atlasSceneAdditionalPad);
	a3demo_setAtlasTransform_internal(demoState->atlas_mars->m, atlasSceneWidth, atlasSceneHeight,
		0, 544, 1024, 512, atlasSceneBorderPad, atlasSceneAdditionalPad);
	a3demo_setAtlasTransform_internal(demoState->atlas_checker->m, atlasSceneWidth, atlasSceneHeight,
		1888, 0, 128, 128, atlasSceneBorderPad, atlasSceneAdditionalPad);


	// done
	a3textureDeactivate(a3tex_unit00);
}


// utility to load framebuffers
void a3demo_loadFramebuffers(a3_DemoState *demoState)
{
	// create framebuffers and change their texture settings if need be
	a3_Framebuffer *fbo;
	a3_FramebufferDouble *fbo_dbl;
	a3ui32 i, j;

	// storage precision
	const a3_FramebufferColorType colorType_scene = a3fbo_colorRGBA32F;
	const a3_FramebufferDepthType depthType_scene = a3fbo_depth24_stencil8;
	const a3_FramebufferColorType colorType_comp = a3fbo_colorRGBA16;
	const a3_FramebufferColorType colorType_post = a3fbo_colorRGBA16;

	// other settings
	const a3ui16 shadowMapSz = 2048;


	// initialize framebuffers: 
	//	- scene, with or without MRT (determine your needs), add depth
	//	- shadow map, depth only
	fbo = demoState->fbo_scene;
	a3framebufferCreate(fbo, "fbo:scene",
		4, colorType_scene, depthType_scene,
		demoState->frameWidth, demoState->frameHeight);

	fbo = demoState->fbo_shadowmap;
	a3framebufferCreate(fbo, "fbo:shadowmap", 
		0, a3fbo_colorDisable, a3fbo_depth24, 
		shadowMapSz, shadowMapSz);


	// double framebuffers: 
	//	- half, quarter and eighth of frame size
	//	- there are two of each because of how they will be used (see render algorithm)
	for (i = 0; i < 2; ++i)
	{
		fbo_dbl = demoState->fbo_dbl_nodepth + i;
		a3framebufferDoubleCreate(fbo_dbl, "fbo-dbl:nodepth",
			2, colorType_post, a3fbo_depthDisable,
			demoState->frameWidth, demoState->frameHeight);

		fbo_dbl = demoState->fbo_dbl_nodepth_2 + i;
		a3framebufferDoubleCreate(fbo_dbl, "fbo-dbl:nodepth-1/2",
			1, colorType_post, a3fbo_depthDisable,
			demoState->frameWidth / 2, demoState->frameHeight / 2);

		fbo_dbl = demoState->fbo_dbl_nodepth_4 + i;
		a3framebufferDoubleCreate(fbo_dbl, "fbo-dbl:nodepth-1/4",
			1, colorType_post, a3fbo_depthDisable,
			demoState->frameWidth / 4, demoState->frameHeight / 4);

		fbo_dbl = demoState->fbo_dbl_nodepth_8 + i;
		a3framebufferDoubleCreate(fbo_dbl, "fbo-dbl:nodepth-1/8",
			1, colorType_post, a3fbo_depthDisable,
			demoState->frameWidth / 8, demoState->frameHeight / 8);
	}


	// change texture settings for all framebuffers
	for (i = 0, fbo = demoState->framebuffer;
		i < demoStateMaxCount_framebuffer;
		++i, ++fbo)
	{
		// color, if applicable
		for (j = 0; j < fbo->color; ++j)
		{
			a3framebufferBindColorTexture(fbo, a3tex_unit00, j);
			a3textureChangeRepeatMode(a3tex_repeatClamp, a3tex_repeatClamp);
			a3textureChangeFilterMode(a3tex_filterLinear);
		}

		// depth, if applicable
		if (fbo->depthStencil)
		{
			a3framebufferBindDepthTexture(fbo, a3tex_unit00);
			a3textureChangeRepeatMode(a3tex_repeatClamp, a3tex_repeatClamp);
			a3textureChangeFilterMode(a3tex_filterLinear);
		}
	}

	// ditto for double framebuffers
	for (i = 0, fbo_dbl = demoState->framebuffer_double;
		i < demoStateMaxCount_framebufferDouble;
		++i, ++fbo_dbl)
	{
		// color, if applicable
		for (j = 0; j < fbo_dbl->color; ++j)
		{
			a3framebufferDoubleSwap(fbo_dbl);
			a3framebufferDoubleBindColorTexture(fbo_dbl, a3tex_unit00, j);
			a3textureChangeRepeatMode(a3tex_repeatClamp, a3tex_repeatClamp);
			a3textureChangeFilterMode(a3tex_filterLinear);
			a3framebufferDoubleSwap(fbo_dbl);
			a3framebufferDoubleBindColorTexture(fbo_dbl, a3tex_unit00, j);
			a3textureChangeRepeatMode(a3tex_repeatClamp, a3tex_repeatClamp);
			a3textureChangeFilterMode(a3tex_filterLinear);
		}

		// depth, if applicable
		if (fbo_dbl->depthStencil)
		{
			a3framebufferDoubleSwap(fbo_dbl);
			a3framebufferDoubleBindDepthTexture(fbo_dbl, a3tex_unit00);
			a3textureChangeRepeatMode(a3tex_repeatClamp, a3tex_repeatClamp);
			a3textureChangeFilterMode(a3tex_filterLinear);
			a3framebufferDoubleSwap(fbo_dbl);
			a3framebufferDoubleBindDepthTexture(fbo_dbl, a3tex_unit00);
			a3textureChangeRepeatMode(a3tex_repeatClamp, a3tex_repeatClamp);
			a3textureChangeFilterMode(a3tex_filterLinear);
		}
	}


	// deactivate texture
	a3textureDeactivate(a3tex_unit00);
}


// utility to load animation
void a3demo_loadAnimation(a3_DemoState *demoState)
{
	// general counters
	a3ui32 j, p;

	// object pointers
	a3_Hierarchy *hierarchy;
	a3_HierarchyState *hierarchyState;
	a3_HierarchyPoseGroup *hierarchyPoseGroup;
	a3_HierarchyNodePose *hierarchyNodePose;
	a3_HierarchyPoseFlag *hierarchyPoseFlag;

	a3_FileStream fileStream[1] = { 0 };
	const a3byte *const geometryStream = "./data/anim_data_gpro_skeletal.dat";

	// stream animation assets
	if (demoState->streaming && a3fileStreamOpenRead(fileStream, geometryStream))
	{
		// load hierarchy assets
		hierarchy = demoState->hierarchy_skel;
		a3hierarchyLoadBinary(hierarchy, fileStream);

		// done
		a3fileStreamClose(fileStream);
	}
	// not streaming or stream doesn't exist
	else if (!demoState->streaming || a3fileStreamOpenWrite(fileStream, geometryStream))
	{
		// manually set up a skeleton
		// first is the hierarchy: the general non-spatial relationship between bones
		const a3ui32 jointCount = 32;

		// indices of joints, their parents and branching joints
		a3ui32 jointIndex = 0;
		a3i32 jointParentIndex = -1;
		a3i32 rootJointIndex, upperSpineJointIndex, clavicleJointIndex, pelvisJointIndex;

		// initialize hierarchy
		hierarchy = demoState->hierarchy_skel;
		a3hierarchyCreate(hierarchy, jointCount, 0);

		// set up joint relationships
		jointParentIndex = rootJointIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:root");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:spine_lower");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:spine_mid");
		jointParentIndex = upperSpineJointIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:spine_upper");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:neck");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:head");
		jointParentIndex = upperSpineJointIndex;
		jointParentIndex = clavicleJointIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:clavicle");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:shoulderblade_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:shoulder_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:elbow_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:forearm_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:wrist_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:hand_r");
		jointParentIndex = clavicleJointIndex;
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:shoulderblade_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:shoulder_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:elbow_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:forearm_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:wrist_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:hand_l");
		jointParentIndex = rootJointIndex;
		jointParentIndex = pelvisJointIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:pelvis");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:hip_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:knee_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:shin_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:ankle_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:foot_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:toe_r");
		jointParentIndex = pelvisJointIndex;
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:hip_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:knee_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:shin_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:ankle_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:foot_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:toe_l");

		// save hierarchy assets
		a3hierarchySaveBinary(hierarchy, fileStream);

		// done
		a3fileStreamClose(fileStream);
	}


	// next set up hierarchy poses
	demoState->editSkeletonIndex = 0;
	hierarchy = demoState->hierarchy_skel + demoState->editSkeletonIndex;
	hierarchyPoseGroup = demoState->hierarchyPoseGroup_skel + demoState->editSkeletonIndex;
	a3hierarchyPoseGroupCreate(hierarchyPoseGroup, hierarchy, 1);
	hierarchyPoseFlag = demoState->hierarchyPoseFlag_skel[demoState->editSkeletonIndex];
	
	p = 0;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:root");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, 0.0f, 0.0, +3.6f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate | a3poseFlag_translate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:spine_lower");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, -90.0f, -5.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, 0.0f, -0.1f, +0.1f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:spine_mid");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, 0.0f, +10.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +2.0f, 0.0f, 0.0f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:spine_upper");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, 0.0f, -5.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +2.0f, 0.0f, 0.0f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:neck");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +0.5f, 0.0f, 0.0f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:head");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +0.5f, 0.0f, 0.0f);
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:clavicle");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, +90.0f, 0.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, -0.1f, +0.1f, 0.0f);
//	j = a3hierarchyGetNodeIndex(hierarchy, "skel:pelvis");
//	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
//	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, 0.0f, +0.1f, -0.1f);

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulderblade_r");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +0.5f, -0.1f, -0.5f);
	hierarchyPoseFlag[j] = a3poseFlag_translate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulder_r");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, +30.0f, -10.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +0.5f, 0.0f, +0.5f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:elbow_r");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, 0.0f, +20.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +2.0f, 0.0f, 0.0f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:forearm_r");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +1.0f, 0.0f, 0.0f);
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:wrist_r");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, 0.0f, -10.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +1.0f, 0.0f, 0.0f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:hand_r");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +0.5f, 0.0f, 0.0f);

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulderblade_l");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, -0.5f, -0.1f, -0.5f);
	hierarchyPoseFlag[j] = a3poseFlag_translate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulder_l");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, -30.0f, +10.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, -0.5f, 0.0f, +0.5f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:elbow_l");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, 0.0f, -20.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, -2.0f, 0.0f, 0.0f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:forearm_l");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, -1.0f, 0.0f, 0.0f);
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:wrist_l");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, 0.0f, +10.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, -1.0f, 0.0f, 0.0f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:hand_l");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, -0.5f, 0.0f, 0.0f);

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:hip_r");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, +90.0f, +10.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +1.0f, -0.1f, +0.5f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:knee_r");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, 0.0f, -20.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +2.0f, 0.0f, 0.0f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:shin_r");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +1.0f, 0.0f, 0.0f);
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:ankle_r");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, 0.0f, +90.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +1.0f, 0.0f, 0.0f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:foot_r");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, 0.0f, +10.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +0.5f, 0.0f, 0.0f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:toe_r");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +0.5f, 0.0f, 0.0f);

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:hip_l");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, -90.0f, -10.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, -1.0f, -0.1f, +0.5f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:knee_l");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, 0.0f, +20.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, -2.0f, 0.0f, 0.0f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:shin_l");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, -1.0f, 0.0f, 0.0f);
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:ankle_l");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, 0.0f, -90.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, -1.0f, 0.0f, 0.0f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:foot_l");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, 0.0f, -10.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, -0.5f, 0.0f, 0.0f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:toe_l");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, -0.5f, 0.0f, 0.0f);


	// finally set up hierarchy states
	hierarchyPoseGroup = demoState->hierarchyPoseGroup_skel + demoState->editSkeletonIndex;
	hierarchyState = demoState->hierarchyState_skel + demoState->editSkeletonIndex;
	a3hierarchyStateCreate(hierarchyState, hierarchyPoseGroup);
}


//-----------------------------------------------------------------------------

// internal utility for refreshing drawable
inline void a3_refreshDrawable_internal(a3_VertexDrawable *drawable, a3_VertexArrayDescriptor *vertexArray, a3_IndexBuffer *indexBuffer)
{
	drawable->vertexArray = vertexArray;
	if (drawable->indexType)
		drawable->indexBuffer = indexBuffer;
}


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
	a3_UniformBuffer *currentUBO = demoState->uniformBuffer,
		*const endUBO = currentUBO + demoStateMaxCount_uniformBuffer;
	a3_Texture *currentTex = demoState->texture,
		*const endTex = currentTex + demoStateMaxCount_texture;
	a3_Framebuffer *currentFBO = demoState->framebuffer,
		*const endFBO = currentFBO + demoStateMaxCount_framebuffer;
	a3_FramebufferDouble *currentDFBO = demoState->framebuffer_double,
		*const endDFBO = currentDFBO + demoStateMaxCount_framebufferDouble;

	// set pointers to appropriate release callback for different asset types
	while (currentBuff < endBuff)
		a3bufferHandleUpdateReleaseCallback(currentBuff++);
	while (currentVAO < endVAO)
		a3vertexArrayHandleUpdateReleaseCallback(currentVAO++);
	while (currentProg < endProg)
		a3shaderProgramHandleUpdateReleaseCallback((currentProg++)->program);
	while (currentUBO < endUBO)
		a3bufferHandleUpdateReleaseCallback(currentUBO++);
	while (currentTex < endTex)
		a3textureHandleUpdateReleaseCallback(currentTex++);
	while (currentFBO < endFBO)
		a3framebufferHandleUpdateReleaseCallback(currentFBO++);
	while (currentDFBO < endDFBO)
		a3framebufferDoubleHandleUpdateReleaseCallback(currentDFBO++);

	// re-link specific object pointers for different asset types
	currentBuff = demoState->vbo_staticSceneObjectDrawBuffer;

	currentVAO = demoState->vao_position;
	currentVAO->vertexBuffer = currentBuff;
	a3_refreshDrawable_internal(demoState->draw_grid, currentVAO, currentBuff);
	a3_refreshDrawable_internal(demoState->draw_pointlight, currentVAO, currentBuff);

	currentVAO = demoState->vao_position_color;
	currentVAO->vertexBuffer = currentBuff;
	a3_refreshDrawable_internal(demoState->draw_axes, currentVAO, currentBuff);

	currentVAO = demoState->vao_position_texcoord;
	currentVAO->vertexBuffer = currentBuff;
	a3_refreshDrawable_internal(demoState->draw_skybox, currentVAO, currentBuff);
	a3_refreshDrawable_internal(demoState->draw_unitquad, currentVAO, currentBuff);

	currentVAO = demoState->vao_tangentBasis;
	currentVAO->vertexBuffer = currentBuff;
	a3_refreshDrawable_internal(demoState->draw_plane, currentVAO, currentBuff);
	a3_refreshDrawable_internal(demoState->draw_sphere, currentVAO, currentBuff);
	a3_refreshDrawable_internal(demoState->draw_cylinder, currentVAO, currentBuff);
	a3_refreshDrawable_internal(demoState->draw_torus, currentVAO, currentBuff);
	a3_refreshDrawable_internal(demoState->draw_teapot, currentVAO, currentBuff);

	currentVAO = demoState->vao_tangentBasis_morph5;
	currentVAO->vertexBuffer = currentBuff;
	a3_refreshDrawable_internal(demoState->draw_teapot_morph, currentVAO, currentBuff);

	a3demo_initDummyDrawable_internal(demoState);

	demoState->hierarchyState_skel->poseGroup = demoState->hierarchyPoseGroup_skel;
	demoState->hierarchyPoseGroup_skel->hierarchy = demoState->hierarchy_skel;
}


//-----------------------------------------------------------------------------
