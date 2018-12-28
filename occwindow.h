#ifndef OCCWINDOW_H
#define OCCWINDOW_H
#include <GLFW/glfw3.h>
#include <Aspect_Window.hxx>

class OCCWindow;

class OCCWindow : public Aspect_Window
{
public:

  OCCWindow(GLFWwindow* window );
  void Destroy();
  ~OCCWindow()
  {
    Destroy();
  }
  //! Returns native Window handle
  Aspect_Drawable NativeHandle() const;
  //! Returns parent of native Window handle.
  Aspect_Drawable NativeParentHandle() const;
  //! Applies the resizing to the window <me>
  Aspect_TypeOfResize DoResize() const;
  //! Returns True if the window <me> is opened
  //! and False if the window is closed.
  Standard_Boolean IsMapped() const;
  //! Apply the mapping change to the window <me>
  //! and returns TRUE if the window is mapped at screen.
  Standard_Boolean DoMapping() const { return Standard_True; }
  //! Opens the window <me>.
  void Map() const;
  //! Closes the window <me>.
  void Unmap() const;
  void Position( Standard_Integer& theX1, Standard_Integer& theY1,
                         Standard_Integer& theX2, Standard_Integer& theY2 ) const;
  //! Returns The Window RATIO equal to the physical
  //! WIDTH/HEIGHT dimensions.
  Standard_Real Ratio() const;
  void Size( Standard_Integer& theWidth, Standard_Integer& theHeight ) const;
  Aspect_FBConfig NativeFBConfig() const Standard_OVERRIDE { return NULL; }
  DEFINE_STANDARD_RTTIEXT(OCCWindow,Aspect_Window)

//protected:
  Standard_Integer myXLeft;
  Standard_Integer myYTop;
  Standard_Integer myXRight;
  Standard_Integer myYBottom;
  GLFWwindow* mWindow;
};

#endif // OCCWINDOW_H
