#ifndef GLFWOCCVIEW_H
#define GLFWOCCVIEW_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "V3d_View.hxx"
#include "OpenGl_Context.hxx"
#include "AIS_InteractiveContext.hxx"
#include <AIS_Shape.hxx>

class GlfwOccView 
{
public:
	GlfwOccView();
	~GlfwOccView();

	enum CurAction3d 
	{
		CurAction3d_Nothing,
		CurAction3d_DynamicZooming,
		CurAction3d_DynamicPanning,
		CurAction3d_DynamicRoation
	};

	void run();
	void initWindow();
	bool initOCC();
	void cleanup();
	void mainloop();
	void drawFrame();
	void redraw();
	void OnWindowResized(int width, int height);
	void OnFBResized(int width, int height);
	void OnMouseScroll(double xoffset, double yoffset);
	void OnMouseButton(int button, int action, int mods);
	void OnMouseMove(double xpos, double ypos);
	static void s_onWindowResized(GLFWwindow* window, int width, int height);
	static void s_onFBResized(GLFWwindow* window, int width, int height);
	static void s_onMouseScroll(GLFWwindow* window, double xoffset, double yoffset);
	static void s_onMouseButton(GLFWwindow* window, int button, int action, int mods);
	static void s_onMouseMove(GLFWwindow* window, double xpos, double ypos);

	GLFWwindow* window;
	Handle_V3d_Viewer m_viewer;
	Handle_V3d_View m_view;
	Handle_OpenGl_Context m_GLcontext;
	Handle_AIS_InteractiveContext mContext;

	bool m_isInitialized = false;
	bool m_needsResize = false;

	CurAction3d mCurAction3d = CurAction3d_Nothing;
	Standard_Integer mXmin = 0;
	Standard_Integer mYmin = 0;
	Standard_Integer mXmax = 0;
	Standard_Integer mYmax = 0;

};

#endif // !GLFWOCCVIEW_H
