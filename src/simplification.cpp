#include "simplification.hpp"

/*
static bool is_simplify_line(const std::string& line)
{
    if(line.find(">>") != std::string::npos)
    {   
        return true;
    }
    else
    {
        return false;
    }
}

static bool is_level(const std::string& line)
{
    if(line.find("level") != std::string::npos)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//simplification
void save_simpl_line(const std::string& line)
{
    simplify_lines.push_back(line);
}
void set_simplification()
{
    for (unsigned int i = 0; i < simplify_lines.size(); i++)
    {
        std::string line = simplify_lines[i];
        std::stringstream stream (line);
        std::string first;
        std::string second;
        stream >> first;// s = R1
        stream >> second;// s = >>
        stream >> second;// s = R2
        simpl_commands.insert(std::make_pair(first,second)); 
    }
}
std::map<std::string, unsigned int > get_simplifications()
{
    return simpl_map;
}
bool is_simplification()
{
    return simplification;
}
void change_simpl_map(std::string greater, std::string smaller)
{
    auto it_first = simpl_map.find(greater);
    auto it_second= simpl_map.find(smaller);
    unsigned int val_second= it_second -> second;
    unsigned int val_first = val_second + 1;
    it_first -> second = val_first;
}
void set_simpl_level(const std::string& line)
{
    std::stringstream stream(line);
    std::string s;
    stream >> s ;// command "Simplify-Level"
    stream >> simpl_level;
}
std::string get_simpl_level()
{
    return simpl_level;
}
std::vector<unsigned int> get_simpl_vector()
{
    std::vector<unsigned int> ret;
    for (auto it = simpl_map.begin(); it != simpl_map.end(); ++it)
    {
        ret.push_back(it -> second);
    }
    return ret;
}
*/
