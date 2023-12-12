// Standard library
#include <stdio.h>
#include <string.h>

// OpenSSL library
#include <openssl/ssl.h>
#include <openssl/bio.h>

#define HOST_NAME "imap.gmail.com"
#define HOST_PORT "993"
#define USERNAME "haydenyin100@gmail.com"
#define PASSWORD "wzwf wfda kwso xryf"


void handleFailure() {
    // Implement your error handling logic
    fprintf(stderr, "Error occurred.\n");
    exit(EXIT_FAILURE);
}

int main() {
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) handleFailure();

    SSL *ssl;
    BIO *bio;

    // Create a TLS connection
    bio = BIO_new_ssl_connect(ctx);
    if (!bio) handleFailure();

    BIO_get_ssl(bio, &ssl);
    if (!ssl) handleFailure();

    // Set up the TLS connection
    BIO_set_conn_hostname(bio, HOST_NAME ":" HOST_PORT);
    if (BIO_do_connect(bio) <= 0) handleFailure();

    // Perform TLS handshake
    if (BIO_do_handshake(bio) <= 0) handleFailure();

    // Log in to the Gmail server
    char loginCommand[256];
    snprintf(loginCommand, sizeof(loginCommand), "A01 LOGIN %s {%lu}\r\n%s\r\n", USERNAME, strlen(PASSWORD), PASSWORD);

    BIO_puts(bio, loginCommand);

    char response[256];
    BIO_read(bio, response, sizeof(response));
    // printf("%s\n", response);
    //    if (strncmp(response, "A01 OK", 6) != 0) {
    //     fprintf(stderr, "Login failed: %s\n", response);
    //     // You may choose to exit here or attempt to continue
    // }

    // Select the folder (INBOX)
    const char *selectCommand = "A02 SELECT INBOX\r\n";
    BIO_puts(bio, selectCommand);

    BIO_read(bio, response, sizeof(response));
    // printf("%s\n", response);

    // if (strncmp(response, "A02 OK", 6) != 0) {
    //     fprintf(stderr, "Folder selection failed: %s\n", response);
    //     // You may choose to exit here or attempt to continue
    // }

    // Fetch the email
    const char *fetchCommand = "A03 FETCH 1 BODY.PEEK[]\r\n";
    BIO_puts(bio, fetchCommand);

    // Read and print the first 10 kiB of the email body
    long bytesRead = 0;
    do {
        char buff[1024];
        int len = BIO_read(bio, buff, sizeof(buff));

        if (len > 0) {
            fwrite(buff, 1, len, stdout);
            bytesRead += len;
        }
    } while (BIO_should_retry(bio) && bytesRead < 10240);

    // Cleanup
    BIO_free_all(bio);
    SSL_CTX_free(ctx);

    return 0;
}
