#include "Misc_functions.hpp"

std::vector<std::string> wrap_text(const std::string& text, int line_width)
{
    std::vector<std::string> lines = {""};
    std::string word = "";
    // scan all words and move them to next lines if they don't fit
    for (char ch : text)
    {
        if (ch == ' ' || ch == '\n') {
            if (lines.back().size() + word.size() > line_width) {
                while (word.size() > line_width)
                {   
                    if (lines.back().size() == 0) {
                        lines.back() += {word.begin(), word.begin()+line_width};
                    }
                    else {
                    lines.push_back({word.begin(), word.begin()+line_width});
                    }
                    word = {word.begin()+line_width, word.end()};
                }
                lines.push_back(word);
            }
            else {
                lines.back() += word;
            }

            if (ch == ' ' && lines.back().size() < line_width) {
                lines.back() += ' ';
            }
            else if (ch == '\n') {
                lines.push_back("");
            }
            word = "";
        }
        else {
            word += ch;
        }
    }
    // handle the last word
    if (lines.back().size() + word.size() > line_width)
        {
            while (word.size() > line_width)
            {
                if (lines.back().size() == 0) {
                    lines.back() += {word.begin(), word.begin()+line_width};
                }
                else {
                lines.push_back({word.begin(), word.begin()+line_width});
                }
                word = {word.begin()+line_width, word.end()};
            }
            lines.push_back(word);

        }
    else
    {
        lines.back() += word;
    }
    // remove whitespaces at the end of each line
    for (std::string& s : lines)
    {
        if(s.size() > 0 && s.back() == ' ')
        {
            s = {s.begin(), s.end()-1};
        }
    }
    return lines;
}