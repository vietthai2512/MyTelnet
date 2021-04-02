#include <iostream>
#include <WinSock2.h>

DWORD WINAPI ClientThread(LPVOID param);
int main(int argc, char** argv)
{
    if (argc < 2)
    {
        return 0;
    }
    else
    {
        short port = atoi(argv[1]);
        WSADATA DATA;
        WSAStartup(MAKEWORD(2, 2), &DATA);
        SOCKADDR_IN saddr;
        SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        saddr.sin_family = AF_INET;
        saddr.sin_addr.S_un.S_addr = INADDR_ANY;
        saddr.sin_port = htons(port);
        bind(s, (sockaddr*) &saddr, sizeof(saddr));
        listen(s, 10);
        while (true)
        {
            SOCKADDR_IN caddr;
            int clen = sizeof(caddr);
            SOCKET c = accept(s, (sockaddr*) &caddr, &clen);
            CreateThread(NULL, 0, ClientThread, (LPVOID) c, 0, NULL);
        }
    }
}

DWORD WINAPI ClientThread(LPVOID param)
{
    SOCKET c = (SOCKET)param;
    char* welcome = (char*) "Nhap [username, password]\n";
    send(c, welcome, strlen(welcome), 0);
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    recv(c, buffer, sizeof(buffer), 0);

    // Trim
    while (buffer[strlen(buffer) - 1] == '\r' || buffer[strlen(buffer) - 1] == '\n')
    {
        buffer[strlen(buffer) - 1] = 0;
    }

    // [user, password]
    char user[1024], password[1024];
    memset(user, 0, sizeof(user));
    memset(password, 0, sizeof(password));
    sscanf(buffer, "%s%s", user, password);

    // Matching
    int matched = 0;
    FILE* f = fopen("F:\\Code\\NetworkProgramming\\MyTelnetData\\TelnetUsers.txt", "rt");
    while (!feof(f))
    {
        char line[1024];
        memset(line, 0, sizeof(line));
        fgets(line, sizeof(line), f);
        char _u[1024], _p[1024];
        memset(_u, 0, sizeof(_u));
        memset(_p, 0, sizeof(_p));
        sscanf(line, "%s%s", _u, _p);
        if (strcmp(user, _u) == 0 && strcmp(password, _p) == 0)
        {
            matched = 1;
            break;
        }
    }
    fclose(f);
    if (matched == 0)
    {
        char* failed = (char*)"Failed to login\n";
        send(c, failed, strlen(failed), 0);
        closesocket(c);
    }
    else
    {
        char* success = (char*)"Please send command to execute\n";
        send(c, success, strlen(success), 0);
        memset(buffer, 0, sizeof(buffer));
        recv(c, buffer, sizeof(buffer), 0);
        
        while (buffer[strlen(buffer) - 1] == '\r' || buffer[strlen(buffer) - 1] == '\n')
        {
            buffer[strlen(buffer) - 1] = 0;
        }

        sprintf(buffer + strlen(buffer), "%s", "> F:\\Code\\NetworkProgramming\\MyTelnetData\\telnet.txt");
        system(buffer);

        FILE* f = fopen("F:\\Code\\NetworkProgramming\\MyTelnetData\\telnet.txt", "rb");
        fseek(f, 0, SEEK_END); // Dua con tro ve cuoi file
        int flen = ftell(f); // Lay vi tri con tro
        fseek(f, 0, SEEK_SET); // Dua con tro ve lai dau file
        char* fdata = (char*)calloc(flen, 1);
        fread(fdata, 1, flen, f);
        fclose(f);
        send(c, fdata, flen, 0);
        free(fdata);
        closesocket(c);
    }
    return 0;
}