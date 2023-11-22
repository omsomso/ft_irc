# IRC overview

The IRC (Internet Relay Chat) protocol is a text-based communication protocol used for real-time communication over the Internet. It was originally designed in the late 1980s and has undergone various updates and extensions. The core specifications can be found in RFC 1459 and RFC 2812, with additional modifications and extensions in subsequent RFCs.

Here's a high-level overview of the IRC protocol:

1. **Connection Establishment:**
   - IRC operates over a client-server model. Clients connect to servers using a TCP connection.
   - The standard port for IRC is 6667 (or 6697 for SSL-encrypted connections).

2. **Messages:**
   - Communication in IRC is based on messages. Messages are plain text strings.
   - Each message consists of a command, parameters, and optional additional information.

3. **Commands:**
   - IRC commands are three or more uppercase letters. Common commands include:
     - **NICK:** Set or change the nickname of the user.
     - **USER:** Specify username, hostname, servername, and realname.
     - **JOIN:** Join a channel.
     - **PART:** Leave a channel.
     - **PRIVMSG:** Send a private message to a user or a message to a channel.
     - **PING:** Used for keep-alive and measuring the round-trip time to the server.
     - **PONG:** Respond to a PING message from the server.

4. **Parameters:**
   - Parameters provide additional information for a command.
   - Parameters are space-separated in the message.
   - Some commands require specific parameters, e.g., JOIN requires the name of the channel to join.

5. **Channels:**
   - Channels are virtual rooms where users can communicate with each other.
   - Channel names start with a '#' character.
   - Users can join and leave channels using JOIN and PART commands.

6. **Nicknames:**
   - Users have nicknames, which are used to identify them in the IRC network.
   - NICK command is used to set or change the nickname.

7. **Modes:**
   - Modes are used to set various attributes on users and channels.
   - Examples include setting a user as an operator (`+o`) or making a channel invite-only (`+i`).

8. **Authentication:**
   - IRC servers may require users to authenticate using a password or other mechanisms.

9. **Server Responses:**
   - Servers respond to client commands with numeric codes indicating success, failure, or other status information.
   - For example, `001` indicates the successful connection of a client.

10. **QUIT Command:**
    - Allows a user to disconnect from the server.

Here's a simple example of an IRC message:
```
:username!user@hostname PRIVMSG #channel :Hello, everyone!
```
- `username` is the sender's nickname.
- `user@hostname` is the sender's username and hostname.
- `PRIVMSG` is the command indicating a message.
- `#channel` is the target channel.
- `Hello, everyone!` is the actual message.

IRC is a flexible protocol, and while the core commands and concepts remain consistent, specific implementations may introduce additional features or modifications. It's essential to refer to the relevant RFCs and documentation for a complete and accurate understanding of the IRC protocol.

# Socket

A socket is a fundamental software abstraction that allows programs on different devices to communicate over a network. It provides a standard interface for networking functions, enabling processes (applications or services) on different devices to establish communication, send and receive data.

In the context of networking, a socket is defined as an endpoint for sending or receiving data across a computer network. It's like a communication endpoint that is identified by an IP address and a port number. The combination of an IP address and a port number uniquely identifies a specific process on a specific device in a network.

Sockets are typically used in network programming to enable communication between different processes, either on the same device or on different devices across a network. The two most common types of sockets are:

1. **Stream Sockets (TCP):**
   - Stream sockets provide a reliable, connection-oriented communication channel.
   - Communication using stream sockets is based on the Transmission Control Protocol (TCP).
   - They ensure that data is delivered in the correct order and without errors.

2. **Datagram Sockets (UDP):**
   - Datagram sockets provide a connectionless, unreliable communication channel.
   - Communication using datagram sockets is based on the User Datagram Protocol (UDP).
   - They do not guarantee the order or reliability of data delivery but are often used when low overhead is crucial.

In programming, sockets are typically implemented as part of a network API (Application Programming Interface) provided by the operating system. Common socket-related functions are `socket()`, `bind()`, `listen()`, `accept()` (for servers), and `connect()` (for clients).

Here's a simple example of creating a socket in C++:

