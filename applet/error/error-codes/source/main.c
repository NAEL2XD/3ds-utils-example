#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_ERROR_CODE 7

char* strings[MAX_ERROR_CODE] = {
    "Error Code randomizer.",
    "Doesn't allow you to use Home Menu.",
    "N3DS Service User Agreement",
    "This system will receive important notices",
    "Important: About System Updates",
    "Displays a network error message.",
    "Displays EULA."
};

void error(int type) {
    errorConf errorCtx;
    errorInit(&errorCtx, ERROR_TEXT, CFG_LANGUAGE_EN);
    errorText(&errorCtx, "An error has occurred");
    
    if (type == 1) errorCtx.errorCode = rand();
    else if (type == 2) errorCtx.homeButton = false;
    else if (type == 3) errorCtx.type = ERROR_TYPE_EULA_DRAW_ONLY;
    else if (type == 4) errorCtx.type = ERROR_TYPE_EULA_FIRST_BOOT;
    else if (type == 5) errorCtx.type = ERROR_TYPE_AGREE;
    else if (type == 6) errorCtx.type = ERROR_CODE_LANGUAGE;
    else if (type == 7) errorCtx.type = ERROR_EULA_LANGUAGE;
    
    errorDisp(&errorCtx);
}

void printText(int code) {
    consoleClear();

    printf("Error Applet testing by Nael2xd\n");
    printf("Press [DLEFT] or [DRIGHT] to scroll.\nPress [START] to exit.\n\n");
    for (int i = 1; i < MAX_ERROR_CODE + 1; i++) printf("%d: %s\n", i, strings[i - 1]);
    printf("\n<< %d >>", code);
}

int main() {
    // Initialize services
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);
    printText(1);

    int code = 1;
    while (aptMainLoop()) {
        bool didit = false;

        hidScanInput();
        u32 kDown = hidKeysDown();

		if (kDown & KEY_START) break;
        else if (kDown & KEY_DLEFT) {
            code--;
            if (code == 0) code = MAX_ERROR_CODE;
            didit = true;
        } else if (kDown & KEY_DRIGHT) {
            code++;
            if (code == MAX_ERROR_CODE + 1) code = 1;
            didit = true;
        } else if (kDown & KEY_A) {
            error(code);
        }

        if (didit) printText(code);
    }

    // Cleanup
    gfxExit();
    return EXIT_SUCCESS;
}