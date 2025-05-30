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

int main() {
    // Initialize services
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);
    errfInit();  // Initialize error framework
    
    printf("ERRF Demo - libctru\n");
    printf("A: Generic Fatal Error\n");
    printf("B: Custom Error Message\n");
    printf("X: Simulate Data Abort\n");
    printf("Y: Log Only (No Reboot)\n");
    printf("L: NAND Damaged Error\n");
    printf("R: Card Removed Error\n");
    printf("ZL: Prefetch Abort Exception\n");
    printf("ZR: Undefined Instruction\n");
    printf("START: Exit\n\n");
    printf("Note: Most errors will reboot the system!\n");

    while (aptMainLoop()) {
        gspWaitForVBlank();
        hidScanInput();
        u32 kDown = hidKeysDown();

        if (kDown & KEY_START) break;

        if (kDown & KEY_A) {
            // Create a custom result code
            Result myResult = MAKERESULT(RL_PERMANENT, RS_INVALIDARG, RM_APPLICATION, 1);
            
            // Throw a generic fatal error
            printf("Throwing generic fatal error...\n");
            gfxFlushBuffers();
            ERRF_ThrowResult(myResult);
            
            // This line will only execute if throw fails
            printf("Throw failed! 0x%08lX\n", myResult);
        }

        if (kDown & KEY_B) {
            // Set custom user string (appears in some error reports)
            ERRF_SetUserString("ERRF Demo Custom String");
            
            // Throw error with custom message
            printf("Throwing custom message error...\n");
            gfxFlushBuffers();
            ERRF_ThrowResultWithMessage(MAKERESULT(RL_FATAL, RS_INTERNAL, RM_APPLICATION, 2), "Custom error message from libctru demo!");
        }

        if (kDown & KEY_X) {
            // Simulate a CPU exception (data abort)
            printf("Simulating CPU data abort...\n");
            gfxFlushBuffers();
            trigger_data_abort();
        }

        if (kDown & KEY_Y) {
            // Log an error without causing reboot
            printf("Logging non-fatal error...\n");
            Result rc = ERRF_LogResult(MAKERESULT(RL_USAGE, RS_NOTSUPPORTED, RM_APPLICATION, 3));
            
            if (R_SUCCEEDED(rc)) {
                printf("Error logged successfully!\n");
            } else {
                printf("Logging failed! 0x%08lX\n", rc);
            }
        }

        if (kDown & KEY_L) {
            // Simulate NAND damaged error
            printf("Throwing NAND damaged error...\n");
            gfxFlushBuffers();
            
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
            printf("Throwing card removed error...\n");
            gfxFlushBuffers();
            
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
            printf("Triggering prefetch abort...\n");
            gfxFlushBuffers();
            trigger_prefetch_abort();
        }

        if (kDown & KEY_ZR) {
            // Simulate undefined instruction exception
            printf("Simulating undefined instruction...\n");
            gfxFlushBuffers();
            
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