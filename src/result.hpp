#ifndef RESULT_HPP
#define RESULT_HPP

#include <ginac/ginac.h>
#include <string>
#include <map>

#include "componentlist.hpp"
#include "nodemap.hpp"
#include "command.hpp"
#include "simplification/weightmap.hpp"

struct quantity
{
    std::string function;
    std::string symbol;
};

class resultmap_t
{
    public:
        void add(const std::string& domain, const std::string& key, const GiNaC::ex& res)
        {
            container.insert(std::make_pair(domain + "$" + key, res));
        }

        bool exists(const std::string& domain, const std::string& key) const
        {
            auto it = container.find(domain + "$" + key);
            return it != container.end();
        }

        GiNaC::ex get(const std::string& domain, const std::string& key) const
        {
            auto it = container.find(domain + "$" + key);
            return it->second;
        }

    private:
        std::map<std::string, GiNaC::ex> container;
};

class result
{
    public:
        result();

        void add(const std::string& domain, const std::string& key, const GiNaC::ex& res)
        {
            resultmap.add(domain, key, res);
        }

        bool exists(const std::string& domain, const std::string& key)
        {
            return resultmap.exists(domain, key);
        }

        GiNaC::ex get(const std::string& domain, const std::string& key)
        {
            return resultmap.get(domain, key);
        }

        void print(const std::vector<command>& print_cmd, bool pretty, bool simpl) const;

        void set_weightmap(const weightmap_t& weightmap);

    private:
        resultmap_t resultmap;
        weightmap_t weightmap;
};

#endif // RESULT_HPP
