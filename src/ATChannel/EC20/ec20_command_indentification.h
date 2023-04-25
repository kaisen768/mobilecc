#ifndef EC20_COMMAND_INDENTIFICATION_H
#define EC20_COMMAND_INDENTIFICATION_H

#include "ec20_basic.h"

#define EC20_CMD_INDENTIFICATION              "ATI"
#define EC20_CMD_INDENTIFICATION_RESPONSE     "Quectel"
#define EC20_INDENTIFICATION_IDS              "Quectel"

typedef struct ec20_indentification {
    std::string manufacturer;
    std::string model;
    std::string revision;

    ec20_indentification() {
        manufacturer.clear();
        model.clear();
        revision.clear();
    }
} ec20_indentification_t;

EC20_HELPER void ec20_indentification_decode(std::string data, ec20_indentification_t &indec)
{
    if (data.empty())
        return;

    int i;
    ssize_t pos;
    std::string tmp;
    std::istringstream ss;
    std::string element;

    pos = data.find(EC20_CMD_INDENTIFICATION_RESPONSE);

    if (pos != std::string::npos) {
        tmp = data.substr(pos);
        ss.str(tmp);
    } else {
        return;
    }

    i = 1;
    while (std::getline(ss, element, '\n')) {

        size_t r = element.find_last_of('\r');
        if (r != 0)
            element.erase(r);

        std::cout << element.length() << " " << element << std::endl;

        switch (i) 
        {
        case 1:
            indec.manufacturer = element;
            break;

        case 2:
            indec.model = element;
            break;

        case 3:
            indec.revision = element;
            return;

        default:
            break;
        }

        i++;
    }
}

EC20_HELPER void ec20_indentification_encode(std::string &command)
{
    command = EC20_CMD_INDENTIFICATION;
    command.append("\r\n");
}

EC20_HELPER void ec20_indentification_debug(const ec20_indentification_t &indec)
{
    EC20_DEBUG_PRINTF(("\n\n%s\n", EC20_CMD_INDENTIFICATION));
    EC20_DEBUG_PRINTF(("**********************************************************\n"));
    EC20_DEBUG_PRINTF(("Manufacturer:\t%s\n", indec.manufacturer.c_str()));
    EC20_DEBUG_PRINTF(("Model:\t\t%s\n", indec.model.c_str()));
    EC20_DEBUG_PRINTF(("Revision:\t%s\n", indec.revision.c_str()));
    EC20_DEBUG_PRINTF(("**********************************************************\n\n"));
}

#endif
