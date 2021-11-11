/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#ifndef JOESCAN_NETWORK_INCLUDES_H
#define JOESCAN_NETWORK_INCLUDES_H

#if defined(__linux__) || defined(__APPLE__)

#include <arpa/inet.h>
// Windows specific includes missing in Linux
#ifndef SOCKET
#define SOCKET int
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif

#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif

#else

// TODO: inet_addr is deprecated in Windows
// suggested to use either inet_pton or InetPton
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#endif

#endif
