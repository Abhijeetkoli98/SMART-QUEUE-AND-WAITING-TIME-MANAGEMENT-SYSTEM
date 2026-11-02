#include "QRCodeGenerator.h"
#include "qrcodegen.hpp"
#include <sstream>

using qrcodegen::QrCode;

std::string QRCodeGenerator::generateSvg(const std::string& text, int border) {
    if (text.empty()) return "<svg></svg>";

    try {
        QrCode qr = QrCode::encodeText(text.c_str(), QrCode::Ecc::MEDIUM);
        int size = qr.getSize();
        int totalSize = size + (border * 2);

        std::ostringstream sb;
        sb << "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 "
           << totalSize << " " << totalSize
           << "\" width=\"240\" height=\"240\" shape-rendering=\"crispEdges\">\n";
        sb << "  <rect width=\"100%\" height=\"100%\" fill=\"#ffffff\" rx=\"10\"/>\n";
        sb << "  <path fill=\"#0f172a\" d=\"";

        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                if (qr.getModule(x, y)) {
                    sb << "M" << (x + border) << "," << (y + border) << "h1v1h-1z ";
                }
            }
        }
        sb << "\"/>\n</svg>";
        return sb.str();
    } catch (...) {
        return "<svg><text x=\"10\" y=\"20\">QR Generation Error</text></svg>";
    }
}

static std::string urlEncodeSvg(const std::string& svg) {
    std::ostringstream escaped;
    for (size_t i = 0; i < svg.size(); ++i) {
        char c = svg[i];
        if (c == '"') escaped << "%22";
        else if (c == '#') escaped << "%23";
        else if (c == '<') escaped << "%3C";
        else if (c == '>') escaped << "%3E";
        else if (c == ' ') escaped << "%20";
        else if (c == '\n' || c == '\r') continue;
        else escaped << c;
    }
    return escaped.str();
}

std::string QRCodeGenerator::generateDataUri(const std::string& text, int border) {
    std::string svg = generateSvg(text, border);
    return "data:image/svg+xml;utf8," + urlEncodeSvg(svg);
}
