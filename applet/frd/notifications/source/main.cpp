#include <3ds.h>
#include <stdio.h>
#include <string>

// Example on using FRD notifications.
// With More error handling that you made me do i guess this'll work for you.

/*
 * A definition to go to the "fail" label if the result has failed.
 */
#define CHECK_RESULT(funcName, result) \
    do { \
        printf("%s: %s! (0x%lX)\n", funcName, R_FAILED(result) ? "failed" : "success", result); \
        if (R_FAILED(result)) { \
            goto fail; \
        } \
    } while (false)

/**
 * Converts a u16 VTA to a char VTA.
 *
 * @param input A VTA variable that is u16-crypted
 * @param length Length of the VTA variable, must be in a `sizeof`.
 * @return A char VTA that's converted.
 */
std::string u16ToString(u16 *input, size_t length) {
    u8 out[length + 1];
    out[utf16_to_utf8(out, input, length)] = '\0';
    return (char *)out;
}

/**
 * A thread just to start processing the FRD notifications.
 */
void threadStart(void *handleForFRD) {
    // Cast void* to Handle so that it can be recognized by `svcWaitSynchronization`.
    Handle frdHandle = (Handle)handleForFRD;

    // A great while loop that can be broken if one of the random FRD functions fail (they will, or will they?)
    while (true) {
        // Wait until FRD got hit by a new notification.
        if (svcWaitSynchronization(frdHandle, 5000000000) == 0) {
            // Declare 3 variables, for the event, information and just the useless total notifications.
            NotificationEvent event;
            FriendInfo profile;
            u32 totalNotifs;

            // Get the new event info and convert to a long width of information.
            CHECK_RESULT("FRD_GetEventNotification", FRD_GetEventNotification(&event, 1, &totalNotifs));
            CHECK_RESULT("FRD_GetFriendInfo", FRD_GetFriendInfo(&profile, &event.sender, 1, false, false));

            const char *eventType = "";
            switch (event.type) {
                case USER_WENT_ONLINE: {
                    eventType = "Self went online";
                    break;
                } case USER_WENT_OFFLINE: {
                    eventType = "Self went offline";
                    break;
                } case FRIEND_WENT_ONLINE: {
                    eventType = "Friend Went Online";
                    break;
                } case FRIEND_UPDATED_PRESENCE: {
                    eventType = "Friend Presence changed";
                    break;
                } case FRIEND_UPDATED_MII: {
                    eventType = "Friend Mii changed";
                    break;
                } case FRIEND_UPDATED_PROFILE: {
                    eventType = "Friend Profile changed";
                    break;
                } case FRIEND_WENT_OFFLINE: {
                    eventType = "Friend went offline";
                    break;
                } case FRIEND_REGISTERED_USER: {
                    eventType = "Friend registered self as friend";
                    break;
                } case FRIEND_SENT_JOINABLE_INVITATION: {
                    eventType = "Friend sent invitation";
                    break;
                } default: {
                    eventType = "Unknown event.";
                    break;
                }
            }

            // Do some information here.
            printf("Screen Name:   %s\n", u16ToString(profile.screenName, sizeof(profile.screenName)).c_str());
            printf("Reason (Type): %u (Text: %s)\n", event.type, eventType);
            printf("Principal ID:  %lu\n", event.sender.principalId);
            printf("Total Notifs:  %lu\n", totalNotifs);
        }
    }

// Exit thread.
fail:
    threadExit(0);
}

int main() {
    // Initialize needed services.
    gfxInitDefault();
    consoleInit(GFX_TOP, nullptr);

    // Print Stuff
    printf("libctru 3DS Notification Test\n");
    printf("Press [START] to exit.\n");

    // Initialize handle for notification callbacks.
    Handle frdHandle;
    Thread thread = NULL;
    int32_t priority;

    // Initialize FRD.
    CHECK_RESULT("frdInit", frdInit(false));

    // Create a new event for this.
    CHECK_RESULT("svcCreateEvent", svcCreateEvent(&frdHandle, RESET_ONESHOT));

    // Attach an event to a notification, duh.
    CHECK_RESULT("FRD_AttachToEventNotification", FRD_AttachToEventNotification(frdHandle));

    // For creating a thread, let's just do this.
    CHECK_RESULT("svcGetThreadPriority", svcGetThreadPriority(&priority, CUR_THREAD_HANDLE));

    // Make a thread with this weirdly complicated way.
    priority -= 1;
    thread = threadCreate(threadStart, (void *)frdHandle, 32768, priority < 0x18 ? 0x18 : priority > 0x3F ? 0x3F : priority, -1, false);

    // Main Loop.
fail:
    while (aptMainLoop()) {
        hidScanInput();
        if (hidKeysDown() & KEY_START) {
            break;
        }
    }

    // Exit thread, Close handle and exit services.
#undef CHECK_RESULT
    threadFree(thread);
    svcCloseHandle(frdHandle);
    frdExit();
    gfxExit();
    return EXIT_SUCCESS;
}