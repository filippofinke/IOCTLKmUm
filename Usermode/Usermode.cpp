#include <iostream>
#include <Windows.h>

int main()
{
    const char* message = "IOCTLKmUm - Hello from usermode.";
   
    HANDLE driver = CreateFile(L"\\\\.\\IOCTLKmUm", GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (driver == NULL) {
        std::cout << "Failed getting handle to the driver!" << std::endl;
        return 0;
    }
    std::cout << "Got driver handle!" << std::endl;

    char buffer[50];
    DWORD read = 0;
    DWORD code = 0x800; // Hello!

    DeviceIoControl(driver, code, (LPVOID)message, strlen(message), buffer, sizeof(buffer), &read, NULL);
    std::cout << "Message received: " << buffer << std::endl;
    std::cout << "Read " << read << " bytes!" << std::endl;
    CloseHandle(driver);
    getchar();
    return 0;
}
