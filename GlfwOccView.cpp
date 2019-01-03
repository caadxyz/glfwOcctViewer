#include <GL/glew.h>
#include <GLFW/glfw3.h>
#if defined (__APPLE__)
	#define GLFW_EXPOSE_NATIVE_COCOA
	#define GLFW_EXPOSE_NATIVE_NSGL
#elif defined (_WIN32)
	#include <windows.h>
	#define GLFW_EXPOSE_NATIVE_WIN32
	#define GLFW_EXPOSE_NATIVE_WGL
#else    
	#define GLFW_EXPOSE_NATIVE_X11
    #define GLFW_EXPOSE_NATIVE_GLX
#endif
#include "GLFW/glfw3native.h"
#include "GlfwOccView.h"
#include "OccWindow.h"
#include <iostream>

#include <Aspect_Handle.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <TopAbs_ShapeEnum.hxx>

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error %d: %s\n", error, description);
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
	window = glfwCreateWindow(800, 600, "glfw occt", NULL, NULL);
	glfwSetWindowUserPointer(window, this);
	// window callback
	glfwSetWindowSizeCallback(window, GlfwOccView::s_onWindowResized);
	glfwSetFramebufferSizeCallback(window, GlfwOccView::s_onFBResized);
	// mouse callback
	glfwSetScrollCallback(window, GlfwOccView::s_onMouseScroll);
	glfwSetMouseButtonCallback(window, GlfwOccView::s_onMouseButton);
	glfwSetCursorPosCallback(window, GlfwOccView::s_onMouseMove);

	glfwMakeContextCurrent(window);
	glewInit();
    /*** todo: imgui init ***/
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
		m_viewer->SetDefaultLights();
		m_viewer->SetLightOn();
	    m_viewer->SetDefaultTypeOfView(V3d_PERSPECTIVE);
		m_viewer->ActivateGrid(Aspect_GT_Rectangular, Aspect_GDM_Lines);

		m_GLcontext = new OpenGl_Context();
#if defined (__APPLE__)
		if (m_GLcontext->Init((NSOpenGLContext*)glfwGetNSGLContext(window)))
			fprintf(stderr, "opengl_context init success\n");
		m_view = m_viewer->CreateView();
		Handle(OccWindow) hWnd = new OccWindow(window);
			fprintf(stderr, "cocoa_window init success\n");
		m_view->SetWindow(hWnd, (NSOpenGLContext*)glfwGetNSGLContext(window));
#elif defined (_WIN32)
	    if(m_GLcontext->Init(glfwGetWin32Window(window), GetDC(glfwGetWin32Window(window)), glfwGetWGLContext(window)))
			fprintf(stderr, "opengl_context init success\n");
		m_view = m_viewer->CreateView();
		Handle(OccWindow) hWnd = new OccWindow(window);
			fprintf(stderr, "wnt_window init success %d %d %d %d\n", 
				hWnd->myXLeft,hWnd->myXRight, hWnd->myYTop,hWnd->myYBottom);
		m_view->SetWindow(hWnd,glfwGetWGLContext(window));
#else
		if (m_GLcontext->Init(glfwGetX11Window(window), glfwGetX11Display(), glfwGetGLXContext(window)))
			fprintf(stderr, "opengl_context init success\n");
		m_view = m_viewer->CreateView();
		Handle(OccWindow) hWnd = new OccWindow(window);
		fprintf(stderr, "x11_window init success %d %d %d %d\n",
			hWnd->myXLeft, hWnd->myXRight, hWnd->myYTop, hWnd->myYBottom);
		m_view->SetWindow(hWnd, glfwGetGLXContext(window));
