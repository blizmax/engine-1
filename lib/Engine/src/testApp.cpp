#include "../includes/testApp.h" 
#include "../includes/freeCamera.h"

//#include "../assimp_3_0/include/Importer.hpp"
//#include "../assimp_3_0/include/scene.h"
//#include "../assimp_3_0/include/mesh.h"

#include <vector>
#include <algorithm>
#include <cmath>

#define D3DDEVICE mWindow->getRenderDevice()->getD3D9Device()

// REMAINDER: may use constant shader table

const float kCameraMovementSpeed=0.4f;
const float kCameraRotationSpeed=0.01f;
const float lightRotationSpeed=0.1f;

CustomVertex3NormalUV* spider = 0;

TestApp::TestApp(Window* window) : mWindow(window), 
													 mBuffer(NULL),
													 mTimeSinceElapsedTimeReset(0.0f), 
													 mSceneCamera(new FreeCamera(D3DXVECTOR3(0.0f,0.0f,-10.0f))),
													 mWireframeMode(false)
{
}

TestApp::~TestApp() {

}

CustomVertex3NormalUV verticesCube[] = {
	// -Z face
	{ D3DXVECTOR3(-1.0f,  1.0f, -1.0f),       D3DXVECTOR3(0.0f, 0.0f, -1.0f), 0.0f, 0.0f },
	{ D3DXVECTOR3(1.0f,  1.0f, -1.0f),       D3DXVECTOR3(0.0f, 0.0f, -1.0f), 1.0f, 0.0f },
	{ D3DXVECTOR3(1.0f, -1.0f, -1.0f),       D3DXVECTOR3(0.0f, 0.0f, -1.0f), 1.0f, 1.0f },
	{ D3DXVECTOR3(1.0f, -1.0f, -1.0f),      D3DXVECTOR3(0.0f, 0.0f, -1.0f),  1.0f, 1.0f },
	{ D3DXVECTOR3(-1.0f, -1.0f, -1.0f),       D3DXVECTOR3(0.0f, 0.0f, -1.0f), 0.0f, 1.0f },
	{ D3DXVECTOR3(-1.0f,  1.0f, -1.0f),       D3DXVECTOR3(0.0f, 0.0f, -1.0f), 0.0f, 0.0f },

	// +Z face
	{ D3DXVECTOR3(1.0f,  1.0f,  1.0f),     D3DXVECTOR3(0.0f, 0.0f, 1.0f),  0.0f, 0.0f },
	{ D3DXVECTOR3(-1.0f,  1.0f,  1.0f),     D3DXVECTOR3(0.0f, 0.0f, 1.0f), 1.0f, 0.0f },
	{ D3DXVECTOR3(-1.0f, -1.0f,  1.0f),     D3DXVECTOR3(0.0f, 0.0f, 1.0f),  1.0f, 1.0f },
	{ D3DXVECTOR3(-1.0f, -1.0f,  1.0f),      D3DXVECTOR3(0.0f, 0.0f, 1.0f), 1.0f, 1.0f },
	{ D3DXVECTOR3(1.0f, -1.0f,  1.0f),     D3DXVECTOR3(0.0f, 0.0f, 1.0f), 0.0f, 1.0f },
	{ D3DXVECTOR3(1.0f,  1.0f,  1.0f),    D3DXVECTOR3(0.0f, 0.0f, 1.0f),  0.0f, 0.0f },

	// -Y face
	{ D3DXVECTOR3(-1.0f, -1.0f, -1.0f),     D3DXVECTOR3(0.0f, -1.0f, 0.0f), 0.0f, 0.0f },
	{ D3DXVECTOR3(1.0f, -1.0f, -1.0f),    D3DXVECTOR3(0.0f, -1.0f, 0.0f),  1.0f, 0.0f },
	{ D3DXVECTOR3(1.0f, -1.0f,  1.0f),      D3DXVECTOR3(0.0f, -1.0f, 0.0f), 1.0f, 1.0f },
	{ D3DXVECTOR3(1.0f, -1.0f,  1.0f),    D3DXVECTOR3(0.0f, -1.0f, 0.0f),  1.0f, 1.0f },
	{ D3DXVECTOR3(-1.0f, -1.0f,  1.0f),     D3DXVECTOR3(0.0f, -1.0f, 0.0f), 0.0f, 1.0f },
	{ D3DXVECTOR3(-1.0f, -1.0f, -1.0f),    D3DXVECTOR3(0.0f, -1.0f, 0.0f),  0.0f, 0.0f },

	// +Y face
	{ D3DXVECTOR3(-1.0f,  1.0f,  1.0f),      D3DXVECTOR3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f },
	{ D3DXVECTOR3(1.0f,  1.0f,  1.0f),     D3DXVECTOR3(0.0f, 1.0f, 0.0f), 1.0f, 0.0f },
	{ D3DXVECTOR3(1.0f,  1.0f, -1.0f),      D3DXVECTOR3(0.0f, 1.0f, 0.0f), 1.0f, 1.0f },
	{ D3DXVECTOR3(1.0f,  1.0f, -1.0f),      D3DXVECTOR3(0.0f, 1.0f, 0.0f), 1.0f, 1.0f },
	{ D3DXVECTOR3(-1.0f,  1.0f, -1.0f),     D3DXVECTOR3(0.0f, 1.0f, 0.0f), 0.0f, 1.0f },
	{ D3DXVECTOR3(-1.0f,  1.0f,  1.0f),      D3DXVECTOR3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f },

	// -X face
	{ D3DXVECTOR3(-1.0f,  1.0f,  1.0f),      D3DXVECTOR3(-1.0f, 0.0f, 0.0f), 0.0f, 0.0f },
	{ D3DXVECTOR3(-1.0f,  1.0f, -1.0f),      D3DXVECTOR3(-1.0f, 0.0f, 0.0f), 1.0f, 0.0f },
	{ D3DXVECTOR3(-1.0f, -1.0f, -1.0f),      D3DXVECTOR3(-1.0f, 0.0f, 0.0f), 1.0f, 1.0f },
	{ D3DXVECTOR3(-1.0f, -1.0f, -1.0f),      D3DXVECTOR3(-1.0f, 0.0f, 0.0f), 1.0f, 1.0f },
	{ D3DXVECTOR3(-1.0f, -1.0f,  1.0f),     D3DXVECTOR3(-1.0f, 0.0f, 0.0f),  0.0f, 1.0f },
	{ D3DXVECTOR3(-1.0f,  1.0f,  1.0f),      D3DXVECTOR3(-1.0f, 0.0f, 0.0f), 0.0f, 0.0f },

	// +X face
	{ D3DXVECTOR3(1.0f,  1.0f, -1.0f),      D3DXVECTOR3(1.0f, 0.0f, 0.0f), 0.0f, 0.0f },
	{ D3DXVECTOR3(1.0f,  1.0f,  1.0f),      D3DXVECTOR3(1.0f, 0.0f, 0.0f), 1.0f, 0.0f },
	{ D3DXVECTOR3(1.0f, -1.0f,  1.0f),      D3DXVECTOR3(1.0f, 0.0f, 0.0f), 1.0f, 1.0f },
	{ D3DXVECTOR3(1.0f, -1.0f,  1.0f),      D3DXVECTOR3(1.0f, 0.0f, 0.0f), 1.0f, 1.0f },
	{ D3DXVECTOR3(1.0f, -1.0f, -1.0f),     D3DXVECTOR3(1.0f, 0.0f, 0.0f),  0.0f, 1.0f },
	{ D3DXVECTOR3(1.0f,  1.0f, -1.0f),      D3DXVECTOR3(1.0f, 0.0f, 0.0f), 0.0f, 0.0f }
};

