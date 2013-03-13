
#include <Windows.h>
#include <cstdio>

int main (int argc, char** argv)
{           
	if (RegisterHotKey(nullptr, 1, MOD_ALT | MOD_NOREPEAT, 0x42)) { //0x42 is 'b'
		printf("Hotkey 'ALT+b' registered, using MOD_NOREPEAT flag\n");
	}

	MSG msg = {0};
	while (GetMessage(&msg, NULL, 0, 0) != 0) {
		if (msg.message == WM_HOTKEY) {
			printf("WM_HOTKEY received\n");            
		}
	} 

	return 0;
}
