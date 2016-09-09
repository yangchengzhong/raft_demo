#include "arg_parse.h"

#include "common_def.h"
#include "error_code.h"
#include "argparse.h"

static char* global_file = NULL;

static const char *const usage[] = {
    "[options] [[--] args]",
    NULL,
};

int arg_parse_init(int argc, char* argv[])
{
    assert_retval(argc >= 0 && NULL != argv, ERR_INVALID_ARG);

    struct argparse_option options[] = 
    {
        OPT_HELP(),
        OPT_STRING('c', "config", &global_file, "global_file"),
        OPT_END(),
    };
    
    struct argparse argparse;
    argparse_init(&argparse, options, usage, 0);
    argc = argparse_parse(&argparse, argc, (const char **)argv);
    
    if(NULL == global_file)
    {
        argparse_usage(&argparse);
        return ERR_INPUT_INVALID;
    }

    return 0;
}

const char* arg_parse_get_global_cfg_name()
{
    return global_file;
}

