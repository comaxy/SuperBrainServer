#include <Windows.h>
#include <WinSock.h>

#define WM_SOCKET (WM_USER + 1000)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
	)
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof (wc);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = TEXT("SuperBrainWndClass");
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	if (!RegisterClassEx(&wc))
	{
		return -1;
	}

	HWND hWnd = CreateWindowEx(0, TEXT("SuperBrainWndClass"), TEXT(""), WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{
		return -1;
	}

	ShowWindow(hWnd, SW_SHOWNORMAL);
	UpdateWindow(hWnd);

	WSADATA wsaData = { 0 };
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		return -1;
	}

	SOCKET hLstnSock = socket(AF_INET, SOCK_STREAM, 0);
	if (hLstnSock == INVALID_SOCKET)
	{
		return -1;
	}

	if (SOCKET_ERROR == WSAAsyncSelect(hLstnSock, hWnd, WM_SOCKET, FD_ACCEPT | FD_READ | FD_WRITE | FD_CLOSE))
	{
		return -1;
	}

	SOCKADDR_IN pstSockName = { 0 };
	pstSockName.sin_family = PF_INET;
	pstSockName.sin_port = htons(7062);
	if (SOCKET_ERROR == bind(hLstnSock, (LPSOCKADDR)&pstSockName, sizeof(pstSockName)))
	{
		return -1;
	}

	if (SOCKET_ERROR == listen(hLstnSock, 5))
	{
		return -1;
	}
	
	MSG msg = { 0 };
	BOOL bRet;
	while ((bRet = GetMessage(&msg, hWnd, 0, 0)) != 0)
	{
		if (bRet == -1)
		{
			return -1;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	WSACleanup();

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SOCKET:
		{
			SOCKET sock = (SOCKET)wParam;
			WORD wsaEvent = WSAGETSELECTEVENT(lParam);
			WORD wsaError = WSAGETASYNCERROR(lParam);
			switch (wsaEvent)
			{
			case FD_READ:
				break;
			case FD_WRITE:
				break;
			case FD_ACCEPT:
				break;
			case FD_CLOSE:
				break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
