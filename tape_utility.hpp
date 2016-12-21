#pragma once

#include <boost/spirit/include/qi.hpp>
#include <cstdlib>

namespace StarTape
{
    template <typename IteratorT>
    uint64_t parseOctal(IteratorT begin, IteratorT end)
    {
        namespace qi = boost::spirit::qi;
        uint64_t res = 0;
        qi::parse(begin, end, qi::oct, res);
        return res;
        //return std::strtoull(std::string{begin, end}.c_str(), nullptr, 8);
    }
}
