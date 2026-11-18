/******************************************************************************
MIT License

Copyright(c) 2026 René Pagel

Filename: OImageD2.cpp
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
//-------------------------------------------------------------------------------------------------------------------------------------------
char pcErrorSavingFile[] = "Error Saving File";
char pcErrorOpeningFile[] = "Error Opening File";
char pcErrorOpeningText[] = "The file could not be opened";
char pcErrorSavingText[] = "The file could not be saved";

const COMDLG_FILTERSPEC c_rgSaveTypes[] =
{
	{L"Bitmap Image (*.bmp)",						L"*.bmp"},
	{L"GIF Image    (*.gif",						L"*.gif"},
	{L"JPG Image		(*.jpg;*.jpeg)",		L"*.jpg;*.jpeg"},
	{L"TIF Image		(*.tif;*.tiff)",		L"*.tif;*.tiff"},
	{L"PNG Image		(*.png)",						L"*.png"}
};

#define INDEX_BMP 1
#define INDEX_GIF 2
#define INDEX_JPG 3
#define INDEX_TIF 4
#define INDEX_PNG 5
//-------------------------------------------------------------------------------------------------------------------------------------------
RePag::DirectX::COImage* __vectorcall RePag::DirectX::COImageV(_In_z_ const char* pcWindowName, _In_ unsigned int uiIDElement,
																															 _In_ STDeviceResources* pstDeviceResourcesA)
{
	COImage* vImage = (COImage*)VMBlock(VMDialog(), sizeof(COImage));
	vImage->COImageV(VMDialog(), pcWindowName, uiIDElement, pstDeviceResourcesA);
	return vImage;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
RePag::DirectX::COImage* __vectorcall RePag::DirectX::COImageV(_In_ VMEMORY vmMemory, _In_z_ const char* pcWindowName, _In_ unsigned int uiIDElement,
																															 _In_ STDeviceResources* pstDeviceResourcesA)
{
	COImage* vImage = (COImage*)VMBlock(vmMemory, sizeof(COImage));
	vImage->COImageV(vmMemory, pcWindowName, uiIDElement, pstDeviceResourcesA);
	return vImage;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK RePag::DirectX::WndProc_Image(_In_ HWND hWnd, _In_ unsigned int uiMessage, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	COImage* pImage;
	switch(uiMessage){
		case WM_CREATE			: ((COImage*)((LPCREATESTRUCT)lParam)->lpCreateParams)->WM_Create_Element(hWnd);
													return NULL;
		case WM_SIZE				: pImage = (COImage*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
													if(pImage) pImage->WM_Size(lParam);
													else return DefWindowProc(hWnd, uiMessage, wParam, lParam);
													return NULL;
		case WM_INITPAINT		: ((COImage*)GetWindowLongPtr(hWnd, GWLP_USERDATA))->WM_InitPaint();
													return NULL;
		case WM_KILLFOCUS		: pImage = (COImage*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
													if(pImage->pfnWM_KillFocus){ pImage->ThreadSafe_Begin(); pImage->pfnWM_KillFocus(pImage); pImage->ThreadSafe_End(); }
													return NULL;
		case WM_KEYDOWN			: pImage = (COImage*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
													if(IsWindowEnabled(hWnd)) pImage->WM_KeyDown(wParam);
													return NULL;
		case WM_COMMAND			: pImage = (COImage*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
													if(!pImage->WM_Command(wParam)) return NULL;
													else if(pImage->pfnWM_Command){
														pImage->ThreadSafe_Begin();
														if(!pImage->pfnWM_Command(pImage, wParam)){ pImage->ThreadSafe_End(); return NULL; }
														pImage->ThreadSafe_End();
													}
													else PostMessage(GetParent(hWnd), WM_COMMAND, wParam, lParam);
													break;
		case WM_CONTEXTMENU	: ((COImage*)GetWindowLongPtr(hWnd, GWLP_USERDATA))->WM_ContexMenu(lParam);
													return NULL;
		case WM_LBUTTONDOWN	: SetFocus(hWnd);
													return NULL;
		case WM_LBUTTONUP		: PostMessage(GetParent(hWnd), WM_COMMAND, MAKEWPARAM(GetWindowLongPtr(hWnd, GWLP_ID), wParam), lParam);
													break;
		case WM_NCDESTROY		: pImage = (COImage*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
													VMFreiV(pImage);
													return NULL;
	}
	return DefWindowProc(hWnd, uiMessage, wParam, lParam);
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COImage::COImageV(_In_ VMEMORY vmMemory, _In_z_ const char* pcWindowName, _In_ unsigned int uiIDElementA,
																										_In_ STDeviceResources* pstDeviceResourcesA)
{
	COGraphicV(vmMemory, pcRePag_Image, pcWindowName, uiIDElementA, pstDeviceResourcesA);

	bChanged = false;
	ucRight = 255;

	asInfoText = "No Image";
	crfBackground = D2D1::ColorF(RGB(110, 110, 110), 0.0f);
	crfText = D2D1::ColorF(RGB(240, 240, 240), 1.0f);
	ifTextColor = nullptr;

	pstDeviceResources->ifdwriteFactory7->CreateTextFormat(L"Arial", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
																												 13.0f, L"de-DE", &ifText);

	stImage.ulBytes = 0;
  stImage.vbImage = nullptr;
  stImage.pwstrFileName = nullptr;
	ifd2dBitmapWIC = nullptr;
	fRotation = 0.0f;

	hMenu = CreatePopupMenu();
	AppendMenu(hMenu, MF_STRING, IDM_DATEI_OFFNEN, "Open Image		Strg+O");
	AppendMenu(hMenu, MF_STRING, IDM_DATEI_SPEICHERNUNTER, "Save Image As...");
	AppendMenu(hMenu, MF_STRING, IDM_DATEI_SPEICHERN, "Save Image		Strg+S");
	AppendMenu(hMenu, MF_STRING, IDM_DATEI_ENTFERNEN, "Delete Image		Strg+Umsch+Entf");

	AppendMenu(hMenu, MF_SEPARATOR, NULL, NULL);

	AppendMenu(hMenu, MF_STRING, IDM_IMAGEROTATION, "Rotate 90 degrees");

	pfnWM_Char_Return = nullptr;
	pfnWM_Char_Escape = nullptr;
	pfnWM_KillFocus = nullptr;
	pfnWM_Command = nullptr;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
VMEMORY __vectorcall RePag::DirectX::COImage::COFreiV(void)
{
  if(stImage.vbImage) VMFrei(stImage.vbImage);
	if(stImage.pwstrFileName) CoTaskMemFree(stImage.pwstrFileName);
	SafeRelease(&ifd2dBitmapWIC); SafeRelease(&ifTextColor);
	DestroyMenu(hMenu);
	return ((COElement*)this)->COFreiV();
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COImage::OnRender(void)
{
	ifD2D1Context6->BeginDraw();
	ifD2D1Context6->Clear(crfBackground);

	if(ifd2dBitmapWIC){
		D2D1::Matrix3x2F tfPrevTransform;
		ifD2D1Context6->GetTransform(&tfPrevTransform);

		float fFaktor_height; float fFaktor_width;
		if(fRotation == 90.0f){
			fFaktor_height = (float)lHeight / (float)stImage.uiWidth;
			D2D1_MATRIX_3X2_F rotation = D2D1::Matrix3x2F::Rotation(fRotation, D2D1::Point2F((float)lWidth / 2.0f, (float)lHeight / 2.0f));
			D2D1_MATRIX_3X2_F scale = D2D1::Matrix3x2F::Scale(D2D1::Size(fFaktor_height, fFaktor_height), D2D1::Point2F((float)lWidth, 0.0f));
			ifD2D1Context6->SetTransform(rotation * scale);

			ifD2D1Context6->DrawBitmap(ifd2dBitmapWIC, D2D1::RectF(0.0f, 0.0f, (float)stImage.uiWidth, (float)stImage.uiHeight), 1.0f,
																 D2D1_INTERPOLATION_MODE_MULTI_SAMPLE_LINEAR);
		}
		else{
			fFaktor_height = (float)lHeight / (float)stImage.uiHeight;
			fFaktor_width = (float)lWidth / (float)stImage.uiWidth;

			if(fFaktor_height > fFaktor_width)
				ifD2D1Context6->DrawBitmap(ifd2dBitmapWIC, D2D1::RectF(0.0f, 0.0f, (float)lWidth, (float)stImage.uiHeight * fFaktor_width), 1.0f,
																	 D2D1_INTERPOLATION_MODE_MULTI_SAMPLE_LINEAR);
			else
				ifD2D1Context6->DrawBitmap(ifd2dBitmapWIC, D2D1::RectF(0.0f, 0.0f, (float)stImage.uiWidth * fFaktor_height, (float)lHeight), 1.0f,
																	 D2D1_INTERPOLATION_MODE_MULTI_SAMPLE_LINEAR);
		}
		ifD2D1Context6->SetTransform(tfPrevTransform);
	}
	else{
		IDWriteTextLayout* ifTextLayout; size_t szBytes_Text; WCHAR wc255Content[255];

		D2D1_RECT_F rcfText = D2D1::RectF(0.0f, 0.0f, 0.0f, 0.0f);
		if(mbstowcs_s(&szBytes_Text, wc255Content, 255, asInfoText.c_Str(), asInfoText.Length())) goto End;
		if(FAILED(pstDeviceResources->ifdwriteFactory7->CreateTextLayout(wc255Content, (UINT32)szBytes_Text, ifText,
																																		 (float)lWidth, (float)lHeight, &ifTextLayout))) goto End;

		DWRITE_TEXT_METRICS stTextMetrics;
		ifTextLayout->GetMetrics(&stTextMetrics);
		rcfText.left = ((float)lWidth - stTextMetrics.width) / 2.0f;
		rcfText.top = ((float)lHeight - stTextMetrics.height) / 2.0f;
    rcfText.right = rcfText.left + stTextMetrics.width;
    rcfText.bottom = rcfText.top + stTextMetrics.height;
		SafeRelease(&ifTextLayout);
		ifD2D1Context6->DrawText(wc255Content, (UINT32)szBytes_Text, ifText, rcfText, ifTextColor, D2D1_DRAW_TEXT_OPTIONS_CLIP);
	}

End:
	ifD2D1Context6->EndDraw();
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COImage::OnPaint(void)
{
	ThreadSafe_Begin();
	OnRender();
	rclDirty.left = 0; rclDirty.top = 0; rclDirty.right = lWidth; rclDirty.bottom = lHeight;
	ifDXGISwapChain4->Present1(0, NULL, &dxgiPresent);
	ThreadSafe_End();
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COImage::WM_InitPaint(void)
{
	if(crfBackground.a == 0.0f) crfBackground.a = 1.0f;

	ifD2D1Context6->CreateSolidColorBrush(crfText, &ifTextColor);
	OnRender();
	ifDXGISwapChain4->Present1(0, NULL, &dxgiPresent);
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COImage::WM_Size(_In_ LPARAM lParam)
{
	ThreadSafe_Begin();
	if(lHeight != HIWORD(lParam) || lWidth != LOWORD(lParam)){
		lHeight = HIWORD(lParam); lWidth = LOWORD(lParam);
		CreateWindowSizeDependentResources();

		OnRender();
		ifDXGISwapChain4->Present1(1, NULL, &dxgiPresent);
	}
	ThreadSafe_End();
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COImage::WM_KeyDown(_In_ WPARAM wParam)
{
	switch(wParam){
		case VK_RETURN	: ThreadSafe_Begin();
											if(pfnWM_Char_Return) pfnWM_Char_Return(this);
											ThreadSafe_End();
											break;
		case VK_ESCAPE	: ThreadSafe_Begin();
											if(pfnWM_Char_Escape) pfnWM_Char_Escape(this);
											ThreadSafe_End();
											break;
		case VK_DELETE	: ThreadSafe_Begin();
											if(GetKeyState(VK_SHIFT) & 0x8000 && GetKeyState(VK_CONTROL) & 0x8000) Delete();
											ThreadSafe_End();
											break;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------------
bool __vectorcall RePag::DirectX::COImage::WM_Command(_In_ WPARAM wParam)
{
	switch(LOWORD(wParam)){
		case IDM_FILE_OPEN			: ThreadSafe_Begin();
															if(IsWindowEnabled(hWndElement) && ucRight & AR_OPEN) FileOpen();
															ThreadSafe_End(); return false;
		case IDM_FILE_SAVE			: ThreadSafe_Begin();
															if(IsWindowEnabled(hWndElement) && ucRight & AR_SAVE) FileSave();
															ThreadSafe_End(); return false;
		case IDM_FILE_SAVE_AS		: ThreadSafe_Begin();
															if(IsWindowEnabled(hWndElement) && ucRight & AR_SAVE) FileSaveAs();
															ThreadSafe_End(); return false;
		case IDM_FILE_DELETE		: ThreadSafe_Begin();
															if(IsWindowEnabled(hWndElement) && ucRight & AR_DELETE) Delete();
															ThreadSafe_End(); return false;
		case IDM_IMAGEROTATION	: ThreadSafe_Begin();
															if(!fRotation) fRotation += 90.0f;
															else fRotation = 0.0f;
															OnRender();
															ifDXGISwapChain4->Present1(1, NULL, &dxgiPresent);
															ThreadSafe_End(); return false;
		default									: return true;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COImage::WM_ContexMenu(_In_ LPARAM lParam)
{
	ThreadSafe_Begin();
	SetFocus(hWndElement);

	if(ucRight & AR_OPEN) EnableMenuItem(hMenu, IDM_FILE_OPEN, MF_BYCOMMAND | MF_ENABLED);
	else EnableMenuItem(hMenu, IDM_FILE_OPEN, MF_BYCOMMAND | MF_GRAYED);
	if(ucRight & AR_SAVE && ifd2dBitmapWIC) EnableMenuItem(hMenu, IDM_FILE_SAVE_AS, MF_BYCOMMAND | MF_ENABLED);
	else EnableMenuItem(hMenu, IDM_FILE_SAVE_AS, MF_BYCOMMAND | MF_GRAYED);
	if(ucRight & AR_SAVE && ifd2dBitmapWIC) EnableMenuItem(hMenu, IDM_FILE_SAVE, MF_BYCOMMAND | MF_ENABLED);
	else EnableMenuItem(hMenu, IDM_FILE_SAVE, MF_BYCOMMAND | MF_GRAYED);
	if(ucRight & AR_DELETE && ifd2dBitmapWIC) EnableMenuItem(hMenu, IDM_FILE_DELETE, MF_BYCOMMAND | MF_ENABLED);
	else EnableMenuItem(hMenu, IDM_FILE_DELETE, MF_BYCOMMAND | MF_GRAYED);
	if(ifd2dBitmapWIC) EnableMenuItem(hMenu, IDM_IMAGEROTATION, MF_BYCOMMAND | MF_ENABLED);
	else EnableMenuItem(hMenu, IDM_IMAGEROTATION, MF_BYCOMMAND | MF_GRAYED);

	POINT ptPosition;
	ptPosition.x = LOWORD(lParam); ptPosition.y = HIWORD(lParam);
	if(ptPosition.x == USHRT_MAX && ptPosition.y == USHRT_MAX) ClientToScreen(GetParent(hWndElement), &Position(ptPosition));
	TrackPopupMenuEx(hMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON, ptPosition.x, ptPosition.y, hWndElement, nullptr);
	ThreadSafe_End();
}
//-------------------------------------------------------------------------------------------------------------------------------------------
HRESULT __vectorcall RePag::DirectX::COImage::FileOpen(void)
{
	IFileDialog* ifFileDialog = nullptr; DWORD dwFlags; IShellItem* psiResult = nullptr; PWSTR pszFilePath = nullptr;	HRESULT hr;
	ThreadSafe_Begin();
	if(FAILED(hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&ifFileDialog)))) goto End;
	if(FAILED(hr = ifFileDialog->GetOptions(&dwFlags))) goto End;
	if(FAILED(hr = ifFileDialog->SetOptions(dwFlags | FOS_FORCEFILESYSTEM))) goto End;
	if(FAILED(hr = ifFileDialog->SetFileTypes(ARRAYSIZE(c_rgSaveTypes), c_rgSaveTypes))) goto End;
	if(FAILED(hr = ifFileDialog->SetFileTypeIndex(INDEX_JPG))) goto End;
	if(FAILED(hr = ifFileDialog->SetDefaultExtension(L"jpg"))) goto End;
	if(FAILED(hr = ifFileDialog->Show(NULL))) goto End;
	if(FAILED(hr = ifFileDialog->GetResult(&psiResult))) goto End;
  if(stImage.pwstrFileName){ CoTaskMemFree(stImage.pwstrFileName); stImage.pwstrFileName = nullptr; }
	if(FAILED(hr = psiResult->GetDisplayName(SIGDN_NORMALDISPLAY, &stImage.pwstrFileName))) goto End;
	if(FAILED(hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath))) goto End;

	size_t szTest; char cFilePath[MAX_PATH];
	if(!wcstombs_s(&szTest, cFilePath, MAX_PATH, pszFilePath, MAX_PATH)){
		HANDLE hDatei = CreateFile(cFilePath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if(hDatei == INVALID_HANDLE_VALUE){ MessageBox(hWndElement, pcErrorOpeningText, pcErrorOpeningFile, MB_ICONHAND); goto End; }
		else{
			ULONG ulBytes = GetFileSize(hDatei, nullptr);
			if(ulBytes != 0xFFFFFFFF){
				if(stImage.vbImage) VMFrei(stImage.vbImage);
				stImage.vbImage = VMBlock(ulBytes);
				if(ReadFile(hDatei, stImage.vbImage, ulBytes, &stImage.ulBytes, nullptr)) bChanged = true;
			}
			CloseHandle(hDatei);
		}
	}
	CoTaskMemFree(pszFilePath);

	if(FAILED(hr = Load())) goto End;
	OnRender();
	ifDXGISwapChain4->Present1(1, NULL, &dxgiPresent);

End:
	SafeRelease(&ifFileDialog); SafeRelease(&psiResult);
	ThreadSafe_End();
	return hr;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
HRESULT __vectorcall RePag::DirectX::COImage::FileSaveAs(void)
{
	IFileDialog* ifFileDialog = nullptr; DWORD dwFlags; IShellItem* psiResult = nullptr; PWSTR pszFilePath = nullptr;	HRESULT hr;
	ThreadSafe_Begin();
	if(FAILED(hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&ifFileDialog)))) goto End;
	if(FAILED(hr = ifFileDialog->GetOptions(&dwFlags))) goto End;
	if(FAILED(hr = ifFileDialog->SetOptions(dwFlags | FOS_FORCEFILESYSTEM))) goto End;
	if(FAILED(hr = ifFileDialog->SetFileTypes(ARRAYSIZE(c_rgSaveTypes), c_rgSaveTypes))) goto End;
	if(stImage.guidContainerFormat == GUID_ContainerFormatBmp){	if(FAILED(hr = ifFileDialog->SetFileTypeIndex(INDEX_BMP))) goto End; }
  else if(stImage.guidContainerFormat == GUID_ContainerFormatGif){ if(FAILED(hr = ifFileDialog->SetFileTypeIndex(INDEX_GIF))) goto End; }
  else if(stImage.guidContainerFormat == GUID_ContainerFormatJpeg){ if(FAILED(hr = ifFileDialog->SetFileTypeIndex(INDEX_JPG))) goto End; }
  else if(stImage.guidContainerFormat == GUID_ContainerFormatTiff){ if(FAILED(hr = ifFileDialog->SetFileTypeIndex(INDEX_TIF))) goto End; }
  else if(stImage.guidContainerFormat == GUID_ContainerFormatPng){ if(FAILED(hr = ifFileDialog->SetFileTypeIndex(INDEX_PNG))) goto End; }
	if(FAILED(hr = ifFileDialog->SetFileName(stImage.pwstrFileName))) goto End;
	if(FAILED(hr = ifFileDialog->Show(NULL))) goto End;
	if(FAILED(hr = ifFileDialog->GetResult(&psiResult))) goto End;
	if(FAILED(hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath))) goto End;

	size_t szTest; char cFilePath[MAX_PATH];
	if(!wcstombs_s(&szTest, cFilePath, MAX_PATH, pszFilePath, MAX_PATH)){
		HANDLE hDatei = CreateFile(cFilePath, GENERIC_WRITE, NULL, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if(hDatei == INVALID_HANDLE_VALUE) MessageBox(hWndElement, pcErrorSavingText, pcErrorSavingFile, MB_ICONHAND);
		else{
			if(WriteFile(hDatei, stImage.vbImage, stImage.ulBytes, nullptr, nullptr)) hr = S_OK;
			CloseHandle(hDatei);
		}
	}
	CoTaskMemFree(pszFilePath);

End:
	SafeRelease(&ifFileDialog); SafeRelease(&psiResult);
	ThreadSafe_End();
	return hr;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
HRESULT __vectorcall RePag::DirectX::COImage::FileSave(void)
{
	return FileSaveAs();
}
//-------------------------------------------------------------------------------------------------------------------------------------------
HRESULT __vectorcall RePag::DirectX::COImage::Load(void)
{
	IWICStream* ifWICStream = nullptr; HRESULT hr;
	IWICFormatConverter* ifConverter = nullptr;	IWICBitmapFrameDecode* ifFrameDecode = nullptr;	IWICBitmapDecoder* ifDecoder = nullptr;
	if(FAILED(hr = ifWICImageFactory->CreateStream(&ifWICStream))) goto End;
	if(FAILED(hr = ifWICStream->InitializeFromMemory((WICInProcPointer)stImage.vbImage, stImage.ulBytes))) goto End;
	if(FAILED(hr = ifWICImageFactory->CreateDecoderFromStream(ifWICStream, NULL, WICDecodeMetadataCacheOnLoad, &ifDecoder))) goto End;
	if(FAILED(hr = ifDecoder->GetContainerFormat(&stImage.guidContainerFormat))) goto End;
	if(FAILED(hr = ifDecoder->GetFrame(0, &ifFrameDecode))) goto End;
	if(FAILED(hr = ifWICImageFactory->CreateFormatConverter(&ifConverter))) goto End;
	if(FAILED(hr = ifConverter->Initialize(ifFrameDecode, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.f, WICBitmapPaletteTypeCustom))) goto End;
	if(FAILED(hr = ifConverter->GetSize(&stImage.uiWidth, &stImage.uiHeight))) goto End;

	if(ifd2dBitmapWIC){ SafeRelease(&ifd2dBitmapWIC); ifd2dBitmapWIC = nullptr; }
	if(FAILED(hr = ifD2D1Context6->CreateBitmapFromWicBitmap(ifConverter, nullptr, &ifd2dBitmapWIC))) goto End;
	bChanged = true;

End:
	SafeRelease(&ifWICStream); SafeRelease(&ifDecoder); SafeRelease(&ifFrameDecode); SafeRelease(&ifConverter);
	return hr;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
VMBLOCK __vectorcall RePag::DirectX::COImage::Read(_Out_ VMBLOCK& vbImage, _Out_ unsigned long& ulBytes)
{
	ulBytes = stImage.ulBytes;
	vbImage = VMBlock(stImage.ulBytes);
  MemCopy(vbImage, stImage.vbImage, stImage.ulBytes);
	return vbImage;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
HRESULT __vectorcall RePag::DirectX::COImage::Write(_In_ VMBLOCK vbImage, _In_ unsigned long ulBytes)
{
  stImage.ulBytes = ulBytes;
  stImage.vbImage = VMBlock(ulBytes);
  MemCopy(stImage.vbImage, vbImage, ulBytes);

	HRESULT hr = Load();
	return hr;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COImage::Delete(void)
{
	ThreadSafe_Begin();
	stImage.ulBytes = 0;
  if(stImage.vbImage){ VMFrei(stImage.vbImage); stImage.vbImage = nullptr; }
	if(ifd2dBitmapWIC){
		SafeRelease(&ifd2dBitmapWIC); ifd2dBitmapWIC = nullptr;
		OnRender();
		ifDXGISwapChain4->Present1(1, NULL, &dxgiPresent);
	}
	bChanged = true;
	ThreadSafe_End();
}
//-------------------------------------------------------------------------------------------------------------------------------------------
bool __vectorcall RePag::DirectX::COImage::Changed(void)
{
	return bChanged;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COImage::Right(_In_ unsigned char ucRightA)
{
	ThreadSafe_Begin();
	ucRight = ucRightA;
	ThreadSafe_End();
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COImage::SetTextColor(_In_ unsigned char ucRed, _In_ unsigned char ucGreen, _In_ unsigned char ucBlue, _In_ float fAlpha)
{
	ThreadSafe_Begin();
	crfText = D2D1::ColorF(RGB(ucBlue, ucGreen, ucRed), fAlpha);
	if(ifTextColor) ifTextColor->SetColor(crfText);
	ThreadSafe_End();
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COImage::SetTextColor(_In_ D2D1_COLOR_F& stTextA)
{
	ThreadSafe_Begin();
	crfText = stTextA;
	if(ifTextColor) ifTextColor->SetColor(crfText);
	ThreadSafe_End();
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COImage::InfoText(_In_z_ char* pcInfoText)
{
	ThreadSafe_Begin();
	asInfoText = pcInfoText;
	ThreadSafe_End();
}
//-------------------------------------------------------------------------------------------------------------------------------------------