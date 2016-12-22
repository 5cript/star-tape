#pragma once

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/karma.hpp>
#include <cstdlib>
#include <iterator>
#include <algorithm>

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

    template <unsigned Width>
    std::string makeOctal(uint64_t number)
    {
        std::string res;
        boost::spirit::karma::generate(std::back_insert_iterator <std::string> (res),
            boost::spirit::maxwidth(Width)   [boost::spirit::oct(number)]
        );
        std::string padding;
        padding.resize(Width - res.length());
        std::fill(std::begin(padding), std::end(padding), '0');
        return padding + res;
    }
}
