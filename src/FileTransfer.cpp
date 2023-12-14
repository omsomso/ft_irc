#include "../inc/Irc.hpp"

// FILE TRANSFER STUFF
// USE /DCC SEND <path/to/file> 127.0.0.1 1096 <filesize> <recipient_nick> to SEND
// USE /DCC ACCEPT <path/to/file> 127.0.0.1 1096 <filesize> <sender_nick> to RECEIVE
void* fileSendThread(void* arg) {
    FileTransferData* data = static_cast<FileTransferData*>(arg);
    std::cout << "Sender Thread Started, waiting for receiver to connect..." << std::endl;

    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        delete data;
        return NULL;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(data->port);

    if (bind(serverSock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        close(serverSock);
        delete data;
        return NULL;
    }

    if (listen(serverSock, 1) < 0) {
        std::cerr << "Listen failed" << std::endl;
        close(serverSock);
        delete data;
        return NULL;
    }

    int clientSock = accept(serverSock, NULL, NULL);
    if (clientSock < 0) {
        std::cerr << "Accept failed" << std::endl;
        close(serverSock);
        delete data;
        return NULL;
    }
    std::cout << "Connection accepted from receiver" << std::endl;

    std::ifstream fileStream(data->filename.c_str(), std::ifstream::binary);
    if (!fileStream) {
        std::cerr << "File open failed: " << data->filename << std::endl;
        close(clientSock);
        close(serverSock);
        delete data;
        return NULL;
    }

    char buffer[1024];
    while (fileStream.read(buffer, sizeof(buffer)) || fileStream.gcount()) {
        ssize_t sentBytes = send(clientSock, buffer, fileStream.gcount(), 0);
        if (sentBytes < 0) {
            std::cerr << "Error sending data" << std::endl;
            break;
        }
        std::cout << "Sent " << sentBytes << " bytes" << std::endl;
    }

    fileStream.close();
    close(clientSock);
    close(serverSock);
    std::cout << "Sender Thread Ended" << std::endl;
    delete data;
    return NULL;
}

void* fileReceiveThread(void* arg) {
    FileTransferData* data = static_cast<FileTransferData*>(arg);
    std::string newFilename = "received_" + data->filename;
    std::cout << "Receiver Thread Started, connecting to sender..." << std::endl;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        delete data;
        return NULL;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(data->port);
    inet_pton(AF_INET, data->peerIP.c_str(), &addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        close(sock);
        delete data;
        return NULL;
    }
    std::cout << "Connected to sender" << std::endl;

    // Extract filename from the full path
    std::string fullPath(data->filename);
    size_t lastSlashPos = fullPath.find_last_of("/\\");
    std::string filename = fullPath.substr(lastSlashPos + 1);

    // Create new file path
    std::string newFilePath = "received_files/" + filename;

    // Ensure the 'received_files' directory exists
    system("mkdir -p received_files");

    std::ofstream fileStream(newFilePath.c_str(), std::ofstream::binary);
    if (!fileStream) {
        std::cerr << "File open failed: " << newFilePath << std::endl;
        return NULL;
    }

    char buffer[1024];
    int bytesRead;
    while ((bytesRead = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
        fileStream.write(buffer, bytesRead);
        std::cout << "Received " << bytesRead << " bytes" << std::endl;
    }

    fileStream.close();
    close(sock);
    std::cout << "Receiver Thread Ended" << std::endl;

    delete data;
    return NULL;
}

void Irc::IrcCommands::handleDCC(const std::string& input, Client& client) {
    std::vector<std::string> dcc_tokens = _irc.tokenizeInput(input, ' ');

    if (dcc_tokens.size() < 5 || dcc_tokens[0] != "DCC" || (dcc_tokens[1] != "SEND" && dcc_tokens[1] != "ACCEPT")) {
        client.sendToClient("Unknown or incomplete DCC command.\n");
        return;
    }

    // Handle DCC SEND
    if (dcc_tokens[1] == "SEND") {
        FileTransferData* data = new FileTransferData();
        data->filename = dcc_tokens[2];
        data->peerIP = dcc_tokens[3];
        data->port = atoi(dcc_tokens[4].c_str());
        data->fileSize = dcc_tokens.size() > 5 ? atoll(dcc_tokens[5].c_str()) : 0;

        pthread_t threadId;
        pthread_create(&threadId, NULL, fileSendThread, data);
        pthread_detach(threadId);
		
		// Notify the recipient
        std::string recipient = dcc_tokens.size() > 6 ? dcc_tokens[6] : "";
        if (!_irc.clientExists(recipient)) {
            client.sendToClient("Recipient not found.");
            return;
        }

        Client& recipientClient = *_irc._clientsByNicks[recipient];
        std::string notification = "Client " + client.getNickName() + " wants to send you " + data->filename + " on " + data->peerIP + " port " + std::to_string(data->port) + "\n";
        recipientClient.sendToClient(notification);

        std::string response = "DCC SEND request sent to " + recipient + "\n";
        client.sendToClient(response);
    }
    // DCC ACCEPT
	if (dcc_tokens[1] == "ACCEPT") {
        FileTransferData* data = new FileTransferData();
        data->filename = dcc_tokens[2];
        data->port = atoi(dcc_tokens[4].c_str());

        pthread_t threadId;
        pthread_create(&threadId, NULL, fileReceiveThread, data);
        pthread_detach(threadId);

        std::string response = "DCC ACCEPT recognized: Receiving file " + data->filename + " on port " + std::to_string(data->port) + "\n";
        client.sendToClient(response);
	}
}