```cpp
#include <iostream>
#include <sys/socket.h>

int main() {
    // Create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket == -1) {
        std::cerr << "Error creating socket\n";
        return 1;
    }

    // Close the socket when done
    close(serverSocket);

    return 0;
}
```

In this example, `socket()` is used to create a socket, and `close()` is used to close the socket when it's no longer needed. The actual usage of sockets involves additional steps like binding to a specific address and port, listening for incoming connections, and handling data transmission.

# Allowed functions (except poll())

1. **setsockopt()**:
   - **Usage**: It is commonly used to set socket options, such as configuring socket behavior or enabling/disabling specific features. For example, setting socket timeout, reusing addresses, or enabling/disabling Nagle's algorithm for TCP.

2. **getsockname()**:
   - **Usage**: After a socket is bound to a local address and port using `bind()`, `getsockname()` can be used to retrieve the local address information. This is useful for servers to determine which address and port they are actually bound to.

3. **getprotobyname()**:
   - **Usage**: It is used to retrieve protocol information based on the protocol name. For example, obtaining the protocol number associated with "tcp" or "udp."

4. **gethostbyname()**:
   - **Usage**: Used to convert a human-readable hostname (like "www.example.com") to an IP address. It's somewhat deprecated in favor of `getaddrinfo()`, which provides more flexibility and supports IPv6.

5. **getaddrinfo()**:
   - **Usage**: It is a versatile function for resolving hostnames and service names. It returns a list of suitable addresses that can be used for creating sockets. This is particularly useful in modern network programming, supporting both IPv4 and IPv6.

6. **freeaddrinfo()**:
   - **Usage**: Essential for releasing the memory allocated by `getaddrinfo()`. This function should be called after using the address information obtained from `getaddrinfo()`.

7. **bind()**:
   - **Usage**: Used on the server side to associate a socket with a specific local address and port before calling `listen()`. It ensures that the server listens on the desired network interface and port.

8. **connect()**:
   - **Usage**: Used on the client side to initiate a connection to a server. The client specifies the remote address and port it wants to connect to using this function.

9. **listen()**:
   - **Usage**: Marks a socket as passive, indicating that it is ready to accept incoming connections. This function is typically called after `bind()` on the server side.

10. **accept()**:
    - **Usage**: Used on the server side to accept an incoming connection on a listening socket. It creates a new socket specifically for the accepted connection, allowing the server to communicate with the client.

11. **htons()** and **htonl()**:
    - **Usage**: Used to convert values between host byte order and network byte order. This is crucial for portability in network programming, where different architectures may have different byte orders.

12. **inet_addr()** and **inet_ntoa()**:
    - **Usage**: Convert between string representations of IP addresses and binary forms. `inet_addr()` converts a string IP address to its binary form, while `inet_ntoa()` converts the binary form back to a string.

13. **send()** and **recv()**:
    - **Usage**: Used for sending and receiving data on a connected socket. These functions are fundamental for communication between processes over a network.

14. **signal()** and **sigaction()**:
    - **Usage**: Used for handling signals in a program. `signal()` is a simpler, older way to handle signals, while `sigaction()` provides more control and is recommended for modern signal handling.

15. **lseek()** and **fstat()**:
    - **Usage**: `lseek()` is used to reposition the file offset within a file, and `fstat()` is used to retrieve information about a file (such as its size, permissions, etc.).

16. **fcntl()**:
    - **Usage**: Performs various control operations on file descriptors. It can be used for tasks like setting non-blocking mode, duplicating file descriptors, or changing file descriptor flags.

These functions collectively enable the development of networked applications and system-level programming in C and C++. They provide the necessary tools to manage sockets, handle signals, and perform file operations.

# Poll() & struct pollfd

Ah, I see. The `struct pollfd` is indeed related to the `poll()` function, which is another mechanism for handling multiple file descriptors in C and C++.

Here's some information about `struct pollfd` and the `poll()` function:

1. **`struct pollfd`:**
   ```cpp
   struct pollfd {
       int   fd;         // file descriptor
       short events;     // events to look for
       short revents;    // events that occurred
   };
   ```
   - `fd`: The file descriptor to be monitored.
   - `events`: The events to monitor for, specified as a bitmask.
   - `revents`: The events that actually occurred, as an output parameter after the call to `poll()`.

