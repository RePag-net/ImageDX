/******************************************************************************
MIT License

Copyright(c) 2026 René Pagel

Filename: OImageD2.h
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
#pragma once
namespace RePag
{
	namespace DirectX
	{
		//-----------------------------------------------------------------------------------------------------------------------------------------
		class __declspec(dllexport) COImage : public COGraphic
		{
				friend LRESULT CALLBACK WndProc_Image(_In_ HWND hWnd, _In_ unsigned int uiMessage, _In_ WPARAM wParam, _In_ LPARAM lParam);

			private:
				struct STImage
				{
					PWSTR pwstrFileName;
					VMBLOCK vbImage;
					unsigned long ulBytes;
					GUID guidContainerFormat;
          unsigned int uiWidth;
          unsigned int uiHeight;
				};
				STImage stImage;
				HMENU hMenu;
				ID2D1Bitmap1* ifd2dBitmapWIC;
				float fRotation;
				D2D1_COLOR_F crfText;
				ID2D1SolidColorBrush* ifTextColor;
				IDWriteTextFormat* ifText;
				COStringA asInfoText;
				bool bChanged;
				unsigned char ucRight;
				void __vectorcall OnRender(void);
				void __vectorcall WM_InitPaint(void);
				void __vectorcall WM_Size(_In_ LPARAM lParam);
				void __vectorcall WM_KeyDown(_In_ WPARAM wParam);
				bool __vectorcall WM_Command(_In_ WPARAM wParam);
				void __vectorcall WM_ContexMenu(_In_ LPARAM lParam);

			protected:

			public:
				void __vectorcall COImageV(_In_ VMEMORY vmMemory, _In_z_ const char* pcWindowName, _In_z_ unsigned int uiIDElementA,
																	 _In_ STDeviceResources* pstDeviceResourcesA);
				VMEMORY __vectorcall COFreiV(void);
				void(__vectorcall* pfnWM_Char_Return)(_In_ COImage*);
				void(__vectorcall* pfnWM_Char_Escape)(_In_ COImage*);
				void(__vectorcall* pfnWM_KillFocus)(_In_ COImage*);
				bool(__vectorcall* pfnWM_Command)(_In_ COImage*, WPARAM);
				HRESULT __vectorcall FileOpen(void);
				HRESULT __vectorcall FileSaveAs(void);
				HRESULT __vectorcall FileSave(void);
				HRESULT __vectorcall Load(void);
				VMBLOCK __vectorcall Read(_Out_ VMBLOCK& vbImage, _Out_ unsigned long& ulBytes);
				HRESULT __vectorcall Write(_In_ VMBLOCK vbImage, _In_ unsigned long ulBytes);
				void __vectorcall Delete(void);
				bool __vectorcall Changed(void);
				void __vectorcall Right(_In_ unsigned char ucRightA);
				//void __vectorcall SetzVerfugbar(bool bVerfugbar);
				void __vectorcall InfoText(_In_z_ char* pcInfoText);
				void __vectorcall SetTextColor(_In_ unsigned char ucRed, _In_ unsigned char ucGreen, _In_ unsigned char ucBlue, _In_ float fAlpha);
				void __vectorcall SetTextColor(_In_ D2D1_COLOR_F& stTextA);
				void __vectorcall OnPaint(void);

		};
		//-----------------------------------------------------------------------------------------------------------------------------------------
		__declspec(dllexport) COImage* __vectorcall COImageV(_In_z_ const char* pcWindowName, _In_ unsigned int uiIDElement, 
																												 _In_ STDeviceResources* pstDeviceResourcesA);
		__declspec(dllexport) COImage* __vectorcall COImageV(_In_ VMEMORY vmMemory, _In_z_ const char* pcWindowName, _In_ unsigned int uiIDElement,
																												 _In_ STDeviceResources* pstDeviceResourcesA);
		//-----------------------------------------------------------------------------------------------------------------------------------------
	}
}