void TestApp::onCreateDevice() {

	//Assimp::Importer importer;
	//
	//const aiScene* scene = importer.ReadFile("model/spider/spider.obj", 0);
	//
	//if(scene == NULL) {
	//	OutputDebugStringA(importer.GetErrorString());
	//	OutputDebugStringA("\n");
	//}

	//aiMesh** mesh = scene->mMeshes;
	//
	//int numVerticesSpider = (*mesh)->mNumVertices;
	//spider = new CustomVertex3NormalUV[numVerticesSpider];

	//for(int i = 0; i < numVerticesSpider; ++i) {
	//	// still have to to check out how this actually works
	//}


	

	// set vertex delaration	
	mWindow->getRenderDevice()->setVertexDeclaration(CustomVertex3NormalUV::decl);

	// load shader
	mWindow->getRenderDevice()->loadEffectFromFile("./shader/basic.fx");

	// load textures
	//HR(D3DXCreateTextureFromFileA(D3DDEVICE, "./texture/white.jpg", &mWhiteTexture));

	// init light
	initLight();

	// create and init cube
	mBuffer = mWindow->getRenderDevice()->createVertexBuffer(36, CUSTOMVERTEX3NORMALUV, std::string("cube"));
	mWindow->getRenderDevice()->setVertexBufferData(std::string("cube"), verticesCube);

	//mSpider = mWindow->getRenderDevice()->createVertexBuffer(numVerticesSpider, CUSTOMVERTEX3NORMALUV, std::string("spider"));
	//mWindow->getRenderDevice()->setVertexBufferData(std::string("spider"), spider);

	// init shader handles
	initShaderHandles();
	
	// setup scene camera
	mSceneCamera->setPosition(D3DXVECTOR3(0.0f,0.0f,-20.0f));
}

