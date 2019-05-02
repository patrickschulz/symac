#include "html.hpp"

void write_html_header(std::ofstream& file)
{
    file << R"(<!DOCTYPE html>)" << '\n';
    file << R"(<html>)" << '\n';
    file << R"(<head>)" << '\n';
    file << R"(  <meta charset="utf-8">)" << '\n';
    file << R"(  <meta name="viewport" content="width=device-width">)" << '\n';
    file << R"(  <title>SymAC Report</title>)" << '\n';
    file << R"(  <script type="text/javascript" async)" << '\n';
    file << R"(  src="https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.5/MathJax.js?config=TeX-MML-AM_CHTML" async>)" << '\n';
    file << R"(</script>)" << '\n';
    file << R"(</head>)" << '\n';
    file << R"(<body>)" << '\n';
}

void write_html_footer(std::ofstream& file)
{
    file << R"(</body>)" << '\n';
    file << R"(</html>)" << '\n';
}
