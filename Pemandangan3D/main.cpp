#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#define GLX_GLXEXT_LEGACY 

#ifdef _WIN32
#include <windows.h>
#include "glwindow.h"
#else
#include "glxwindow.h"
#endif

#include "projek.h"

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR cmdLine,
                   int cmdShow)
{
#else
int main(int argc, char** argv)
{
#endif
    //Mengatur Tampilan Window
    const int windowWidth = 1024;
    const int windowHeight = 768;
    const int windowBPP = 16;
    const int windowFullscreen = false;

#ifdef _WIN32
    
    GLWindow programWindow(hInstance);
#else
    SimpleGLXWindow programWindow;
#endif

    //Contoh Kode OpenGL
    Example example;

    //Menyertakan Contoh Proyek ke window
    programWindow.attachExample(&example);

    
    if (!programWindow.create(windowWidth, windowHeight, windowBPP, windowFullscreen))
    {
        
#ifdef _WIN32
        MessageBox(NULL, "Unable to create the OpenGL Window", "An error occurred", MB_ICONERROR | MB_OK);
#endif
        programWindow.destroy(); 
        return 1;
    }

    if (!example.init()) 
    {
#ifdef _WIN32
        MessageBox(NULL, "Could not initialize the application", "An error occurred", MB_ICONERROR | MB_OK);
#endif
        programWindow.destroy(); 
        return 1;
    }

    
    while(programWindow.isRunning())
    {
        programWindow.processEvents(); //Proses Semua Aktifitas Window

       
        float elapsedTime = programWindow.getElapsedSeconds();

        example.prepare(elapsedTime); 
        example.render(); 

        programWindow.swapBuffers();
    }

    example.shutdown(); 
    programWindow.destroy(); 

    return 0; 
}
