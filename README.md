# glfw-occt-demo
a simple demo for opencascade and glfw

classplatform: win10, apple


ubuntu: 


```
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX

m_GLcontext = new OpenGl_Context();
m_GLcontext->Init(glfwGetX11Window(window), glfwGetX11Display(), glfwGetGLXContext(window));
```
