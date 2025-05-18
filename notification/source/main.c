#include <3ds.h>
#include <string.h>
#include <stdio.h>

int main() {
    // Initialize services
    gfxInitDefault();
    newsInit();
    consoleInit(GFX_TOP, NULL);

    printf("Notification (News) testing by Nael2xd\n");
    printf("Press [START] to exit.\nAlso check your notifications!");

    // Convert strings to UTF-16
    u16 title[32] = {0};
    u16 message[256] = {0};
    
    // Convert ASCII to UTF-16
    const char* ascii_title = "Notification Test";
    const char* ascii_message = "Hey y'all, scott here!\nI mean, Nael2xd was here!!! Heheheh... sorry.\n\nAlso thanks for checking out my gist :D";
    
    for (size_t i = 0; i < strlen(ascii_title); i++)
        title[i] = ascii_title[i];
    
    for (size_t i = 0; i < strlen(ascii_message); i++)
        message[i] = ascii_message[i];

    // Create notification
    NEWS_AddNotification(
        title, strlen(ascii_title),   // UTF-16 title
        message, strlen(ascii_message), // UTF-16 message
        NULL, 0,                      // No image
        false                         // No JPEG flag
    );

    while (aptMainLoop()) {
        hidScanInput();
		if (hidKeysDown() & KEY_START) break;
    }

    // Cleanup
    newsExit();
    gfxExit();
    return 0;
}