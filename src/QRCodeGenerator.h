#ifndef QR_CODE_GENERATOR_H
#define QR_CODE_GENERATOR_H

#include <string>

class QRCodeGenerator {
public:
    // Generates a clean, standalone SVG XML string of the QR Code
    static std::string generateSvg(const std::string& text, int border = 3);

    // Generates an encoded Data URI string suitable for <img src="...">
    static std::string generateDataUri(const std::string& text, int border = 3);
};

#endif // QR_CODE_GENERATOR_H
