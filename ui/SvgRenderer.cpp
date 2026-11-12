#include "SvgRenderer.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace graphx {
namespace ui {

namespace {

const int kWidth = 1000;
const int kHeight = 700;
const double kLeft = 72.0;
const double kRight = 30.0;
const double kTop = 55.0;
const double kBottom = 55.0;

std::string escapeXml(const std::string& value) {
    std::string result;
    for (std::size_t i = 0; i < value.size(); ++i) {
        switch (value[i]) {
        case '&': result += "&amp;"; break;
        case '<': result += "&lt;"; break;
        case '>': result += "&gt;"; break;
        case '"': result += "&quot;"; break;
        case '\'': result += "&apos;"; break;
        default: result += value[i]; break;
        }
    }
    return result;
}

double mapX(double x, double minimum, double maximum) {
    return kLeft + (x - minimum) / (maximum - minimum) *
                   (kWidth - kLeft - kRight);
}

double mapY(double y, double minimum, double maximum) {
    return kHeight - kBottom - (y - minimum) / (maximum - minimum) *
                                 (kHeight - kTop - kBottom);
}

std::string makeSvg(const Expression& expression,
                    const DynamicArray<GraphSample>& samples,
                    double minimum,
                    double maximum) {
    std::ostringstream svg;
    svg << std::fixed << std::setprecision(2);
    const double plotWidth = kWidth - kLeft - kRight;
    const double plotHeight = kHeight - kTop - kBottom;
    const bool includesZero = minimum <= 0.0 && maximum >= 0.0;
    const double zeroX = includesZero ? mapX(0.0, minimum, maximum) :
                         (minimum > 0.0 ? kLeft : kWidth - kRight);
    const double zeroY = includesZero ? mapY(0.0, minimum, maximum) :
                         (minimum > 0.0 ? kHeight - kBottom : kTop);

    svg << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << kWidth
        << "\" height=\"" << kHeight << "\" viewBox=\"0 0 "
        << kWidth << ' ' << kHeight << "\">\n"
        << "<rect width=\"100%\" height=\"100%\" fill=\"#fbfcff\"/>\n"
        << "<text x=\"" << kLeft << "\" y=\"30\" font-family=\"Arial\" "
        << "font-size=\"18\" font-weight=\"bold\" fill=\"#172033\">y = "
        << escapeXml(expression.source()) << "</text>\n"
        << "<g stroke=\"#e1e6ef\" stroke-width=\"1\">\n";

    for (int i = 0; i <= 10; ++i) {
        const double x = kLeft + plotWidth * i / 10.0;
        const double y = kTop + plotHeight * i / 10.0;
        svg << "<line x1=\"" << x << "\" y1=\"" << kTop << "\" x2=\""
            << x << "\" y2=\"" << kHeight - kBottom << "\"/>\n"
            << "<line x1=\"" << kLeft << "\" y1=\"" << y << "\" x2=\""
            << kWidth - kRight << "\" y2=\"" << y << "\"/>\n";
    }
    svg << "</g>\n<g stroke=\"#64748b\" stroke-width=\"1.7\">"
        << "<line x1=\"" << zeroX << "\" y1=\"" << kTop << "\" x2=\""
        << zeroX << "\" y2=\"" << kHeight - kBottom << "\"/>"
        << "<line x1=\"" << kLeft << "\" y1=\"" << zeroY << "\" x2=\""
        << kWidth - kRight << "\" y2=\"" << zeroY << "\"/></g>\n"
        << "<g font-family=\"Arial\" font-size=\"11\" fill=\"#526079\">\n";

    for (int i = 0; i <= 10; ++i) {
        const double value = minimum + (maximum - minimum) * i / 10.0;
        const double x = kLeft + plotWidth * i / 10.0;
        const double y = kTop + plotHeight * i / 10.0;
        svg << "<text text-anchor=\"middle\" x=\"" << x << "\" y=\""
            << kHeight - 30 << "\">" << value << "</text>\n"
            << "<text text-anchor=\"end\" x=\"" << kLeft - 8 << "\" y=\""
            << y + 4 << "\">" << maximum - (maximum - minimum) * i / 10.0
            << "</text>\n";
    }

    svg << "</g>\n<g fill=\"none\" stroke=\"#2563eb\" stroke-width=\"2.5\" "
        << "stroke-linejoin=\"round\" stroke-linecap=\"round\">\n"
        << "<path d=\"";
    bool drawing = false;
    double previousY = 0.0;
    for (std::size_t i = 0; i < samples.size(); ++i) {
        const GraphSample& point = samples[i];
        if (!point.valid || (drawing &&
            std::fabs(point.y - previousY) > (maximum - minimum) * 0.35)) {
            drawing = false;
        }
        if (!point.valid) {
            continue;
        }
        svg << (drawing ? "L " : "M ") << mapX(point.x, minimum, maximum)
            << ' ' << mapY(point.y, minimum, maximum) << ' ';
        previousY = point.y;
        drawing = true;
    }
    svg << "\"/>\n</g>\n</svg>\n";
    return svg.str();
}

} // namespace

void writeSvg(const Expression& expression,
              const DynamicArray<GraphSample>& samples,
              double minimum,
              double maximum,
              const std::string& fileName) {
    std::ofstream output(fileName.c_str(), std::ios::out | std::ios::trunc);
    if (!output) {
        throw std::runtime_error("Could not write graph file: " + fileName);
    }
    output << makeSvg(expression, samples, minimum, maximum);
    if (!output) {
        throw std::runtime_error("An error occurred while writing: " + fileName);
    }
}

} // namespace ui
} // namespace graphx
