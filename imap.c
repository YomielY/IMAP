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
    const char *tag1 = "A01";

    int length = strlen(PASSWORD);
    char loginCommand[256];
    sprintf(loginCommand, "%s LOGIN %s {%d}\r\n%s\r\n", tag1, USERNAME, length, PASSWORD);

    res = BIO_puts(web, loginCommand);
    if (!(res > 0)) handleFailure();

    // Read data from the connection
    int bytesRead;
    char buffer[1024];
    do 
    {
        bytesRead = BIO_read(web, buffer, sizeof(buffer) - 1);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';  
            printf("%s", buffer);

            // Check if the received data contains the tag "A01"
            if (strstr(buffer, tag1) != NULL) {
                printf("Connection closed\n");
                break;
            }
        } else if (bytesRead == 0 || bytesRead == -1) {
            printf("Connection closed by the server.\n");
            break;
        } else {
            printf("Error reading data.\n");
            break;
        }
    } while (1);


    /* Select Folder */
    const char *tag2 = "A02";
    const char *selectCommand = "A02 SELECT " INBOX_FOLDER "\r\n";
    res = BIO_puts(web, selectCommand);
    if (!(res > 0)) handleFailure();

    do 
    {
        bytesRead = BIO_read(web, buffer, sizeof(buffer) - 1);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';  
            printf("%s", buffer);

            // Check if the received data contains the tag "A01"
            if (strstr(buffer, tag2) != NULL) {
                printf("Connection closed\n");
                break;
            }
        } else if (bytesRead == 0 || bytesRead == -1) {
            printf("Connection closed by the server.\n");
            break;
        } else {
            printf("Error reading data.\n");
            break;
        }
    } while (1);

    /* Fetch the email from mailbox */
    const char *tag3 = "A03";
    const char *fetchCommand = "A03 FETCH 1 BODY.PEEK[]\r\n";
    res = BIO_puts(web, fetchCommand);
    if (!(res > 0)) handleFailure();
    // Read data from the connection after FETCH
    do 
    {
        bytesRead = BIO_read(web, buffer, sizeof(buffer) - 1);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';  
            printf("%s", buffer);

            // Check if the received data contains the tag "A01"
            if (strstr(buffer, tag3) != NULL) {
                printf("Connection closed\n");
                break;
            }
        } else if (bytesRead == 0 || bytesRead == -1) {
            printf("Connection closed by the server.\n");
            break;
        } else {
            printf("Error reading data.\n");
            break;
        }
    } while (1);

    /* free memory */
    if (web != NULL) {
        BIO_free_all(web);
    }

    if (NULL != ctx) {
        SSL_CTX_free(ctx);
    }
}

