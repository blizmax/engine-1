#include "../includes/DirectX9.h"

DirectX9::DirectX9() : m_pDevice(NULL), m_pD3D(NULL), m_runningApp(NULL) {
	
}

DirectX9::DirectX9(App* currentApp) : m_pDevice(NULL), m_pD3D(NULL), m_runningApp(currentApp) {

}

DirectX9::~DirectX9() {
	release();
}

void DirectX9::init(HWND window) {
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	// struct that holds informations provided to the video card
	D3DPRESENT_PARAMETERS dev_info;

	memset(&dev_info,'\0', sizeof(dev_info));
	
	dev_info.BackBufferCount = 1; // 1 back buffer (double buffering)
	dev_info.SwapEffect = D3DSWAPEFFECT_DISCARD; // discard previous frame
	dev_info.MultiSampleType = D3DMULTISAMPLE_NONE; // no multi sampling
	dev_info.MultiSampleQuality = 0; // no multi sampling
	dev_info.hDeviceWindow = window;
	dev_info.Flags = 0;
	dev_info.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	dev_info.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	dev_info.EnableAutoDepthStencil = true; // z-buffer
	dev_info.AutoDepthStencilFormat = D3DFMT_D16;
	dev_info.BackBufferFormat = D3DFMT_X8R8G8B8;
#ifndef FULLSCREEN
	dev_info.Windowed = true;
#else
	dev_info.Windowed = false;
	dev_info.BackBufferWidth = WINDOW_WIDTH;
	dev_info.BackBufferHeight = WINDOW_HEIGHT;
#endif

	HRESULT result;

	result = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
							   D3DDEVTYPE_HAL,
							   window,
							   D3DCREATE_SOFTWARE_VERTEXPROCESSING,
							   &dev_info,
							   &m_pDevice);

	

	if(m_runningApp)
		m_runningApp->onCreateDevice();
}

LPDIRECT3DDEVICE9 DirectX9::getD3D9Device() const {
	return m_pDevice;
}

void DirectX9::release() {

	std::map<std::string,VertexbufferInfo*>::iterator iter;
	iter = m_vertexBuffers.begin();
	while(iter!=m_vertexBuffers.end()) {
		iter->second->buffer->Release();
		++iter;
	}

	m_vertexBuffers.clear();

	m_pDevice->Release();
	m_pD3D->Release();

	m_runningApp->onReleaseDevice();
}


int DirectX9::calcPrimitiveCount(T_PRIMITIVE primitiveType, const DWORD numberOfVertices) {
	switch(primitiveType) {
		case D3DPT_POINTLIST:
			return numberOfVertices;
		case D3DPT_LINELIST:
			return ceil((double)numberOfVertices/2.0);
		case D3DPT_TRIANGLELIST:
			return ceil((double)numberOfVertices/3.0);
		default: return -1;
	}
}

int DirectX9::calcCustomStructSize(DWORD FVF) {
	switch(FVF) {
		case CUSTOMVERTEX3COLORUVFORMAT:
			return sizeof(CustomVertex3ColorUV);
		case CUSTOMVERTEX3COLORFORMAT:
			return sizeof(CustomVertex3Color);
		default: return -1;
	}

}

void DirectX9::setRenderState(T_RENDERSTATE renderState, const DWORD value) {
	m_pDevice->SetRenderState(renderState, value);
}

void DirectX9::setTransform(T_TRANSFORM transformState, const D3DMATRIX* pMatrix) {
	m_pDevice->SetTransform(transformState, pMatrix);
}

void DirectX9::renderFrame() {

	m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 255, 0), 1.0f, 0);

	m_pDevice->BeginScene();
	
	if(m_runningApp) {
		m_runningApp->onUpdate();
		m_runningApp->onRender();
	}

	m_pDevice->EndScene();
	
	m_pDevice->Present(NULL,NULL,NULL,NULL);

}

//// vertex buffer functions
//VertexbufferInfo* createVertexBuffer(const DWORD numberOfVertices, const DWORD FVF, std::string tag);
//// the customVertex FVF has to be exactly as defined in the associated buffer 
//void setVertexBufferData(std::string tag, void* customVertices);
//
//// this function returns the raw video memory pointer given by
//// buffer->Lock(0, 0, (void**)&pVoid, 0); return pVoid;
//// u must not forget to call unlockRawVideoMemoryPtr to tell DirectX9 the memory region locked can be unlocked again
//void* getRawVideoMemoryPtr(std::string vertexBuffer);
//void unlockRawVideoMemoryPtr(std::string vertexBuffer);
//
//// render given vertex buffer 
//// important: it is assumed that you provide the correct information about the size of the customVertexStruct and their amount
//void renderVertexbuffer(T_PRIMITIVE type, std::string tag);
//void renderAllVertexbuffers();

VertexbufferInfo* DirectX9::createVertexBuffer(const DWORD numberOfVertices, const DWORD FVF, const std::string tag) {

	if(FVF != D3DPT_POINTLIST && FVF != D3DPT_LINELIST && FVF != D3DPT_TRIANGLELIST)
		return NULL;

	VertexbufferInfo* info = new VertexbufferInfo;
	info->FVF = FVF;
	info->vertexCount = numberOfVertices;

	int vertexStructSize = calcCustomStructSize(FVF);

	info->FVF = m_pDevice->CreateVertexBuffer(numberOfVertices*vertexStructSize, 0, FVF, D3DPOOL_MANAGED, &info->buffer, NULL);

	m_vertexBuffers[tag] = info;
	return info;
}



