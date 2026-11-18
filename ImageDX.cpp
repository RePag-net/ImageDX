/******************************************************************************
MIT License

Copyright(c) 2026 René Pagel

Filename: ImageDX.cpp
For more information see https://github.com/RePag-net/ImageDX

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/
#include "HImageDX.h"
#include "OImageD2.h"

using namespace RePag::DirectX;
//---------------------------------------------------------------------------
HKEY hSchlussel; DWORD dwBytes = 4;
//---------------------------------------------------------------------------
char pcRePag_Image[] = "RePag_Image";
char pcRePag_ImageBox[] = "RePag_ImageBox";

IWICImagingFactory* ifWICImageFactory = nullptr;

//---------------------------------------------------------------------------
void __vectorcall RegisterClassName(void)
{
  WNDCLASSEX wndKlasse;
  wndKlasse.cbSize = sizeof(WNDCLASSEX);
  wndKlasse.style = CS_OWNDC;
  wndKlasse.cbClsExtra = 0;
  wndKlasse.cbWndExtra = 16;
  wndKlasse.hInstance = nullptr;
  wndKlasse.hIcon = nullptr;
  wndKlasse.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wndKlasse.hbrBackground = nullptr;
  wndKlasse.lpszMenuName = nullptr;
  wndKlasse.hIconSm = nullptr;

  wndKlasse.lpszClassName = pcRePag_Image;
  wndKlasse.lpfnWndProc = WndProc_Image;
  //wndKlasse.style |= CS_HREDRAW | CS_VREDRAW;
  RegisterClassEx(&wndKlasse);
  wndKlasse.style = CS_OWNDC;

  wndKlasse.lpszClassName = pcRePag_ImageBox;
  //wndKlasse.lpfnWndProc = WndProc_ImageBox;
  wndKlasse.style |= CS_HREDRAW | CS_VREDRAW;
  RegisterClassEx(&wndKlasse);
  wndKlasse.style = CS_OWNDC;

  CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&ifWICImageFactory));
}
//---------------------------------------------------------------------------
void __vectorcall UnRegisterClassName(void)
{
  HINSTANCE hInstance = GetModuleHandle(NULL);

  UnregisterClass(pcRePag_Image, hInstance);
  UnregisterClass(pcRePag_ImageBox, hInstance);
}
//---------------------------------------------------------------------------