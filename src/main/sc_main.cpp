#include <vector>

#include "Top/Top.h"
#include "Bob/Bob.h"
#include "At91sam9261/At91sam9261.h"
#include "Mc13224v/Mc13224v.h"
#include "B2070/B2070.h"

void usage(void)
{
    printf("\n"
            "Synopsis:\n"
            "    open-socemu [-d] (configfile)\n\n"
            "Parameters:\n"
            "    - -d : indicates that the connection to the debugger should\n"
            "        be polled on before starting the execution\n\n"
            "    - configfile : XML file containing the configuration of the\n"
            "        plateform.\n\n");

}

void recurse_parameters(int level, MSP *children)
{
    for (MSP::const_iterator i = children->begin(); i != children->end(); ++i)
    {
        printf("  - ");
        for (int j = 0; j < level; j++)
            printf("    ");
        printf("%s = %s\n", (*i).first.c_str(), (*i).second->c_str());
        // all the equivalent for the sub parameters
        recurse_parameters(level+1, (*i).second->get_config());
    }
}

/// Main SystemC entry point to our environment
int sc_main(int argc, char* argv[])
{
    MSP *scope;
    Parameters parameters;
    Parameter *parameter;
    char *configfile, *opt, *buf;
    char line[4096];
    std::vector<MSP*> stack;
    int i;
    FILE* fp;
    size_t pos;
    int indentation, linenum;

    // initialize the parameters
    parameters.gdb_wait.set_string("FALSE");

    // initialize the configuration file
    configfile = NULL;

    for (i = 1; i < argc; i++)
    {
        // retrieve the current option
        opt = argv[i];
        if (opt[0] != '-')
        {
            // check if another configuration file was not specified
            if (configfile != NULL)
            {
                usage();
                printf("\nERROR: configuration file (%s) already provided, (%s) is extra\n", configfile, opt);
                return -1;
            }
            configfile = opt;
        }
        else
        {
            // handle - and -- the same way
            if (opt[1] == '-')
                opt++;

            switch (opt[1])
            {
            case 'd':
                parameters.gdb_wait.set_string("TRUE");
                break;
            default:
                usage();
                printf("\nERROR: unsupported parameter (%s)\n", opt);
                return -1;
                break;
            }

        }
    }

    // check that at least a configuration file was provided
    if (configfile == NULL)
    {
        usage();
        printf("\nERROR: configuration file not provided\n");
        return -1;
    }

    // save the configfile in the parameters structure
    parameters.configfile.assign(configfile);

    // extract the path from the configuration file
    parameters.configpath.assign(configfile);

    // find the last occurrence of / or \ in the string
    // cast rfind to int because otherwise returns -1 as unsigned which leads to error
    pos = ((int)parameters.configpath.rfind('/') > (int)parameters.configpath.rfind('\\'))?
            parameters.configpath.rfind('/'):parameters.configpath.rfind('\\');
    //printf("configpath = %s\n", parameters.configpath.c_str());
    //printf("configpath pos = %d\n", pos);
    if (pos != std::string::npos)
        parameters.configpath.erase(pos+1);
    else
        parameters.configpath.clear();

    // reset the current indentation
    indentation = linenum = 0;
    // reset the current scope
    scope = &parameters.config;
    // reset the current parameter
    parameter = NULL;

    // open the configuration file
    fp = fopen(parameters.configfile.c_str(), "r");

    // read line by line
    while (fgets(line, 4096, fp) != NULL)
    {
        int diff;

        // increment the line number
        linenum++;

        // sanity check (line length)
        if (strlen(line) > 4000)
        {
            printf("\nERROR: in configuration file, line %d, too close to maximum line length\n", linenum);
            return -1;
        }

        // sanity check (tabs are not supported)
        buf = line;
        while (*buf != '\0')
        {
            if (*buf == '\t')
            {
                printf("\nERROR: in configuration file, line %d, tabs not supported in configuration file\n", linenum);
                return -1;
            }
            buf++;
        }

        // remove the newline char
        buf = line;
        while ((*buf != '\r') && (*buf != '\n')) buf++;
        *buf = '\0';

        // count the leading spaces
        buf = line;
        while (*buf == ' ') buf++;

        // check if this was an empty line or a comment
        if ((*buf == '\0') || (*buf == '#')) continue;

        // compute the difference to the last indentation
        diff = buf - line - indentation;

        // save the new indentation value
        indentation = buf - line;

        // debug
        //printf("line:\n%s\ndiff=%d\n", line, diff);

        // if there is no indentation difference with the previous
        if (diff == 0)
        {
        }
        else if (diff == 4)
        {
            // single indentation

            // sanity check: there is already a parameter created
            if (parameter == NULL)
            {
                printf("\nERROR: in configuration file, line %d, indentation without preceding parameter definition\n", linenum);
                return -1;
            }

            // push the current scope onto the stack
            stack.push_back(scope);

            // retrieve the new scope
            scope = parameter->get_config();
        }
        else if (diff < 0)
        {
            // sanity check: should be a multiple of 4
            if (diff & 3)
            {
                printf("\nERROR: in configuration file, line %d, line indentation is not multiple of 4\n", linenum);
                return -1;
            }

            // pop the scope
            for (; diff != 0; diff += 4)
            {
                // retrieve the last element
                scope = stack.back();
                // pop the last element
                stack.pop_back();
            }
        }
        else
        {
            printf("\nERROR: in configuration file, line %d, unsupported configuration line indentation\n", linenum);
            return -1;
        }

        char *value = buf;

        // look for the next space char and at the same time convert parameter name to lowercase
        while ((*value != '\0') && (*value != ' '))
        {
            *value = tolower(*value);
            value++;
        }

        // check if there is at least a value string
        if (*value == '\0')
        {
            printf("\nERROR: in configuration file, line %d, line format not correct\n", linenum);
            return -1;
        }

        // end the string of the name of the parameter
        *value = '\0';
        value++;

        // debug
        //fprintf(stderr, "parameter (%s) = (%s)\n", buf, value);

        // create a new parameter
        parameter = new Parameter(value);

        // append the parameter to the current scope list
        (*scope)[buf] = parameter;
    }
    // close the file
    fclose(fp);


    printf("  - Config file path %s\n", parameters.configpath.c_str());
    printf("  - Config file name %s\n", parameters.configfile.c_str());
    printf("  - GDB wait at start %s\n", parameters.gdb_wait.c_str());
    recurse_parameters(0, &parameters.config);

    // check if there is a platform defined
    if (parameters.config.count("platform") != 1)
    {
        printf("\nERROR: platform definition not found in configuration file\n");
        return -1;
    }
    parameter = parameters.config["platform"];

    // check if it is an AT91SAM9261 platform that is requested
    if (*parameter == "at91sam9261")
    {
        At91sam9261 at91sam9261("at91sam9261", parameters, *parameter);
    }
    else if (*parameter == "mc13224v")
    {
        Mc13224v mc13224v("mc13224v", parameters, *parameter);
    }
    else if (*parameter == "b2070")
    {
        B2070 b2070("b2070", parameters, *parameter);
    }
    else if (*parameter == "top")
    {
        // create the Top level system
        Top top("top", parameters, *parameter);
    }
    else if (*parameter == "bob")
    {
        // create the Top level system
        Bob bob("top", parameters, *parameter);
    }
    else
    {
        printf("\nERROR: unknown platform %s\n", parameter->c_str());
        return -1;
    }

    // start the simulation
    sc_core::sc_start();
    return 0;
}

