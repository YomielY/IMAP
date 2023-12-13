/*
Time taken: 
 - Generally it takes me 2 days for finishing all the requirements. 
 - It took me one day for understanding the code provided by the OpenSSL page on how to form connection between client and server, 
   as well as read through the function given by the openSSL library for a fully understanding how can I incorporate them into the 
   realization of receiving email. 
 - The time it took me for writing the code is rather short compared with understanding the material and debugging. 

Difficulties faced: 
 - The most challenging part is to understand each OpenSSL API provided by its library and figuring out where can they be used properly. 
 - The installation of OpenSSL took me some efforts at first for its complex configuration process for windows OS. 
 - For reading the information from server side, it was challenging to think of the proper point to break the loop and proceed onto the next step.
 - The authentication process caused a little terbulence for my implementation. 

Extention interested: 
 - The security level of this simple mail reading API can indeed be improved by using certificate verification. 
 - Two step authentication is a convenience functionality provided by Gmail but may position the security at risk. 
 - The SSL/TSL verification is important as a user as well as a developer of the computer network, it provides safety control over people's privacy.  
 - Configuration of open source library using Linux is way more easier than setting up using windows OS. 
*/



#include <openssl/ssl.h>
#include <openssl/bio.h>

#define HOST_NAME "imap.gmail.com"
#define HOST_PORT "993"
#define INBOX_FOLDER "INBOX"
#define USERNAME "haydenyin100@gmail.com"
#define PASSWORD "ztfi minb xqac vvqg"

void handleFailure() {
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
    char buffer[2160];
    do 
    {
        bytesRead = BIO_read(web, buffer, sizeof(buffer) - 1);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';  
            printf("%s", buffer);

            // If tag A01 identified 
            if (strstr(buffer, tag1) != NULL) {
                break;
            }
        } else if (bytesRead == 0 || bytesRead == -1) {
            printf(" No data was successfully read or written. ");
            break;
        } else {
            printf(" Operation is not implemented in the specific BIO type. ");
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

            // If tag A02 identified
            if (strstr(buffer, tag2) != NULL) {
                break;
            }
        } else if (bytesRead == 0 || bytesRead == -1) {
            printf(" No data was successfully read or written. ");
            break;
        } else {
            printf(" Operation is not implemented in the specific BIO type. ");
            break;
        }
    } while (1);

    /* Fetch the email from mailbox */
    const char *tag3 = "A03";
    const char *fetchCommand = "A03 FETCH 1 BODY.PEEK[]\r\n";
    res = BIO_puts(web, fetchCommand);
    if (!(res > 0)) handleFailure();

    // Read data after FETCH
    do 
    {
        bytesRead = BIO_read(web, buffer, sizeof(buffer) - 1);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';  
            printf("%s", buffer);
            // If tag A03 identified
            if (strstr(buffer, tag3) != NULL) {
                break;
            }
        } else if (bytesRead == 0 || bytesRead == -1) {
            printf(" No data was successfully read or written. ");
            break;
        } else {
            printf(" Operation is not implemented in the specific BIO type. ");
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

    return 0;
}

