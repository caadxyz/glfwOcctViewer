#include <GL/glew.h>
#include <GLFW/glfw3.h>

#if defined (__APPLE__)
	#define GLFW_EXPOSE_NATIVE_COCOA
	#define GLFW_EXPOSE_NATIVE_NSGL
#elif defined (_WIN32)
	#include <windows.h>
	#define GLFW_EXPOSE_NATIVE_WIN32
	#define GLFW_EXPOSE_NATIVE_WGL
#endif
#include "GLFW/glfw3native.h"
#include "GlfwOccView.h"
#include "occwindow.h"
#include <iostream>

#include <Aspect_Handle.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <OpenGl_GraphicDriver.hxx>


static void error_callback(int error, const char* description)
{
	fprintf_s(stderr, "Error %d: %s\n", error, description);
}


GlfwOccView::GlfwOccView(){}

GlfwOccView::~GlfwOccView() {}

void GlfwOccView::run()
{
	initWindow();
	mainloop();
	cleanup();
}

void GlfwOccView::initWindow()
{
	glfwSetErrorCallback(error_callback);
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(800, 600, "occt", NULL, NULL);
	glfwSetWindowUserPointer(window, this);

	// window callback
	glfwSetWindowSizeCallback(window, GlfwOccView::s_onWindowResized);
	// mouse callback
	glfwSetMouseButtonCallback(window, GlfwOccView::s_onMouseButton);

	glfwMakeContextCurrent(window);
	glewInit();

    //imgui init
	
    initOCC();
}


bool GlfwOccView::initOCC()
{
	if (!m_isInitialized) {
		Handle_Graphic3d_GraphicDriver gpxDriver;
		Handle(OpenGl_GraphicDriver) myGraphicDriver;
		Handle_Aspect_DisplayConnection dispConnection;
		if (dispConnection.IsNull())
			dispConnection = new Aspect_DisplayConnection;
		myGraphicDriver = new OpenGl_GraphicDriver(dispConnection,false);
		myGraphicDriver->SetBuffersNoSwap(true);
		// Create the named OCC 3d viewer
		m_viewer = new V3d_Viewer(myGraphicDriver);
		// Configure the OCC 3d viewer
		m_viewer->SetDefaultViewSize(800.);
		m_viewer->SetDefaultLights();
		m_viewer->SetLightOn();
		m_viewer->ActivateGrid(Aspect_GT_Rectangular, Aspect_GDM_Lines);

		m_GLcontext = new OpenGl_Context();
#if defined (__APPLE__)
		if (m_GLcontext->Init((NSOpenGLContext*)glfwGetNSGLContext(window)))
			fprintf(stderr, "opengl_context init success\n");
		m_view = m_viewer->CreateView();
		Handle(OCCWindow) hWnd = new OCCWindow(window);
			fprintf(stderr, "cocoa_window init success\n");
		m_view->SetWindow(hWnd, (NSOpenGLContext*)glfwGetNSGLContext(window));
#elif defined (_WIN32)
	    if(m_GLcontext->Init(glfwGetWin32Window(window), GetDC(glfwGetWin32Window(window)), glfwGetWGLContext(window)))
			fprintf(stderr, "opengl_context init success\n");
		m_view = m_viewer->CreateView();
		Handle(OCCWindow) hWnd = new OCCWindow(window);
			fprintf(stderr, "wnt_window init success %d %d %d %d\n", 
				hWnd->myXLeft,hWnd->myXRight, hWnd->myYTop,hWnd->myYBottom);
		m_view->SetWindow(hWnd,glfwGetWGLContext(window));
#endif

		if (!hWnd->IsMapped())
			hWnd->Map();
		m_view->MustBeResized();
		m_isInitialized = true;
		m_needsResize = true;
		mContext = new AIS_InteractiveContext(m_viewer);
		mContext->SetDisplayMode(AIS_Shaded, true);
		Handle(AIS_Shape) box = new AIS_Shape(BRepPrimAPI_MakeBox(50,50,50).Shape());
		m_view->FitAll();
		mContext->Display(box, Standard_False);

	}
	return true;
}


void GlfwOccView::cleanup()
{
    // todo cleanup imgui

	//cleanup occt
	m_view->Remove();
	// cleanup glfw
	glfwDestroyWindow(window);
	glfwTerminate();
}


void GlfwOccView::s_onWindowResized(GLFWwindow* window, int width, int height)
{
	if (width == 0 || height == 0) return;
	GlfwOccView* app = reinterpret_cast<GlfwOccView*>(glfwGetWindowUserPointer(window));
	app->OnWindowResized(width,height);
}

void GlfwOccView::s_onMouseButton(GLFWwindow* window, int button, int action, int mods)
{
	GlfwOccView* app = reinterpret_cast<GlfwOccView*>(glfwGetWindowUserPointer(window));
	app->OnMouseButton(button, action, mods);
}
void GlfwOccView::rotateXY(double degrees)
{
	if (m_view.IsNull())
		return;
	int display_w, display_h;
	glfwGetFramebufferSize(window, &display_w, &display_h);
	int x = display_w / 2, y = display_h / 2;
	double X, Y, Z;
	m_view->Convert(x, y, X, Y, Z);
	m_view->Rotate(0, 0, degrees * M_PI / 180., X, Y, Z);
}

void GlfwOccView::OnMouseButton(int button, int action, int mods)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	ButtonMouseX = (int)xpos;
	ButtonMouseY = (int)ypos;
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (action == GLFW_PRESS)
		{
			rotateXY(2.5);
		}
	}
}


void GlfwOccView::mainloop()
{
	while (!glfwWindowShouldClose(window)) {
		drawFrame();
		glfwPollEvents();
	}
	
}

void GlfwOccView::OnWindowResized(int width, int height)
{
	if (!m_view.IsNull())
	{
		m_view->Window()->DoResize();
	}
	m_needsResize = true;
}



void GlfwOccView::redraw()
{
	if (!m_view.IsNull())
	{
		m_GLcontext->MakeCurrent();
		if (m_needsResize)
			m_view->MustBeResized();
		else
		{
			m_view->Invalidate();
		}
	}
	m_needsResize = false;
}


void GlfwOccView::drawFrame()
{
	if (!m_view.IsNull() )
	{
		m_GLcontext->MakeCurrent();

		if (m_needsResize)
			m_view->MustBeResized();
		else
		{
			m_view->Invalidate();
		}

		//todo imgui rendering

		m_GLcontext->SwapBuffers();
	}
}
