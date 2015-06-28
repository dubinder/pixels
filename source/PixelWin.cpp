// include the basic windows header file
#include <windows.h>
#include <windowsx.h>
#include <fstream>
#include <d3d11.h>

#pragma comment (lib, "d3d11.lib")

// global declarations
const int g_width = 640;                  // Screen Width
const int g_height = 480;                 // Screen Height
IDXGISwapChain *g_pSwapchain;              // the pointer to the swap chain interface
ID3D11Device *g_pDev;                      // the pointer to our Direct3D device interface
ID3D11DeviceContext *g_pDevcon;            // the pointer to our Direct3D device context
ID3D11RenderTargetView *g_pBackbuffer;     // the pointer to the back buffer
ID3D11PixelShader *g_pPS;                 // the pointer to the pixel shader
ID3D11VertexShader *g_pVS;                // the pointer to the vertex shader
ID3D11Buffer *g_pVBuffer;                 // the pointer to the vertex buffer
ID3D11InputLayout *g_pILayout;            // the pointer to the input Layout

struct Color
{
  float red;
  float green;
  float blue;
  float alpha;
};

struct Vertex
{
  float x;
  float y;
  float z;
  Color vColor;
};

Vertex firstVertex = { 0, 0, 0, firstVertex.vColor = { 1.0f, 0.0, 0.0, 1.0 } };