2. **`poll()` Function:**
   ```cpp
   int poll(struct pollfd fds[], nfds_t nfds, int timeout);
   ```
   - This function waits for one of a set of file descriptors to become ready to perform I/O operations.
   - `fds`: An array of `struct pollfd` structures, each describing a file descriptor to be monitored.
   - `nfds`: The number of elements in the `fds` array.
   - `timeout`: The maximum time, in milliseconds, that `poll()` should block. A timeout of -1 means to block indefinitely until an event occurs.

Usage example:

```cpp
#include <poll.h>

// Example usage of poll
struct pollfd my_fds[1];
my_fds[0].fd = your_file_descriptor;
my_fds[0].events = POLLIN;  // Monitor for input data

int result = poll(my_fds, 1, 1000);  // Wait for 1 second

if (result > 0) {
    // Check my_fds[0].revents to see which events occurred
    if (my_fds[0].revents & POLLIN) {
        // Input data is ready to be read
    }
} else if (result == 0) {
    // Timeout occurred
} else {
    // An error occurred
}
```

In this example, `poll()` is used to monitor a single file descriptor (`your_file_descriptor`) for input data (`POLLIN`). The function will block for up to 1 second (`1000` milliseconds) or until the specified events occur. After the function returns, you can check the `revents` field in the `struct pollfd` to determine which events occurred.

# Poll() events

The `pollfd` structure is used in conjunction with the `poll()` function to monitor specific events on file descriptors. The events are specified using constants that are defined in the `<poll.h>` header. Here are some commonly used events:

1. **POLLIN:**
   - This event indicates that data can be read without blocking. It is typically used to monitor whether there is incoming data on a file descriptor.

2. **POLLOUT:**
   - This event indicates that data can be written without blocking. It is often used to check whether a file descriptor is ready for writing.

3. **POLLERR:**
   - This event indicates that an error occurred on the file descriptor. It can be used to check for exceptional conditions.

4. **POLLHUP:**
   - This event indicates that the other end of the connection has closed. It is often used to detect hang-up conditions.

5. **POLLNVAL:**
   - This event indicates that the file descriptor is not open. It can be used to check for an invalid file descriptor.

6. **POLLPRI:**
   - This event is typically used with STREAMS-based files and indicates that there is urgent data to be read.

These events are specified using the `events` member of the `struct pollfd` structure when setting up the monitoring. For example, to monitor for read events, you would set `my_fds[0].events = POLLIN;`.

Here's a brief overview of how you might use these events in the context of the `poll()` function:

```cpp
#include <poll.h>

struct pollfd my_fds[1];
my_fds[0].fd = your_file_descriptor;
my_fds[0].events = POLLIN | POLLOUT;  // Monitor for both read and write events

int result = poll(my_fds, 1, 1000);  // Wait for 1 second

if (result > 0) {
    if (my_fds[0].revents & POLLIN) {
        // Input data is ready to be read
    }

    if (my_fds[0].revents & POLLOUT) {
        // File descriptor is ready for writing
    }

    // Check for other events (POLLERR, POLLHUP, etc.)
} else if (result == 0) {
    // Timeout occurred
} else {
    // An error occurred
}
```

In this example, `POLLIN` and `POLLOUT` are used to monitor both read and write events. After the `poll()` call, you can check the `revents` field to determine which events occurred.

# Poll(), sockets and IRC

In the context of an IRC server, file descriptors, the `poll()` system call, and the overall functionality of the server are closely related. Let's explore how these elements are connected:

1. **File Descriptors (FDs):**
   - In Unix-like operating systems, everything is a file, and file descriptors are used to represent open files, sockets, pipes, etc.
   - Sockets, which are the communication endpoints for network communication, are associated with file descriptors.
   - In the context of an IRC server, file descriptors are crucial for managing connections with clients. Each connected client is associated with a socket, represented by a file descriptor.

