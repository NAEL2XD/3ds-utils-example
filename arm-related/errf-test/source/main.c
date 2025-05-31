/**
 * Yeah so i am dumb and had to use ai for this, sorry my bad!!
 * 
 * But hey, at least it works!
 */

#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>

// Function to generate a prefetch abort exception
__attribute__((noreturn)) void trigger_prefetch_abort() {
    // This will cause a prefetch abort by jumping to an invalid address
    void (*invalid_function)() = (void (*)())0xDEADBEEF;
    invalid_function();
    __builtin_unreachable();
}

// Function to generate a data abort exception
void trigger_data_abort() {
    volatile u32 *invalid_ptr = (volatile u32*)0xDEADC0DE;
    *invalid_ptr = 0x12345678;  // Write to invalid memory
}

void ptfGfb(const char* txt) {
    printf("%s\n", txt);
    gfxFlushBuffers();
}

int main() {
    // Initialize services
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);
    errfInit();  // Initialize error framework
    
    ptfGfb("ERRF Demo - libctru");
    ptfGfb("A: Generic Fatal Error");
    ptfGfb("B: Custom Error Message");
    ptfGfb("X: Simulate Data Abort");
    ptfGfb("Y: Log Only (No Reboot)");
    ptfGfb("L: NAND Damaged Error");
    ptfGfb("R: Card Removed Error");
    ptfGfb("ZL: Prefetch Abort Exception");
    ptfGfb("ZR: Undefined Instruction");
    ptfGfb("START: Exit\n");
    ptfGfb("Note: Most errors will reboot the system!");

    while (aptMainLoop()) {
        gspWaitForVBlank();
        hidScanInput();
        u32 kDown = hidKeysDown();

        if (kDown & KEY_START) break;

        if (kDown & KEY_A) {
            // Create a custom result code
            Result myResult = MAKERESULT(RL_PERMANENT, RS_INVALIDARG, RM_APPLICATION, 1);
            
            // Throw a generic fatal error
            ptfGfb("Throwing generic fatal error...");
            ERRF_ThrowResult(myResult);
            
            // This line will only execute if throw fails
            printf("Throw failed! 0x%08lX\n", myResult);
        }

        if (kDown & KEY_B) {
            // Set custom user string (appears in some error reports)
            ERRF_SetUserString("ERRF Demo Custom String");
            
            // Throw error with custom message

            ptfGfb("Throwing custom message error...");
            ERRF_ThrowResultWithMessage(MAKERESULT(RL_FATAL, RS_INTERNAL, RM_APPLICATION, 2), "Custom error message from libctru demo!");
        }

        if (kDown & KEY_X) {
            // Simulate a CPU exception (data abort)
            ptfGfb("Simulating CPU data abort...");
            trigger_data_abort();
        }

        if (kDown & KEY_Y) {
            // Log an error without causing reboot
            ptfGfb("Logging non-fatal error...");
            Result rc = ERRF_LogResult(MAKERESULT(RL_USAGE, RS_NOTSUPPORTED, RM_APPLICATION, 3));
            
            if (R_SUCCEEDED(rc)) {
                ptfGfb("Error logged successfully!");
            } else {
                printf("Logging failed! 0x%08lX\n", rc);
            }
        }

        if (kDown & KEY_L) {
            // Simulate NAND damaged error
            ptfGfb("Throwing NAND damaged error...");
            
            ERRF_FatalErrInfo errorInfo = {
                .type = ERRF_ERRTYPE_NAND_DAMAGED,
                .revHigh = 0,
                .revLow = 0,
                .resCode = MAKERESULT(RL_STATUS, RS_INVALIDSTATE, RM_FS, 4),
                .pcAddr = (u32)__builtin_return_address(0),
                .procId = 0,
                .titleId = 0,
                .appTitleId = 0,
                .data.failure_mesg = ""  // Not used for this type
            };
            ERRF_Throw(&errorInfo);
        }

        if (kDown & KEY_R) {
            // Simulate card removed error
            ptfGfb("Throwing card removed error...");
            
            ERRF_FatalErrInfo errorInfo = {
                .type = ERRF_ERRTYPE_CARD_REMOVED,
                .revHigh = 0,
                .revLow = 0,
                .resCode = MAKERESULT(RL_STATUS, RS_NOTFOUND, RM_FS, 5),
                .pcAddr = (u32)__builtin_return_address(0),
                .procId = 0,
                .titleId = 0,
                .appTitleId = 0,
                .data.failure_mesg = ""  // Not used for this type
            };
            ERRF_Throw(&errorInfo);
        }

        if (kDown & KEY_ZL) {
            // Trigger prefetch abort
            ptfGfb("Triggering prefetch abort...");
            trigger_prefetch_abort();
        }

        if (kDown & KEY_ZR) {
            // Simulate undefined instruction exception
            ptfGfb("Simulating undefined instruction...\n");
            
            ERRF_FatalErrInfo errorInfo = {
                .type = ERRF_ERRTYPE_EXCEPTION,
                .revHigh = 0,
                .revLow = 0,
                .resCode = 0,
                .pcAddr = (u32)__builtin_return_address(0),
                .procId = 0,
                .titleId = 0,
                .appTitleId = 0,
                .data.exception_data = {
                    .excep = {
                        .type = ERRF_EXCEPTION_UNDEFINED,
                        .fsr = 0,
                        .far = (u32)__builtin_return_address(0),
                        .fpexc = 0,
                        .fpinst = 0,
                        .fpinst2 = 0
                    },
                    .regs = {0}
                }
            };
            ERRF_Throw(&errorInfo);
        }
    }
    
    // Cleanup
    errfExit();
    gfxExit();
    return 0;
}