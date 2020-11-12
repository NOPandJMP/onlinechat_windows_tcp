#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <stdlib.h>
#pragma comment(lib, "ws2_32.lib")
static int _parse_address(const char *address, char *ipv4, char *port);

extern int listen_net(const char *address) {
	// Инициализация сокетов в Windows.
	// В Linux инициализация сокетов не производится.

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        return -2;
    }


	// Создаём сокет на прослушивание.
	// AF_INET - интернет протокол.
	// SOCK_STREAM - tcp соединение.
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        return -3;
    }

    // Даём возможность ОС применять сокет повторно.

    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &(char){1}, sizeof(char)) < 0) {
        return -4;
    }


    // Разбиваем адрес на ipv4 и port.
    char ipv4[16];
    char port[6];
    if (_parse_address(address, ipv4, port) != 0) {
        return -5;
    }

	// Загружаем адрес в структуру.
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(port));
    addr.sin_addr.s_addr = inet_addr(ipv4);

	// Связываем адрес с сокетом.
    if (bind(listener, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        return -6;
    }

	// Начинаем принимать соединение.
    if (listen(listener, SOMAXCONN) != 0) {
        return -7;
    }

    return listener;
}

extern int connect_net(const char *address) {
	// Инициализация сокетов в Windows.
// В Linux инициализация сокетов не производится.

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        return -2;
    }


// Создаём сокет на прослушивание.
// AF_INET - интернет протокол.
// SOCK_STREAM - tcp соединение.
    int conn = socket(AF_INET, SOCK_STREAM, 0);
    if (conn < 0) {
        return -3;
    }

// Разбиваем адрес на ipv4 и port.
    char ipv4[16];
    char port[6];
    if (_parse_address(address, ipv4, port) != 0) {
        return -4;
    }

// Загружаем адрес в структуру.
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(port));
    addr.sin_addr.s_addr = inet_addr(ipv4);

// Присоединяемся к серверу.
    if (connect(conn, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        return -5;
    }

    return conn;
}

extern int close_net(int conn) {
	#ifdef __linux__
        return close(conn);
    #elif __WIN32
        return closesocket(conn);
    #endif
}

extern int send_net(int conn, const char *data, size_t size) {
	return send(conn, data, size, 0);
}

extern int recv_net(int conn, char *data, size_t size) {
	return recv(conn, data, size, 0);
}

extern int accept_net(int listener) {
	return accept(listener, NULL, NULL);
}

static int _parse_address(const char *address, char *ipv4, char *port) {
    size_t i = 0, j = 0;
    for (; address[i] != ':'; ++i) {
        if (address[i] == '\0') {
            return 1;
        }
        if (i >= 16-1) {
            return 2;
        }
        ipv4[i] = address[i];
    }
    ipv4[i] = '\0';
    for (i += 1; address[i] != '\0'; ++i, ++j) {
        if (j >= 6-1) {
            return 3;
        }
        port[j] = address[i];
    }
    port[j] = '\0';
    return 0;
}
