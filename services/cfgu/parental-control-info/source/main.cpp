#include <3ds.h>
#include <stdio.h>
#include <string>

#define CHECK_RESULT(funcName, result) \
    do { \
        printf("%s: %s! (0x%lX)\n", funcName, R_FAILED(result) ? "failed" : "success", result); \
        if (R_FAILED(result)) { \
            goto fail; \
        } \
    } while (false)

struct Parental {
    u32 restrictionBitmask;
    u32 unknown0x4;
    u8  ratingSystem;
    u8  maxAllowedAge;
    u8  secretQuestion;
    u8  unknown0xB;
    u16 pinCode[4];
    u16 secretAnswer[34];
};

int main() {
    // Initialize needed services.
    gfxInitDefault();
    consoleInit(GFX_TOP, nullptr);

    // Print Stuff
    printf("libctru 3DS Test\n");
    printf("Press [START] to exit.\n");

    Parental out;
    std::string strOut;

    CHECK_RESULT("cfguInit", cfguInit());
    CHECK_RESULT("CFG_GetConfigInfoBlk4", CFG_GetConfigInfoBlk4(0xC0, 0x000C0000, &out));
    
    printf("restrictionBitmask: %lu\n", out.restrictionBitmask);
    printf("unknown0x4:         %lu\n", out.unknown0x4);
    printf("ratingSystem:       %u\n",  out.ratingSystem);
    printf("maxAllowedAge:      %u\n",  out.maxAllowedAge);
    printf("secretQuestion:     %u\n",  out.secretQuestion);
    printf("unknown0xB:         %u\n",  out.unknown0xB);
    
    printf("pinCode:            ");
    for (int i = 0; i < 4; i++)
        printf("%02X ", out.pinCode[i]);

    printf("\n\t- Formatted PIN: %u%u%u%u\n",
        out.pinCode[0] & 0xF, (out.pinCode[0] >> 8) & 0xF,
        out.pinCode[1] & 0xF, (out.pinCode[1] >> 8) & 0xF);
    
    printf("\nsecretAnswer:       ");
    for (int i = 0; i < 32; i++) {
        u16 digit = out.secretAnswer[i];
        if (digit == 0) break;
        printf("%02X ", digit);
        strOut += digit;
    }
    printf("\n\t- (Converted: %s)\n", strOut.c_str());
    
    fail:
    while (aptMainLoop()) {
        hidScanInput();
        if (hidKeysDown() & KEY_START) {
            break;
        }
    }

#undef CHECK_RESULT
    cfguExit();
    gfxExit();
    return 0;
}