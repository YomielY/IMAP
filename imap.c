#include <openssl/ssl.h>
#include <openssl/bio.h>

#define HOST_NAME "imap.gmail.com"
#define HOST_PORT "993"
#define INBOX_FOLDER "INBOX"
#define USERNAME "haydenyin100@gmail.com"
#define PASSWORD "ztfi minb xqac vvqg"

void handleFailure() {
    // Implement your error handling logic
    fprintf(stderr, "Error occurred.\n");
    exit(EXIT_FAILURE);
}

int main() {
    long res = 1;
    SSL_CTX* ctx = NULL;
    BIO *web = NULL;
    SSL *ssl = NULL;

    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    const SSL_METHOD* method = SSLv23_method();
    if (!(NULL != method)) handleFailure();

    ctx = SSL_CTX_new(method);
    if (!(ctx != NULL)) handleFailure();

    ssl = SSL_new(ctx);
    if (!(ssl != NULL)) handleFailure();

    web = BIO_new_ssl_connect(ctx);
    if (!(web != NULL)) handleFailure();

    res = BIO_set_conn_hostname(web, HOST_NAME ":" HOST_PORT);
    if (!(1==res)) handleFailure();

    BIO_get_ssl(web, &ssl);
    if (!(ssl != NULL)) handleFailure(); 

    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

    res = BIO_do_connect(web);
    if (!(1==res)) handleFailure();

    res = BIO_do_handshake(web);
    if (!(1==res)) handleFailure();

    /* hostname verification */
    const char *tag = "A01";
    int length = strlen(PASSWORD);
    char loginCommand[256];
    // Use sprintf to format the string
    sprintf(loginCommand, "%s LOGIN %s {%d}\r\n%s\r\n", tag, USERNAME, length, PASSWORD);

    res = BIO_puts(web, loginCommand);
    if (!(res > 0)) handleFailure();

    // Read data from the connection
    char buffer[1024];
    int bytesRead = BIO_read(web, buffer, sizeof(buffer) - 1);
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';  // Null-terminate the buffer
        printf("Received data:\n%s\n", buffer);
    } else if (bytesRead == 0 || bytesRead == -1) {
        printf("Connection closed by the server.\n");
    } else {
        printf("Error reading data.\n");
    }

    /* Select Folder */
    const char *selectCommand = "A02 SELECT " INBOX_FOLDER "\r\n";
    res = BIO_puts(web, selectCommand);
    if (!(res > 0)) handleFailure();

    // Read data from the connection after SELECT
    bytesRead = BIO_read(web, buffer, sizeof(buffer) - 1);

    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        printf("Received data after SELECT:\n%s\n", buffer);
    } else {
        printf("Error reading data after SELECT.\n");
    }

    /* Fetch the email from mailbox */
    const char *fetchCommand = "A03 FETCH 1 BODY.PEEK[]\r\n";

    res = BIO_puts(web, fetchCommand);
    if (!(res > 0)) handleFailure();

    // Read data from the connection after FETCH
    bytesRead = BIO_read(web, buffer, sizeof(buffer) - 1);

    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        printf("Received data after FETCH:\n%s\n", buffer);
    } else {
        printf("Error reading data after FETCH.\n");
    }

    /* free memory */
    if (web != NULL) {
        BIO_free_all(web);
    }

    if (NULL != ctx) {
        SSL_CTX_free(ctx);
    }
}

