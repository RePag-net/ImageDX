/******************************************************************************
MIT License

Copyright(c) 2026 René Pagel

Filename: HImageDX.cpp
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

#pragma comment(lib, "CompSys")
#pragma comment(lib, "ADT")
#pragma comment(lib, "ProgDlgDX")
#pragma comment(lib, "StdElemDX")
#pragma comment(lib, "D2D1")

void __vectorcall RegisterClassName(void);
void __vectorcall UnRegisterClassName(void);
//---------------------------------------------------------------------------
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ulReason, LPVOID lpReserved)
{
  switch(ulReason){
    case DLL_PROCESS_ATTACH: RegisterClassName(); break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH: break;
    case DLL_PROCESS_DETACH: UnRegisterClassName(); break;
  }
  return true;
}