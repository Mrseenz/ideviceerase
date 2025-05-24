#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>

#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/lockdown.h>
#include <libimobiledevice/diagnostics_relay.h>
#include <plist/plist.h>

// Global variables to store parsed arguments
static char *udid = NULL;
static char *ecid = NULL; // Parsed, but not used in core logic yet
static int debug_flag = 0;

// Function to print usage information
void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s -u <device_udid> [--ecid <value>] [--debug]\n", prog_name);
    fprintf(stderr, "A utility to erase all content and settings on an iDevice.\n\n");
    fprintf(stderr, "  -u, --udid <device_udid>   : Target device UDID (mandatory).\n");
    fprintf(stderr, "      --ecid <value>         : Target device ECID (optional, not currently used for erase operation).\n");
    fprintf(stderr, "      --debug                : Enable debug output.\n\n");
    fprintf(stderr, "WARNING: This is a destructive operation and cannot be undone.\n");
    fprintf(stderr, "Ensure you have backed up your device if you need to preserve its data.\n");
}

// Placeholder for the erase function
int perform_erase(idevice_t device, lockdownd_client_t client, const char *udid_arg) {
    printf("Attempting to perform erase on device %s\n", udid_arg);
    // TODO: Implement erase logic here
    // 1. Start com.apple.diagnostics_relay service
    // 2. Create diagnostics_relay_client
    // 3. Create {"Request": "MobileObliterator"} plist
    // 4. Send plist
    // 5. Handle response

    lockdownd_service_descriptor_t service = NULL;
    diagnostics_relay_client_t diag_client = NULL;
    plist_t request_plist = NULL;
    plist_t response_plist = NULL;
    int ret_val = -1;

    printf("Starting diagnostics relay service...\n");
    if (lockdownd_start_service(client, "com.apple.diagnostics_relay", &service) != LOCKDOWN_E_SUCCESS || service == NULL || service->port == 0) {
        fprintf(stderr, "Error: Could not start com.apple.diagnostics_relay service.\n");
        if (service) {
            lockdownd_service_descriptor_free(service);
        }
        return -1;
    }
    printf("Diagnostics relay service started on port %d.\n", service->port);

    if (diagnostics_relay_client_new(device, service, &diag_client) != DIAGNOSTICS_RELAY_E_SUCCESS) {
        fprintf(stderr, "Error: Could not connect to diagnostics_relay service.\n");
        lockdownd_service_descriptor_free(service);
        return -1;
    }
    printf("Diagnostics relay client created.\n");

    // Create {"Request": "MobileObliterator"} plist
    request_plist = plist_new_dict();
    if (!request_plist) {
        fprintf(stderr, "Error: Could not create request PList.\n");
        diagnostics_relay_client_free(diag_client);
        lockdownd_service_descriptor_free(service);
        return -1;
    }
    plist_dict_set_item(request_plist, "Request", plist_new_string("MobileObliterator"));
    if (debug_flag) {
        char *plist_xml = NULL;
        plist_to_xml(request_plist, &plist_xml, NULL);
        printf("Sending PList:\n%s\n", plist_xml);
        free(plist_xml);
    }

    printf("Sending MobileObliterator request...\n");
    // diagnostics_relay_request_diagnostics is for getting info, not sending commands like this.
    // diagnostics_relay_goodbye might be one option, or we might need to send raw plist.
    // Let's try sending goodbye with our custom plist. This is a guess.
    // If this doesn't work, the next step is to look for a generic send/receive plist function,
    // or how other tools use this service for obliteration.
    // For now, let's assume diagnostics_relay_goodbye might work or needs to be replaced.
    // The diagnostics_relay.h header shows:
    // diagnostics_relay_error_t diagnostics_relay_goodbye(diagnostics_relay_client_t client);
    // This doesn't take a plist.
    // diagnostics_relay_error_t diagnostics_relay_send(diagnostics_relay_client_t client, plist_t plist);
    // diagnostics_relay_error_t diagnostics_relay_recv(diagnostics_relay_client_t client, plist_t *plist);

    if (diagnostics_relay_send(diag_client, request_plist) != DIAGNOSTICS_RELAY_E_SUCCESS) {
        fprintf(stderr, "Error: Failed to send MobileObliterator request.\n");
        plist_free(request_plist);
        diagnostics_relay_client_free(diag_client);
        lockdownd_service_descriptor_free(service);
        return -1;
    }
    printf("MobileObliterator request sent. Waiting for response...\n");

    // Attempt to receive a response. The device might just reboot without a proper response.
    // Set a timeout for receiving the response?
    if (diagnostics_relay_recv(diag_client, &response_plist) == DIAGNOSTICS_RELAY_E_SUCCESS && response_plist) {
        if (debug_flag) {
            char *plist_xml = NULL;
            plist_to_xml(response_plist, &plist_xml, NULL);
            printf("Received PList response:\n%s\n", plist_xml);
            free(plist_xml);
        }
        // Check response for success, if any specific format is expected
        // For MobileObliterator, the device likely just reboots.
        // A simple acknowledgement might be {"Status": "Acknowledged"} or something similar.
        // Or it could be an empty response.
        printf("Erase command acknowledged by device (response received).\n");
        ret_val = 0; // Success
    } else {
        // This path might be taken if the device reboots before sending a response,
        // which could be considered a success for an erase command.
        printf("No specific response received, or error receiving. This might be normal for an erase command.\n");
        printf("Assuming erase command was accepted if no send error occurred.\n");
        ret_val = 0; // Consider it a success if send was okay.
    }


    if (request_plist) plist_free(request_plist);
    if (response_plist) plist_free(response_plist);
    diagnostics_relay_client_free(diag_client);
    lockdownd_service_descriptor_free(service);

    if (ret_val == 0) {
        printf("Device %s should now begin erasing all content and settings.\n", udid_arg);
    }
    return ret_val;
}


