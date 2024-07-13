// Chris Yocum - June 20, 2024
// make -f makefile-gcc
// make -f makefile-arm
#include <stdio.h>
#include <argp.h>
#include <stdbool.h>
#include <curl/curl.h>
#include <string.h>

#define NO_ARG      0
#define OK          0
#define INIT_ERR    1
#define REQ_ERR     2


static char args_doc[] = "-u http://localhost:8000 -o 'argument'";
static char doc[] = "Performs http request for get, post, delete or put.";

// arguments will be used for storing values from command line
struct Arguments {
    char *arg;  // for string argument
    char *url;    
    bool post;
    bool get;
    bool put;
    bool delete;
};

// OPTIONS
// Name, Key, Arg, Flags, Doc
static struct argp_option options[] = {
    {"url", 'u', "String", NO_ARG, "URL for HTTP Request, REQUIRED"},
    {"post", 'o', NO_ARG, NO_ARG, "POST HTTP Request, requires VERB"},
    {"get", 'g', NO_ARG, NO_ARG, "GET HTTP Request"},
    {"put", 'p', NO_ARG, NO_ARG, "GET HTTP Request, requires VERB"},
    {"delete", 'd', NO_ARG, NO_ARG, "GET HTTP Request, requires VERB"},
    {NO_ARG}
};


int handle_error(char* message, struct argp_state *state) {
    printf("%s",message);
    argp_usage(state);
    return REQ_ERR;
}

// sending the http request
static int send_http_request(char *url, char *message, char *type, bool verb) {
    printf("%s request at url: %s\n", type, url);
    CURL *curl = curl_easy_init();
    if (curl) {
        CURLcode res;
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, type);
        if (verb) {
            printf("message: %s\n", message);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, message);
        } else {
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        }
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            return REQ_ERR;
        }

        curl_easy_cleanup(curl);
    } else {
        return INIT_ERR;
    }
    return OK;
}

// parsing inputs from options
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct Arguments *arguments = state->input;

    switch (key) {
        case 'u':
            arguments->url = arg;
            break;
        case 'o':
            arguments->post = true;
            break; 
        case 'g':
            arguments->get = true;
            break;
        case 'p':
            arguments->put = true;
            break;
        case 'd':
            arguments->delete = true;
            break;
        // standards from argp below, from urls in main ()
        case ARGP_KEY_NO_ARGS:  
        // provides post, put or delete but no url for argument          
            if (arguments->post == true || arguments->put == true || arguments->delete == true) {
                return handle_error("VERB required.\n", state);
            }
        case ARGP_KEY_ARG:    
        // provides post, put or delete but too much in url(s) for argument        
            if (state->arg_num >= 1) {
                printf("Use quotes around arguments.\n");
                argp_usage(state);
                return REQ_ERR;
            }
            arguments->arg = arg;
            break;
        case ARGP_KEY_END:   
        // Bad URL given because it's NULL or wrong, missing operation for URL.         
            if (arguments->url == NULL) {
                printf("Please provide a valid url.\n");
                argp_usage(state);
                return REQ_ERR;
            } else if (arguments->get == false && arguments->post == false && arguments->put == false && arguments->delete == false) {
                return handle_error("Select request type.\n", state);
            }
            break;
        // But if it succeeds....
        case ARGP_KEY_SUCCESS: 
        // Perform CURL based on Verb requested, limit one Verb           
            if (arguments->get) {
                int err = send_http_request(arguments->url, NULL, "GET", false);
                break;
            } else if (arguments->post) {
                int err = send_http_request(arguments->url, arguments->arg, "POST", true);
                break;
            } else if (arguments->put) {
                int err = send_http_request(arguments->url, arguments->arg, "PUT", true);
                break;
            } else if (arguments->delete) {
                int err = send_http_request(arguments->url, arguments->arg, "DELETE", true);
                break;
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

int main(int argc, char **argv) {
    // https://www.gnu.org/software/libc/manual/html_node/Argp-Example-3.html
    // https://www.gnu.org/software/libc/manual/html_node/Argp.html
    struct Arguments arguments;

    // default arguments
    arguments.url = NULL;
    arguments.arg = NULL;
    arguments.post = false;
    arguments.get = false;
    arguments.put = false;
    arguments.delete = false; 

    struct argp argp = {options, parse_opt, args_doc, doc};
    // parse the arguments
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    return 0;
}