# IRC server overview

```
1. Initialize server
   1.1. Create a socket
   1.2. Bind the socket to an IP address and port
   1.3. Set the socket to listen for incoming connections

2. Enter main server loop
   2.1. Use poll() to wait for events
   2.2. If a new connection is ready to be accepted on the server socket:
       2.2.1. Accept the new connection, get the clientSocket
       2.2.2. Add the new clientSocket to the poll() set
   2.3. If a clientSocket is ready to be read:
       2.3.1. Read the client's message
       2.3.2. If the message is a valid IRC command:
           2.3.2.1. Process the command
           2.3.2.2. Send the response to the client
       2.3.3. If the client has disconnected:
           2.3.3.1. Remove the clientSocket from the poll() set
           2.3.3.2. Close the clientSocket
```