void InitGraphics()
{
  // create a triangle using the VERTEX struct

  Vertex Triangle[] =
  {
    { 0.0f, 0.5f, 0.0f, { 1.0f, 0.0f, 0.0f, 1.0f } },
    { 0.45f, -0.5, 0.0f, { 0.0f, 1.0f, 0.0f, 1.0f } },
    { -0.45f, -0.5f, 0.0f, { 0.0f, 0.0f, 1.0f, 1.0f } }
  };

  // create the vertex buffer
  D3D11_BUFFER_DESC bd;
  ZeroMemory(&bd, sizeof(bd));

  bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
  bd.ByteWidth = sizeof(Vertex) * 3;             // size is the VERTEX struct * 3
  bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
  bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

  g_pDev->CreateBuffer(&bd, NULL, &g_pVBuffer);       // create the buffer


  // copy the vertices into the buffer
  D3D11_MAPPED_SUBRESOURCE ms;
  g_pDevcon->Map(g_pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
  memcpy(ms.pData, Triangle, sizeof(Triangle));                 // copy the data
  g_pDevcon->Unmap(g_pVBuffer, NULL);                                      // unmap the buffer
}

char * ReadData(const char* path)
{
  std::ifstream shader;
  size_t shaderSize;
  char * shaderData = { 0 };

  shader.open(path, std::ios::in | std::ios::binary);
  if (shader.is_open())
  {
    shader.seekg(0, std::ios::end);
    shaderSize = static_cast<size_t>(shader.tellg());
    shaderData = new char[shaderSize];
    shader.seekg(0, std::ios::beg);
    shader.read(shaderData, shaderSize);
    shader.close();
  }
  return shaderData;
}

size_t ShaderSize(const char* path)
{
  std::ifstream shader;
  size_t shaderSize;

  shader.open(path, std::ios::in | std::ios::binary);
  if (shader.is_open())
  {
    shader.seekg(0, std::ios::end);
    shaderSize = static_cast<size_t>(shader.tellg());
    shader.close();
  }
  return shaderSize;
}
void InitPipeline()
{
  auto shaderByteCode = ReadData("E:/bit.butcket/pixels/pixels/Debug/VertexShader.cso");
  auto vShaderSize = ShaderSize("E:/bit.butcket/pixels/pixels/Debug/VertexShader.cso");

  size_t test = sizeof(shaderByteCode);
  HRESULT result = g_pDev->CreateVertexShader(
    shaderByteCode,
    vShaderSize,
    nullptr,
    &g_pVS
    );
  shaderByteCode = ReadData("E:/bit.butcket/pixels/pixels/Debug/PixelShader.cso");
  auto pShaderSize = ShaderSize("E:/bit.butcket/pixels/pixels/Debug/PixelShader.cso");
  result = g_pDev->CreatePixelShader(
    shaderByteCode,
    pShaderSize,
    nullptr,
    &g_pPS
    );

  g_pDevcon->VSSetShader(g_pVS, 0, 0);
  g_pDevcon->PSSetShader(g_pPS, 0, 0);

  // create the input layout object
  const D3D11_INPUT_ELEMENT_DESC ied[] =
  {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  };

  result = g_pDev->CreateInputLayout(ied, ARRAYSIZE(ied), g_pVS, vShaderSize, &g_pILayout);
  g_pDevcon->IASetInputLayout(g_pILayout);
}
// sets up and initializes Direct3D
void InitD3D(HWND hWnd)
{
  //local swap chain struct
  DXGI_SWAP_CHAIN_DESC scd;

  //Clear the struct
  ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

  //Fill the struct
  scd.BufferCount = 1;                                // One Back Buffer
  scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Use 32-bit Color
  scd.BufferDesc.Width = g_width;                     // set the back buffer width
  scd.BufferDesc.Height = g_height;                   // set the back buffer height
  scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // how swap chain is to be used
  scd.OutputWindow = hWnd;                            // The window to be used
  scd.SampleDesc.Count = 4;                           // How manu multisamples
  scd.Windowed = TRUE;                                // Windowed/full-screen mode
  scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow full-screen switching

  int HRESULT = D3D11CreateDeviceAndSwapChain(NULL,
    D3D_DRIVER_TYPE_HARDWARE,
    NULL,
    NULL,
    NULL,
    NULL,
    D3D11_SDK_VERSION,
    &scd,
    &g_pSwapchain,
    &g_pDev,
    NULL,
    &g_pDevcon);

  // get the address of the back buffer
  ID3D11Texture2D *pBackBuffer;
  g_pSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

  // use the back buffer address to create the render target
  g_pDev->CreateRenderTargetView(pBackBuffer, NULL, &g_pBackbuffer);
  pBackBuffer->Release();

  // set the render target as the back buffer
  g_pDevcon->OMSetRenderTargets(1, &g_pBackbuffer, NULL);

  D3D11_VIEWPORT viewport;
  ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

  viewport.TopLeftX = 0;
  viewport.TopLeftY = 480;
  viewport.Width = g_width;
  viewport.Height = g_height;

  g_pDevcon->RSSetViewports(1, &viewport);

  InitPipeline();
  InitGraphics();
}




// closes Direct3D and releases memory
void CleanD3D()
{
  g_pSwapchain->SetFullscreenState(FALSE, NULL);    // switch to windowed mode
  
  g_pVS->Release();
  g_pPS->Release();
  g_pSwapchain->Release();
  g_pBackbuffer->Release();
  g_pDev->Release();
  g_pDevcon->Release();
}

void RenderFrame(void)
{
  // clear the back buffer to a deep blue
  float color[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
  g_pDevcon->ClearRenderTargetView(g_pBackbuffer, color);

  // do 3D rendering on the back buffer here
  // select which vertex buffer to display
  UINT stride = sizeof(Vertex);
  UINT offset = 0;
  g_pDevcon->IASetVertexBuffers(0, 1, &g_pVBuffer, &stride, &offset);

  // select which primtive type we are using
  g_pDevcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  // draw the vertex buffer to the back buffer
  g_pDevcon->Draw(3, 0);

  // switch the back buffer and the front buffer
  g_pSwapchain->Present(0, 0);
}
// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd,
  UINT message,
  WPARAM wParam,
  LPARAM lParam);

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR lpCmdLine,
  int nCmdShow)
{
  // the handle for the window, filled by a function
  HWND hWnd;
  // this struct holds information for the window class
  WNDCLASSEX wc;

  // clear out the window class for use
  ZeroMemory(&wc, sizeof(WNDCLASSEX));

  // fill in the struct with the needed information
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = WindowProc;
  wc.hInstance = hInstance;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  //wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
  wc.lpszClassName = "WindowClass1";

  // register the window class
  RegisterClassEx(&wc);
  
  // calculate the size of the client area
  RECT wr = { 0, 0, g_width, g_height };    // set the size, but not the position
  AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);    // adjust the size

  // create the window and use the result as the handle
  hWnd = CreateWindowEx(NULL,
    "WindowClass1",    // name of the window class
    "Pixels",   // title of the window
    WS_OVERLAPPEDWINDOW,    // window style
    300,    // x-position of the window
    300,    // y-position of the window
    g_width,    // width of the window
    g_height,    // height of the window
    NULL,    // we have no parent window, NULL
    NULL,    // we aren't using menus, NULL
    hInstance,    // application handle
    NULL);    // used with multiple windows, NULL

  // display the window on the screen
  ShowWindow(hWnd, nCmdShow);

  // set up and initialize Direct3D
  InitD3D(hWnd);

  // enter the main loop:

  // this struct holds Windows event messages
  MSG msg = { 0 };

  // Enter the infinite message loop
  while (TRUE)
  {
    // Check to see if any messages are waiting in the queue
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      // translate keystroke messages into the right format
      TranslateMessage(&msg);

      // send the message to the WindowProc function
      DispatchMessage(&msg);

      // check to see if it's time to quit
      if (msg.message == WM_QUIT)
      {
        break;
      }
    }
    else
    {
      // Run game code here
      RenderFrame();
    }
  }
  // return this part of the WM_QUIT message to Windows
  return msg.wParam;
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  // sort through and find what code to run for the message given
  switch (message)
  {
    // this message is read when the window is closed
    case WM_DESTROY:
      // close the application entirely
      PostQuitMessage(0);
      return 0;
    default:
      break;
  }

  // Handle any messages the switch statement didn't
  return DefWindowProc(hWnd, message, wParam, lParam);
}

//Common resolutions
//Resolution  Pixels    Widescreen
//800 x 600   480,000   No
//1024 x 768  786,432   No
//1152 x 864  995,328   No
//1280 x 1024 1,310,720 No
//1600 x 1200 1,920,000 No
//1440 x 900  1,296,000 Yes
//1680 x 1050 1,764,000 Yes
//1920 x 1080 2,073,600 Yes
//1920 x 1200 2,304,000 Yes
