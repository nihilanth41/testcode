/* Example of getopt() usage courtesy of IBM:
http://www.ibm.com/developerworks/aix/library/au-unix-getopt.html */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 

struct globalArgs_t {
    int numSamples;             /* -n option */
    const char *logFileName;    /* -l option */
    FILE *logFile;
    const char *outFileName;    /* -o option */
    FILE *outFile;
    int verbosity;              /* -v option */
    char **inputFiles;          /* input files */
    int numInputFiles;          /* # of input files */
} globalArgs;

static const char *optString = "n:l:o:vh?";

/* function prototype(s): */
void display_usage(void);

int main(int argc, char **argv, char **envp) { 
/* Begin checking command line arguments */
int opt = 0;
/* Initialize globalArgs before we get to work. */
 globalArgs.numSamples = 100; //Default 
 globalArgs.logFileName = NULL;
 globalArgs.outFileName = NULL;
 globalArgs.outFile = NULL;
 globalArgs.verbosity = 0;
 globalArgs.inputFiles = NULL;
 globalArgs.numInputFiles = 0;
 
 /*The while loop and switch statement are the meat of the command-line processing for this program. 
 Whenever getopt() discovers an option, the switch statement decides which option was found, and you take note of that in the globalArgs structure. 
 When getopt() finally returns -1, you're done processing options, and the remaining arguments are your input files. */
 opt = getopt( argc, argv, optString );
    while( opt != -1 ) {
        switch( opt ) {
            case 'n':
		globalArgs.numSamples = atoi(optarg); 
                break;
                
            case 'l':
                globalArgs.logFileName = optarg;
                break;
                
            case 'o':
                globalArgs.outFileName = optarg;
                break;
                
            case 'v':
                globalArgs.verbosity++;
                break;
                
            case 'h':   /* fall-through is intentional */
            case '?':
                display_usage();
                return 0; /* close main() */
		break;
                
            default:
                /* You won't actually get here. */
                break; }
    opt = getopt( argc, argv, optString ); }
    
    /* Remaining arguments are input files */
    globalArgs.inputFiles = argv + optind;
    globalArgs.numInputFiles = argc - optind;
    
    return 0; }
    
    
/* prints usage information to stdout if -h or -? option is specified. */
void display_usage(void) {
printf("Command Line Arguments:\n \
-n --num-samples 	number of samples (rounded to the next integer)\n \
-v --verbose		enable verbosity\n \
-vv --extra-verbose    extra verbosity (Prints timer output, etc.)\n \
-l --log <logfile>	optional  logfile for errors and non-data output\n \
-o --output <outfile> 	optional file for program data output\n \
Program takes input file on stdin\n");
return; }
    
