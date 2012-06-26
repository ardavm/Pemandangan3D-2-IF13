#ifndef GLXWINDOW_H_INCLUDED
#define GLXWINDOW_H_INCLUDED

#define GLX_GLXEXT_LEGACY //Must be declared so that our local glxext.h is picked up, rather than the system one
#include "glee/GLee.h"
#include <GL/glx.h>
#include "glx/glxext.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/xf86vmode.h>
#include <X11/keysym.h>
#include <ctime>

class Example; //Declare our Example class

class SimpleGLXWindow
{
public:
    SimpleGLXWindow(); //default constructor
    virtual ~SimpleGLXWindow();

    bool create(int width, int height, int bpp, bool fullscreen);
    void destroy();
    void processEvents();
    void attachExample(Example* example);

    bool isRunning(); //Is the window running?

    void swapBuffers() { glXSwapBuffers(m_display, m_XWindow); }

    float getElapsedSeconds();

private:
    Example* m_example; //A link to the example program
    bool m_isRunning; //Is the window still running?

    Example* getAttachedExample() { return m_example; }

    unsigned int m_lastTime;

    Display* m_display;
    Window m_XWindow;
    GLXContext m_glContext;
    XF86VidModeModeInfo m_XF86DeskMode;
    XSetWindowAttributes m_XSetAttr;
    int m_screenID;

    bool m_isFullscreen;
    unsigned int m_width;
    unsigned int m_height;
    unsigned int m_bpp;

    bool m_GL3Supported;
};

#endif // GLXWINDOW_H_INCLUDED
