#ifndef CERT_H
#define CERT_H

#include <fstream>
#include <sstream>

class Cert
{
public:
    std::ostringstream input;

    Cert();
    void buildCert();
    void certToFile(const std::string& path);
};

#endif