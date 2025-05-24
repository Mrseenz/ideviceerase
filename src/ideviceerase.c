#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

// Global variables to store parsed arguments
static char *udid = NULL;
static char *ecid = NULL;
static int debug_flag = 0;

// Function to print usage information
void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s -u <device_udid> [--ecid <value>] [--debug]\n", prog_name);
    fprintf(stderr, "  -u, --udid <device_udid>   : Target device UDID (mandatory).\n");
    fprintf(stderr, "      --ecid <value>         : Target device ECID.\n");
    fprintf(stderr, "      --debug                : Enable debug output.\n");
}

int main(int argc, char *argv[]) {
    int opt;
    // Define long options
    static struct option long_options[] = {
        {"udid",    required_argument, 0, 'u'},
        {"ecid",    required_argument, 0, 'e'}, // Using 'e' as a short option internally for ecid
        {"debug",   no_argument,       0, 'd'}, // Using 'd' as a short option internally for debug
        {0, 0, 0, 0} // Terminating entry
    };
    int option_index = 0;

    // Argument parsing loop
    // The '+' in "+u:e:d" means getopt stops scanning as soon as a non-option argument is found.
    // 'u:' means -u takes an argument. 'e:' means --ecid (internally 'e') takes an argument. 'd' means --debug (internally 'd') is a flag.
    while ((opt = getopt_long(argc, argv, "+u:e:d", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'u':
                udid = optarg;
                break;
            case 'e': // Corresponds to --ecid
                ecid = optarg;
                break;
            case 'd': // Corresponds to --debug
                debug_flag = 1;
                break;
            case '?': // Unknown option or missing argument
                // getopt_long already prints an error message
                print_usage(argv[0]);
                return 1;
            default:
                // This case should ideally not be reached if '+' is used in optstring
                // and all long options have corresponding short options or are handled.
                abort(); 
        }
    }

    // Check for mandatory arguments
    if (udid == NULL) {
        fprintf(stderr, "Error: UDID is a mandatory argument.\n");
        print_usage(argv[0]);
        return 1;
    }

    // Print parsed arguments
    printf("Program: ideviceerase\n");
    printf("UDID: %s\n", udid);
    if (ecid) {
        printf("ECID: %s\n", ecid);
    } else {
        printf("ECID: Not provided\n");
    }
    printf("Debug mode: %s\n", debug_flag ? "enabled" : "disabled");

    // Handle non-option arguments if any (not expected for this tool)
    if (optind < argc) {
        fprintf(stderr, "Unexpected non-option arguments: ");
        while (optind < argc) {
            fprintf(stderr, "%s ", argv[optind++]);
        }
        fprintf(stderr, "\n");
        print_usage(argv[0]);
        return 1;
    }

    return 0; // Success
}