#endif

		if (!hWnd->IsMapped())
			hWnd->Map();
		m_view->MustBeResized();
		m_isInitialized = true;
		m_needsResize = true;
		mContext = new AIS_InteractiveContext(m_viewer);
		mContext->SetDisplayMode(AIS_Shaded, true);

		//opencascade scene
		gp_Ax2 anAxis;
		anAxis.SetLocation(gp_Pnt(0.0,0.0,0.0));
		Handle(AIS_Shape) box = new AIS_Shape(BRepPrimAPI_MakeBox(anAxis,50,50,50).Shape());
		mContext->Display(box, Standard_False);
		anAxis.SetLocation(gp_Pnt(25.0,125.0,0.0));
		Handle(AIS_Shape) cone = new AIS_Shape(BRepPrimAPI_MakeCone(anAxis,25,0,50).Shape());
		mContext->Display(cone, Standard_False);
	}
	return true;
}

void GlfwOccView::mainloop()
{
	while (!glfwWindowShouldClose(window)) {
		drawFrame();
		glfwPollEvents();
	}
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
		/*** todo: imgui rendering ***/
		m_GLcontext->SwapBuffers();
	}
}

void GlfwOccView::cleanup()
{
    /*** todo cleanup imgui ***/
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
void GlfwOccView::s_onFBResized(GLFWwindow* window, int width, int height)
{
	if (width == 0 || height == 0) return;
	GlfwOccView* app = reinterpret_cast<GlfwOccView*>(glfwGetWindowUserPointer(window));
	app->OnFBResized(width, height);
}
void GlfwOccView::s_onMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	GlfwOccView* app = reinterpret_cast<GlfwOccView*>(glfwGetWindowUserPointer(window));
	app->OnMouseScroll(xoffset, yoffset);
}
void GlfwOccView::s_onMouseButton(GLFWwindow* window, int button, int action, int mods)
{
	GlfwOccView* app = reinterpret_cast<GlfwOccView*>(glfwGetWindowUserPointer(window));
	app->OnMouseButton(button, action, mods);
}
void GlfwOccView::s_onMouseMove(GLFWwindow* window, double xpos, double ypos)
{
	GlfwOccView* app = reinterpret_cast<GlfwOccView*>(glfwGetWindowUserPointer(window));
	app->OnMouseMove(xpos, ypos);
}

void GlfwOccView::OnMouseScroll(double xoffset, double yoffset)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	m_view->StartZoomAtPoint((int)xpos,(int)ypos);
	m_view->ZoomAtPoint((int)xpos,(int)ypos,(int)xpos*(1+yoffset*0.05), (int)ypos*(1+yoffset*0.05));
}
void GlfwOccView::OnMouseButton(int button, int action, int mods)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	if (action == GLFW_PRESS)
	{
		mXmin = (int)xpos;
		mXmax = (int)xpos;
		mYmin = (int)ypos;
		mYmax = (int)ypos;
		if (button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			mCurAction3d = CurAction3d_DynamicRoation;
			m_view->StartRotation((int)xpos, (int)ypos);
		}
		if (button == GLFW_MOUSE_BUTTON_MIDDLE)
		{
			mCurAction3d = CurAction3d_DynamicPanning;
		}
	}
	else 
	{
		mCurAction3d = CurAction3d_Nothing;
	}
}

void GlfwOccView::OnMouseMove(double xpos, double ypos)
{
	if (mCurAction3d == CurAction3d_DynamicRoation)
	{
		if (!m_view.IsNull())
		{
			m_view->Rotation((int)xpos, (int)ypos);
		}
	}
	else if (mCurAction3d == CurAction3d_DynamicPanning)
	{
		if (!m_view.IsNull())
		{
			m_view->Pan((int)xpos - mXmax, -((int)ypos - mYmax));
			mXmax = (int)xpos;
			mYmax = (int)ypos;
		}
	}
	else
	{
		if (!m_view.IsNull())
		{
			//hilight selected shapes
			mContext->MoveTo((int)xpos,(int)ypos,m_view,Standard_True);
		}
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
void GlfwOccView::OnFBResized(int width, int height)
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