2. **`poll()` System Call:**
   - `poll()` is a system call used for monitoring multiple file descriptors to see if they are ready for I/O operations (read, write, etc.) without blocking the program's execution.
   - It allows a program to efficiently wait for events on multiple file descriptors and then take appropriate action based on the readiness of those descriptors.
   - For an IRC server, `poll()` is particularly useful for handling multiple client connections concurrently without the need for multiple threads or processes.

3. **IRC Server Functionality:**
   - An IRC server needs to handle various I/O operations, including listening for incoming connections, reading data from clients, and writing data to clients.
   - The server needs to be able to respond to multiple clients simultaneously without blocking on any single client's I/O operation.

4. **Connection Handling:**
   - When a new client connects to the IRC server, a new socket is created, and this socket is associated with a file descriptor.
   - The server needs to be able to accept new connections (using the `accept()` system call) and add the new file descriptors to the set of descriptors monitored by `poll()`.

5. **Event-Driven Architecture:**
   - `poll()` enables an event-driven architecture, where the server responds to events (e.g., data available for reading or space available for writing) as they occur.
   - When `poll()` indicates that a file descriptor is ready, the server can perform the corresponding I/O operation without blocking.

Here's a simplified example of how file descriptors and `poll()` might be used in the context of an IRC server:

```cpp
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // Set up serverSocket (bind, listen, etc.)

    struct pollfd fds[1];
    fds[0].fd = serverSocket;
    fds[0].events = POLLIN; // Poll for incoming data

    while (true) {
        int pollResult = poll(fds, 1, -1); // -1 means wait indefinitely

        if (pollResult == -1) {
            perror("poll");
            break;
        }

        if (fds[0].revents & POLLIN) {
            // Accept new connection
            int clientSocket = accept(serverSocket, nullptr, nullptr);

            // Add clientSocket to the set of monitored file descriptors
            // Handle other operations (read, write) as needed
        }
    }

    close(serverSocket);
    return 0;
}
```

In this example, the `poll()` call is used to monitor the server's listening socket (`serverSocket`). When `poll()` indicates that there is incoming data (`POLLIN` event), the server can accept a new connection using `accept()` and then proceed to handle other I/O operations on the newly created client socket.

This kind of design allows an IRC server to efficiently handle multiple clients concurrently without the need for threading or asynchronous I/O libraries, making it suitable for a simple and scalable architecture. However, implementing a fully functional IRC server involves additional complexities, such as managing multiple clients, handling IRC protocol specifics, and supporting various commands.

Yes, once a client socket is accepted, it typically persists for the entire duration of the client's connection to the server. The server maintains a separate socket for each connected client, and this socket is used to exchange data between the server and the client.

Here's a basic outline of how the lifecycle of a client socket works in the context of a server:

1. **Connection Establishment:**
   - When a client connects to the server, the server uses the `accept()` system call to accept the incoming connection.
   - The `accept()` call returns a new socket descriptor (let's call it `clientSocket`) that represents the specific connection to that client.

2. **Data Exchange:**
   - The server and client can now exchange data using the `clientSocket`.
   - The server can use `send()` or `write()` to send data to the client, and `recv()` or `read()` to receive data from the client.
   - Similarly, the client can use its own socket to send and receive data.

3. **Connection Termination:**
   - When the client decides to disconnect or the server determines that the connection is no longer needed, the server can use the `close()` system call to close the `clientSocket`.
   - Closing the socket indicates the end of the connection between the server and the specific client.

4. **Handling Multiple Clients:**
   - For an IRC server or any server that needs to handle multiple clients simultaneously, the server would typically manage a collection of client sockets, possibly using an array, a data structure like a linked list, or another appropriate container.
   - The server would use `poll()` or another mechanism to determine which sockets are ready for I/O operations and then handle data exchange accordingly.

# More on bind()

Binding a socket to an address and port is necessary for two main reasons:

To establish a specific point for receiving data: When you bind a socket to a specific IP address and port number, you're telling the operating system that any data sent to that IP and port should be delivered to this particular application. Without binding, the operating system wouldn't know where to deliver incoming data packets.

To establish a known point for sending data: In the case of a server, clients need to know where to send their requests. By binding a socket to a specific port, you're establishing a known location where clients can send their data.

In summary, binding a socket to an address and port is a crucial step in setting up network communication, as it establishes the endpoint for data transmission and reception.

The bind() function is not typically used for client sockets because it's not usually necessary for a client to bind to a specific port. When a client creates a socket and then makes a connect() call, the operating system will automatically select an available port to use for that socket. This is often referred to as an ephemeral port.

No, not all sockets created through poll() are bound to the server address and port created with bind().

In your code, poll() is used to monitor one or more file descriptors (in this case, sockets) for certain events, such as incoming connections. When poll() indicates that a new connection is ready to be accepted on the server socket, you call accept() to accept the new connection.

The accept() function creates a new socket for each incoming connection. This new socket is not bound to a specific address and port with bind(). Instead, it's automatically associated with the client's address and port, and it's used for communication with that specific client.

So, in summary, the server socket is bound to a specific address and port with bind(), but the client sockets created by accept() are not.

No, it's not against the IRC protocol to use ephemeral ports for established client connections.

In fact, it's standard practice for client connections to use ephemeral ports. When a client connects to an IRC server (or any server), the client's operating system typically assigns an ephemeral port for the client's side of the connection. This is true regardless of the port that the server is listening on (in this case, 6667 for an IRC server).

The server's listening port (6667 for an IRC server) is what's known and consistent. This is the port that clients know to connect to. The client's source port, on the other hand, can be any available ephemeral port assigned by the client's operating system.

So, in the context of an IRC server, the server would be bound to a specific port (6667), but the client connections accepted by the server would be associated with whatever ephemeral port the client's operating system assigned for that connection.

# Struct sockaddr & sockaddr_in

The struct sockaddr is a general-purpose data structure used in the socket API for specifying network addresses. It's used in various socket functions, including bind(), connect(), accept(), and others.

Here's what it looks like:

```cpp
struct sockaddr {
	unsigned short sa_family;   // address family, AF_xxx
	char           sa_data[14]; // 14 bytes of protocol address
};
```

sa_family is the address family (also known as the domain) that the socket will use. This could be AF_INET for IPv4, AF_INET6 for IPv6, AF_UNIX for local socket communication, and so on.

sa_data contains the actual protocol-specific address information. For AF_INET, this would include the IP address and port number.

However, because sa_data is not easy to work with directly, we usually use a protocol-specific version of sockaddr, such as sockaddr_in for IPv4:

```cpp
struct sockaddr_in {
    short            sin_family;   // e.g. AF_INET
    unsigned short   sin_port;     // e.g. htons(3490)
    struct in_addr   sin_addr;     // see struct in_addr, below
    char             sin_zero[8];  // zero this if you want to
};

struct in_addr {
    unsigned long s_addr;  // load with inet_aton()
};
```

In this structure, sin_family is the address family (should be AF_INET for IPv4), sin_port is the port number, and sin_addr is the IP address. The sin_zero field is just padding to make the structure the same size as struct sockaddr.

# Non-blocking mode

In C++ socket programming, the term "non-blocking mode" refers to the ability of a socket to operate without blocking the execution of the program while waiting for an operation to complete. By default, sockets in C++ operate in blocking mode, meaning that certain socket operations, such as reading or writing data, will block the execution of the program until the operation is completed.

In non-blocking mode, socket operations are designed to return immediately, whether they succeed or not, without waiting for the operation to finish. This allows the program to continue executing other tasks while waiting for the socket operation to complete. Non-blocking sockets are useful in situations where you want to perform multiple operations concurrently or where you don't want a single slow operation to block the entire program.

To set a socket to non-blocking mode in C++, you typically use platform-specific functions or libraries. For example, on Unix-like systems, you might use the `fcntl` function with the `O_NONBLOCK` flag, and on Windows, you might use the `ioctlsocket` function with the `FIONBIO` flag.

Here's a simple example in C++ on Unix-like systems using `fcntl` to set a socket to non-blocking mode:

```cpp
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>

int main() {
    // Assume sockfd is a valid socket descriptor

    // Set the socket to non-blocking mode
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "Error getting socket flags" << std::endl;
        return 1;
    }

    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        std::cerr << "Error setting socket to non-blocking mode" << std::endl;
        return 1;
    }

    // Now sockfd is in non-blocking mode

    // Perform socket operations without blocking the program
    // ...

    return 0;
}
```

Remember that when using non-blocking sockets, you need to handle the fact that operations may not complete immediately. You may need to use techniques such as polling or asynchronous I/O to check when a socket is ready for reading or writing.

# Reading from client socket

To read, store, and process data from a client without blocking the process, you can use non-blocking I/O operations. This is typically achieved using the `O_NONBLOCK` flag for file descriptors or by setting sockets to non-blocking mode. Additionally, you can use functions like `select()`, `poll()`, or `epoll()` to determine which sockets are ready for I/O operations without blocking.

Here's a simplified example using the `fcntl()` function to set the client socket to non-blocking mode and `poll()` to wait for data:

```cpp
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

const int MAX_BUFFER_SIZE = 1024;

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // Set up serverSocket (bind, listen, etc.)

    struct pollfd fds[1];
    fds[0].fd = serverSocket;
    fds[0].events = POLLIN;

    while (true) {
        int pollResult = poll(fds, 1, -1);

        if (pollResult == -1) {
            perror("poll");
            break;
        }

        if (fds[0].revents & POLLIN) {
            int clientSocket = accept(serverSocket, nullptr, nullptr);

            // Set clientSocket to non-blocking mode
            fcntl(clientSocket, F_SETFL, O_NONBLOCK);

            // Continue handling other I/O operations for this clientSocket
            char buffer[MAX_BUFFER_SIZE];
            ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

            if (bytesRead == -1) {
                // Handle error or check errno for EWOULDBLOCK/EAGAIN
            } else if (bytesRead == 0) {
                // Connection closed by client
                close(clientSocket);
            } else {
                // Process the received data
                // ...
            }
        }
    }

    close(serverSocket);
    return 0;
}
```

In this example:

1. The `fcntl()` function is used to set the `clientSocket` to non-blocking mode by adding the `O_NONBLOCK` flag.

2. The `poll()` function is used to wait for incoming connections. When a connection is detected (`POLLIN` event), the server accepts the connection, sets the client socket to non-blocking mode, and then proceeds to handle other I/O operations.

3. The `recv()` function is used to read data from the client socket. If no data is available, `recv()` will return -1, and you can check `errno` for `EWOULDBLOCK` or `EAGAIN` to handle the non-blocking situation without blocking the process.

This allows your server to efficiently handle multiple clients concurrently, reading data without blocking the entire process. Keep in mind that you may need additional error handling and logic based on your specific application requirements.

# Client handling

```
1. Accept the new connection, get the clientSocket
2. Read the initial messages from the client
3. If the messages are valid:
   4. Send the welcome messages
   5. While the client is connected:
      6. Read a command from the client
      7. Process the command
      8. Send the response to the client
```

# Netcat (nc)

The `nc` command, which stands for "netcat," is a versatile networking utility used for reading from and writing to network connections using TCP or UDP protocols. It can be used to create connections to different network services, transfer files, or perform other networking tasks.

Here is a basic syntax for the `nc` command:

```bash
nc [options] host port
```

- `host`: The target host or IP address.
- `port`: The port number on the target host.

Some common options include:

- `-l`: Used to set up a listening mode for incoming connections.
- `-p`: Specifies the source port to use.
- `-u`: Use UDP instead of the default TCP.
- `-v`: Be verbose, showing more information about the connection.

Here are a few examples:

1. **Open a TCP connection to a specific port on a host:**
    ```bash
    nc example.com 80
    ```

2. **Listen on a specific port:**
    ```bash
    nc -l 1234
    ```

3. **Transfer a file using `nc`:**
    On the receiving side:
    ```bash
    nc -l -p 1234 > received_file
    ```
    On the sending side:
    ```bash
    nc other_host 1234 < file_to_send
    ```

Please note that the `nc` command and its capabilities may vary slightly between different operating systems. It's a powerful tool, and caution should be exercised, especially when using it on public networks or the internet. Always refer to the manual (`man nc` on Unix-like systems) for detailed information and options available on your specific system.