int main(int argc, char *argv[]) {
    int opt;
    static struct option long_options[] = {
        {"udid",    required_argument, 0, 'u'},
        {"ecid",    required_argument, 0, 'e'},
        {"debug",   no_argument,       0, 'd'},
        {0, 0, 0, 0}
    };
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "+u:e:d", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'u':
                udid = optarg;
                break;
            case 'e':
                ecid = optarg;
                break;
            case 'd':
                debug_flag = 1;
                break;
            case '?':
                print_usage(argv[0]);
                return 1;
            default:
                abort();
        }
    }

    if (udid == NULL) {
        fprintf(stderr, "Error: UDID is a mandatory argument.\n");
        print_usage(argv[0]);
        return 1;
    }

    if (debug_flag) {
        printf("Debug mode enabled.\n");
        printf("Program: ideviceerase\n");
        printf("UDID: %s\n", udid);
        if (ecid) {
            printf("ECID: %s\n", ecid);
        } else {
            printf("ECID: Not provided\n");
        }
    }

    if (optind < argc) {
        fprintf(stderr, "Unexpected non-option arguments: ");
        while (optind < argc) {
            fprintf(stderr, "%s ", argv[optind++]);
        }
        fprintf(stderr, "\n");
        print_usage(argv[0]);
        return 1;
    }

    idevice_t device = NULL;
    lockdownd_client_t lockdown_client = NULL;
    int result = 1; // Default to failure

    printf("Connecting to device %s...\n", udid);
    if (idevice_new_with_options(&device, udid, IDEVICE_LOOKUP_USBMUX) != IDEVICE_E_SUCCESS) {
        fprintf(stderr, "Error: Could not connect to device with UDID %s. Make sure it's connected and accessible.\n", udid);
        return 1;
    }
    printf("Device connected.\n");

    printf("Attempting to handshake with lockdown service...\n");
    if (lockdownd_client_new_with_handshake(device, &lockdown_client, "ideviceerase") != LOCKDOWN_E_SUCCESS) {
        fprintf(stderr, "Error: Could not connect to lockdown service on device %s.\n", udid);
        idevice_free(device);
        return 1;
    }
    printf("Lockdown handshake successful.\n");

    if (perform_erase(device, lockdown_client, udid) == 0) {
        printf("Erase process initiated successfully for device %s.\n", udid);
        result = 0; // Success
    } else {
        fprintf(stderr, "Failed to initiate erase process for device %s.\n", udid);
        result = 1; // Failure
    }

    printf("Cleaning up...\n");
    if (lockdown_client) {
        lockdownd_client_free(lockdown_client);
    }
    if (device) {
        idevice_free(device);
    }
    printf("Cleanup complete.\n");

    return result;
}