void TestApp::onResetDevice() {
	// set vertex delaration	
	
}

void TestApp::onReleaseDevice() {

}

void TestApp::onUpdate() {
	if(mWindow) {
		setShaderData();
	}
}

void TestApp::onRender() {
	if(mWindow) {
		mT.start();
		
		mWindow->getRenderDevice()->getCurrentEffect()->Begin(NULL,NULL);
		mWindow->getRenderDevice()->getCurrentEffect()->BeginPass(0);
		
		mWindow->getRenderDevice()->renderVertexbuffer(D3DPT_TRIANGLELIST, std::string("cube"));

		mWindow->getRenderDevice()->getCurrentEffect()->EndPass();
		mWindow->getRenderDevice()->getCurrentEffect()->End();

		mT.stop();
		float elapsedTime = mT.getElapsedTimeInMilliSec();
		mTimeSinceElapsedTimeReset = mTimeSinceElapsedTimeReset + elapsedTime;
	}
}

void TestApp::onLostDevice() {

}

void TestApp::setWindow(Window* window) {
	mWindow = window;
}

void TestApp::onKeyPressed(WPARAM keyCode) {
	

	switch(keyCode) {	
	case 'W':
		mSceneCamera->moveForward(kCameraMovementSpeed);
		break;
	case 'S':
		mSceneCamera->moveForward(-kCameraMovementSpeed);
		break;
	case 'A':
		mSceneCamera->moveRight(-kCameraMovementSpeed);
		break;
	case 'D':
		mSceneCamera->moveRight(kCameraMovementSpeed);
		break;
	case 'Q':
		mWireframeMode ? mWireframeMode = false : mWireframeMode = true;
		break;
	case VK_UP:
		changeDeviceInfo();
		break;
	case VK_DOWN:
		
		break;
	case VK_LEFT:
		
		break;
	case VK_RIGHT:
		
		break;

	default: break;

	}
}

void TestApp::onRawMouseInputReceived(RAWINPUT const& rawMouseInput) {
	static BOOL mouseDown;

	if(rawMouseInput.data.mouse.usButtonFlags == RI_MOUSE_LEFT_BUTTON_DOWN)
		mouseDown = true;
	if(rawMouseInput.data.mouse.usButtonFlags == RI_MOUSE_LEFT_BUTTON_UP)
		mouseDown = false;

	if(mouseDown) {
		float lastX = rawMouseInput.data.mouse.lLastX;
		float lastY = rawMouseInput.data.mouse.lLastY;

		mSceneCamera->pitch(kCameraRotationSpeed*lastY);
		mSceneCamera->yaw(kCameraRotationSpeed*lastX);
	}
}

void TestApp::initLight() {
	// only do this now because no of no init list
	//memset(&mDirLight,'\0', sizeof(DirectionalLight));
	mDirLight.ambient = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.0f);
	mDirLight.diffuse = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.0f);
	mDirLight.spec = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
	mDirLight.posW = D3DXVECTOR3(2.0f, 2.0f, 2.0f); 

	// only do this now because no of no init list
	//memset(&mMaterial,'\0', sizeof(Material));
	mMaterial.diffuse = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
	mMaterial.ambient = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
	mMaterial.spec = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f);
	mMaterial.specPower = 150.0f;

	mAmbient.r = 0.2f;
	mAmbient.g = 0.2f;
	mAmbient.b = 0.2f;
	mAmbient.a = 1.0f;
}


void TestApp::onCustomUserFunction() {
	
}


