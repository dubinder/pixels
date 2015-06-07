
#include <array>
#include <random>
#include <algorithm>
#include <gl/glut.h>

#include <io.h>
#include <fcntl.h>
#include <iostream>

#include <Windows.h>
#include <windowsx.h>
#include <d3d11.h>



const int g_width = 640;
const int g_height = 480;
int g_windowID = 0;
std::string g_fileName;
void help()
{
  std::cout << "Help\n"
    << "-f (file name)\n"
    << "      The name of the file to be read.\n";
}

void draw(std::string fileName)
{
  int fd = _open(fileName.c_str(), _O_BINARY | _O_RDONLY);
  if (fd == -1)
    std::cout << "failed to open file data 2.\n";
  char r(0);
  char g(0);
  char b(0);
  glBegin(GL_POINTS);
  for (int x = 0; x < g_width; ++x)
  { 
    for (int y = 0; y < g_height; ++y)
    {
      _read(fd, &r, sizeof(r));
      _read(fd, &g, sizeof(g));
      _read(fd, &b, sizeof(b));
      glColor3ub(r, g, b);
      glVertex2i(x, y);
    }
  }
  glEnd();
  _close(fd);
}
static void init()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//Set Background color to black and opaque
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, g_width, 0, g_height);
}

static void reshape(int width, int height){}

static void keyboard(unsigned char key, int x, int y){}

static void keyboardSpecial(int key, int x, int y){}

static void timer(int n)
{
  glutSetWindow(g_windowID);
  glutPostRedisplay();
  glutTimerFunc(1000 / 60, timer, n);
}

void display()
{
  glClear(GL_COLOR_BUFFER_BIT);
  draw(g_fileName);
  glutSwapBuffers();
}

int main(int argc, char** argv)
{

  if (argc < 2)
  {
    help();
    exit(1);
  }
  for (int i = 0; i < argc; ++i)
  {
    if (!strcmp("-f", argv[i]) || !strcmp("-F", argv[i]))
    {
      g_fileName = argv[++i];
    }
  }
  //Free console
  //directx

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowPosition(50, 50);
  glutInitWindowSize(g_width, g_height);
  g_windowID = glutCreateWindow("Pixels");
  init();

  glutDisplayFunc(display);
  //glutIdleFunc(display);
  glutTimerFunc(100, timer, 0);
  //glutReshapeFunc(reshape);
  //glutMouseFunc(mouseFcn);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(keyboardSpecial);

  glutMainLoop();
  return 0;

}//end main
