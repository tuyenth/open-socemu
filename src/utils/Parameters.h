#ifndef PARAMETERS_H_
#define PARAMETERS_H_

#include <string>
#include <map>

// forward declaration
class Parameter;

typedef std::map<std::string, Parameter*> MSP;

/// Parameter definition
class Parameter
{
public:
    /// Minimal constructor
    Parameter()
    {
        this->value = "";
        this->lowercase = "";
    }
    /**
     * Constructor of the parameter
     * @param[in] value Content of the parameter
     */
    Parameter(const std::string &value)
    {
        this->set_string(value);
    }

    /**
     * Set the non parsed value of the parameter content as a string
     * @param[in] str The new parameter value
     */
    void set_string(const std::string &value)
    {
        // copy the value from the parameter
        this->value = value;
        this->lowercase = value;

        // change to lowercase
        for (size_t i = 0; i < this->lowercase.length(); i++)
        {
            this->lowercase[i] = tolower(this->lowercase[i]);
        }
    }

    /**
     * @return The pointer to the string containing the parameter content
     */
    std::string *get_string()
    {
        return &this->value;
    }

    /**
     * @return The C string pointer of the parameter content
     */
    const char *c_str()
    {
        return this->value.c_str();
    }

    /**
     * @return A pointer to a lowercase copy of the parameter content
     */
    std::string *get_lowercase()
    {
        return &this->lowercase;
    }

    /**
     * @return The boolean value of the parameter content
     */
    bool get_bool()
    {
        int foo;

        // check for known words
        if ((this->lowercase == "true") || (this->lowercase == "on"))
            return true;

        // try to read an integer
        foo = this->get_int();
        if (foo == 1)
            return true;
        return false;
    }

    /**
     * @return The integer value of the parameter content, if not parsable, value returned is -1
     */
    int get_int()
    {
        int foo;

        // scan the string for an integer value
        if (sscanf(this->value.c_str(), "%d", &foo) != 1)
            return -1;

        return foo;
    }

    /**
     * @return The pointer to the sub-parameters of the current parameter
     */
    MSP *get_config()
    {
        return &this->config;
    }

    /**
     * Add a path in front of the parameter
     * @param[in] path String of the path to add in front of the parameter
     * @warning This is not an immediate prepend in front of the current parameter,
     * the values are checked for existing path delimiters
     */
    void add_path(const std::string &path)
    {
        if (this->value[0] != '/' && this->value[0] != '\\' &&
            this->value[1] != ':')
        {
            // make sure that path ends with a slash
            if ((path.length() != 0) && (path[path.length()-1] != '/'))
                this->set_string(path + "/" + this->value);
            else
                this->set_string(path + this->value);
        }
    }

    bool operator ==(const std::string& b)
    {
        return this->value == b;
    }
private:
    /// Value of the parameter
    std::string value;
    /// Lowercase value of the parameter
    std::string lowercase;
    /// Sub-parameters list
    MSP config;
};

/// Structure containing command line parameters
class Parameters
{
public:
    /// Name of the configuration as passed at the command line
    std::string configfile;
    /// Path to the configuration file, extracted from the command line
    std::string configpath;
    /// Command line parameter indicating that the debug interface should wait at init
    Parameter gdb_wait;

    /// Parameters parsed from configuration file
    MSP config;
};

#endif /*PARAMETERS_H_*/
