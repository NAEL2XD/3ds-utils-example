# ARM > ERRF Information

The ERRF (Error Framework) is a system service in Nintendo 3DS that handles fatal errors and exceptions. It provides a structured way to:
- Display custom error messages to users
- Log system errors to storage
- Handle CPU and floating-point exceptions
- Trigger system reboots on fatal errors
- Generate detailed crash reports for debugging

Homebrew developers use ERRF to:
1. Create custom error handling in applications
2. Simulate system-level crashes for testing
3. Log non-fatal errors without disrupting gameplay
4. Generate professional-looking error screens
5. Handle hardware exceptions gracefully

## Types of Errors:

```c
/// Types of errors that can be thrown by err:f.
typedef enum {
    ERRF_ERRTYPE_GENERIC      = 0,  ///< Generic fatal error. Shows miscellaneous info, including the address of the caller
    ERRF_ERRTYPE_NAND_DAMAGED = 1,  ///< Damaged NAND (CC_ERROR after reading CSR)
    ERRF_ERRTYPE_CARD_REMOVED = 2,  ///< Game content storage medium (cartridge and/or SD card) ejected. Not logged
    ERRF_ERRTYPE_EXCEPTION    = 3,  ///< CPU or VFP exception
    ERRF_ERRTYPE_FAILURE      = 4,  ///< Fatal error with a message instead of the caller's address
    ERRF_ERRTYPE_LOG_ONLY     = 5,  ///< Log-level failure. Does not display the exception and does not force the system to reboot
} ERRF_ErrType;

/// Types of 'Exceptions' thrown for ERRF_ERRTYPE_EXCEPTION
typedef enum {
    ERRF_EXCEPTION_PREFETCH_ABORT = 0, ///< Prefetch Abort
    ERRF_EXCEPTION_DATA_ABORT     = 1, ///< Data abort
    ERRF_EXCEPTION_UNDEFINED      = 2, ///< Undefined instruction
    ERRF_EXCEPTION_VFP            = 3, ///< VFP (floating point) exception.
} ERRF_ExceptionType;
```

## Key Functions:

| Function | Description |
|----------|-------------|
| `ERRF_Throw()` | Throws a system error with detailed info |
| `ERRF_ThrowResult()` | Throws error with Result code |
| `ERRF_ThrowResultWithMessage()` | Throws error with custom message |
| `ERRF_LogResult()` | Logs error without reboot |
| `ERRF_SetUserString()` | Adds custom text to error reports |
| `ERRF_ExceptionHandler()` | Handles CPU exceptions |

## Basic Usage:

1. Initialize ERRF service: `errfInit()`
2. Throw errors when needed:
```c
// Simple fatal error
ERRF_ThrowResult(MAKERESULT(RL_PERMANENT, RS_INVALIDARG, RM_APPLICATION, 1));

// Custom error message
ERRF_ThrowResultWithMessage(0xD900182, "Game data corrupted!");

// CPU exception simulation
ERRF_FatalErrInfo errorInfo = {
    .type = ERRF_ERRTYPE_EXCEPTION,
    // ... populate struct ...
};
ERRF_Throw(&errorInfo);
```

3. Clean up on exit: `errfExit()`

## Example Scenarios:

1. **Game Cartridge Removed**  
   `ERRF_ERRTYPE_CARD_REMOVED` - Show error when cartridge is ejected during gameplay

2. **Corrupted Save Data**  
   `ERRF_ThrowResultWithMessage()` - Display custom recovery instructions

3. **Debugging Crashes**  
   `ERRF_ERRTYPE_EXCEPTION` - Capture CPU state during crashes

4. **Non-Fatal Logging**  
   `ERRF_LogResult()` - Record gameplay errors without interrupting the player

5. **System Warnings**  
   `ERRF_ERRTYPE_FAILURE` - Display important system notifications

Note: Most errors will force a system reboot. Use `ERRF_ERRTYPE_LOG_ONLY` for non-fatal errors.

<h6>Generated using DeepSeek