# Services > CFGU > Parental Control Information

A way to get the Parental Control's information.

```cpp
struct Parental {
    u32 restrictionBitmask; // https://www.3dbrew.org/wiki/Config_Savegame#Parental_Control_Restriction_Bitmask
    u32 unknown0x4;         // UNKNOWN
    u8  ratingSystem;       // Rating system used for configuration 
    u8  maxAllowedAge;      // The rating system games that you can play if you're on that minimum age.
    u8  secretQuestion;     // Secret Question Type (0-5: Pre-defined, 6: Custom) 
    u8  unknown0xB;         // UNKNOWN
    u16 pinCode[4];         // Parental Controls PIN code (with NULL-termination, although restricted to 4 digits), this is also gonna get extracted.
    u16 secretAnswer[34];   // Secret Answer (UTF-16) 
};
```