void TestApp::initShaderHandles() {
	// get handle for rendering scene
	mTechniqueHandle = mWindow->getRenderDevice()->getCurrentEffect()->GetTechniqueByName("NormalMappingDirectionalLighting");
	ASSERT(mTechniqueHandle != 0, "NormalMappingDirectionalLighting technique returns NULL pointer");

	mTechniqueWireFrameHandle = mWindow->getRenderDevice()->getCurrentEffect()->GetTechniqueByName("NormalMappingDirectionalLightingWireframe");
	ASSERT(mTechniqueWireFrameHandle != 0, "NormalMappingDirectionalLightingWireframe technique returns NULL pointer");

	// initializing shader handles for scene transform matrices
	mWorldHandle = mWindow->getRenderDevice()->getCurrentEffect()->GetParameterByName(0, "gWorldMatrix");
	ASSERT(mWorldHandle != 0, "worldMatrix == NULL");
	mInverseTransposeHandle = mWindow->getRenderDevice()->getCurrentEffect()->GetParameterByName(0, "gWorldInverseTransposeMatrix");
	ASSERT(mInverseTransposeHandle != 0, "mInverseTransposeHandle == NULL");

	mViewProjectionHandle = mWindow->getRenderDevice()->getCurrentEffect()->GetParameterByName(0, "gViewProjectionMatrix");
	ASSERT(mViewProjectionHandle != 0, "projMatrix == NULL");

	// initializing shader handle to camera pos
	mCameraPosHandle = mWindow->getRenderDevice()->getCurrentEffect()->GetParameterByName(0, "gCameraPos");
	ASSERT(mCameraPosHandle != 0, "mCameraPosHandle == NULL");

	// initializing shader handles for global ambient
	mAmbientHandle = mWindow->getRenderDevice()->getCurrentEffect()->GetParameterByName(0, "gGlobalAmbient");
	ASSERT(mAmbientHandle != 0, "mAmbientHandle == NULL");
	
	// light
	mLightHandle = mWindow->getRenderDevice()->getCurrentEffect()->GetParameterByName(0, "gLight");
	ASSERT(mLightHandle != 0, "mLightHandle == NULL");
	
	// material
	mMaterialHandle = mWindow->getRenderDevice()->getCurrentEffect()->GetParameterByName(0, "gMaterial");
	ASSERT(mMaterialHandle != 0, "mMaterialHandle == NULL");
	
	
	// texture
	//mTextureHandle = mWindow->getRenderDevice()->getCurrentEffect()->GetParameterByName(0, "gColorMapTexture");
	//ASSERT(mTextureHandle != 0, "mTextureHandle == NULL");

}

void TestApp::setShaderData() {
	static D3DXMATRIX world, view, proj;
	static D3DXMATRIX worldViewProjectionMatrix;
	static D3DXMATRIX worldInverseTransposeMatrix;

	mSceneCamera->calculateViewMatrix(view);
	mSceneCamera->calculateProjectionMatrix(proj);

	// Calculate world matrix to transform the cube.
	D3DXMatrixIdentity(&world);

	// Calculate combined world-view-projection matrix.
	worldViewProjectionMatrix = view * proj;

	// Calculate the transpose of the inverse of the world matrix.
	D3DXMatrixInverse(&worldInverseTransposeMatrix, 0, &world);
	D3DXMatrixTranspose(&worldInverseTransposeMatrix, &worldInverseTransposeMatrix);

	// set technique
	if(!mWireframeMode)
		mWindow->getRenderDevice()->getCurrentEffect()->SetTechnique(mTechniqueHandle);
	else
		mWindow->getRenderDevice()->getCurrentEffect()->SetTechnique(mTechniqueWireFrameHandle);

	// Set the matrices for the shader.
	mWindow->getRenderDevice()->getCurrentEffect()->SetMatrix(mWorldHandle, &world);

	mWindow->getRenderDevice()->getCurrentEffect()->SetMatrix(mInverseTransposeHandle, &worldInverseTransposeMatrix);
	mWindow->getRenderDevice()->getCurrentEffect()->SetMatrix(mViewProjectionHandle, &worldViewProjectionMatrix);

	// Set the camera position.
	mWindow->getRenderDevice()->getCurrentEffect()->SetValue(mCameraPosHandle, &mSceneCamera->getPosition(), sizeof(D3DXVECTOR3));

	// Set the scene global ambient term.
	mWindow->getRenderDevice()->getCurrentEffect()->SetValue(mAmbientHandle, &mAmbient, sizeof(D3DXCOLOR));

	// Set the lighting parameters for the shader.
	mWindow->getRenderDevice()->getCurrentEffect()->SetValue(mLightHandle, &mDirLight, sizeof(PointLight));

	// Set the material parameters for the shader.
	mWindow->getRenderDevice()->getCurrentEffect()->SetValue(mMaterialHandle, &mMaterial, sizeof(Material));

	// Bind the textures to the shader.
	//mWindow->getRenderDevice()->getCurrentEffect()->SetTexture(mTextureHandle, mWhiteTexture);
}

void TestApp::changeDeviceInfo() {
	//D3DPRESENT_PARAMETERS newDevInfo;
	D3DPRESENT_PARAMETERS* currentDevInfo = mWindow->getRenderDevice()->getDeviceInfo();

	//memcpy(&newDevInfo, currentDevInfo, sizeof(D3DPRESENT_PARAMETERS));
	//newDevInfo.Flags = 0;
	VertexDeclarations::releaseVertexDeclarations();
	mWindow->getRenderDevice()->getCurrentEffect()->Release();
	mWindow->getRenderDevice()->resetDevice(currentDevInfo);
	VertexDeclarations::initVertexDeclarations(D3DDEVICE);
	mWindow->getRenderDevice()->setVertexDeclaration(CustomVertex3NormalUV::decl);
	mWindow->getRenderDevice()->loadEffectFromFile("./shader/basic.fx");
	initShaderHandles();

}