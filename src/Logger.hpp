#pragma once

#include <glm/glm.hpp>
#include <string>      // for std::string
#include <sstream>     // for std::ostringstream
#include <iomanip>     // for std::fixed and std::setprecision
#include <cctype>
#include <bitset>      // std::bitset
#include <vector>
#include <format>



namespace debug 
{ 
    // Take a string and go lowercase
    inline std::string toLowercase(const std::string string)
    {
        std::string s = string;  // copy first – fixes the "read-only variable" error
        for (char& c : s)
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        return s;
    }

    /* Turn vectors into strings */
    inline std::string str(const glm::vec4& vec, const int precision = -1)  
    {
        std::ostringstream oss;
        if (precision >= 0) {
            oss << std::fixed << std::setprecision(precision);
        }
        oss << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
        return oss.str();
    }

    inline std::string str(const glm::vec3& vec, const int precision = -1)  
    {
        std::ostringstream oss;
        if (precision >= 0) {
            oss << std::fixed << std::setprecision(precision);
        }
        oss << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
        return oss.str();
    }

    inline std::string str(const glm::dvec3& vec, const int precision = -1)  
    {
        std::ostringstream oss;
        if (precision >= 0) {
            oss << std::fixed << std::setprecision(precision);
        }
        oss << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
        return oss.str();
    }

    inline std::string str(const glm::vec2& vec, const int precision = -1)  
    {
        std::ostringstream oss;
        if (precision >= 0) {
            oss << std::fixed << std::setprecision(precision);
        }
        oss << "(" << vec.x << ", " << vec.y << ")";
        return oss.str();
    }

    inline std::string str(const glm::ivec4& vec)  
    {
        std::ostringstream oss;
        oss << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
        return oss.str();
    }

    inline std::string str(const glm::ivec3& vec)  
    {
        std::ostringstream oss;
        oss << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
        return oss.str();
    }

    inline std::string str(const glm::ivec2& vec)  
    {
        std::ostringstream oss;
        oss << "(" << vec.x << ", " << vec.y << ")";
        return oss.str();
    }
    
    /* Turn a boolean into a string */
    inline std::string str(const bool boolean)
    {
        std::ostringstream oss;
        oss << (boolean == true ? "True" : "False");
        return oss.str();
    }

    /* Turn numbers into a string */
    inline std::string str(const float number)
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << number;
        return oss.str();
    }

    inline std::string str(const double number)
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << number;
        return oss.str();
    }

    inline std::string str(const int number)
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << number;
        return oss.str();
    }

    inline std::string str(const uint64_t number)
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << number;
        return oss.str();
    }

    inline std::string bitStr(const float number)
    {
        std::bitset<32> bits(*reinterpret_cast<const uint32_t*>(&number));
        return bits.to_string();
    }

    inline std::string bitStr(const double number)
    {
        std::bitset<64> bits(*reinterpret_cast<const uint64_t*>(&number));
        return bits.to_string();
    }

    inline std::string bitStr(const int number)
    {
        std::bitset<32> bits(number);
        return bits.to_string();
    }

    inline std::string bitStr(const uint64_t number)
    {
        std::bitset<64> bits(number);
        return bits.to_string();
    }

    inline std::string str(const std::vector<bool>& vec)
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2);
        oss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            oss << str(vec[i]);
            if (i < vec.size() - 1) oss << ", ";
        }
        oss << "]";
        return oss.str();
    }

    inline std::string str(const std::vector<float>& vec)
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2);
        oss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            oss << vec[i];
            if (i < vec.size() - 1) oss << ", ";
        }
        oss << "]";
        return oss.str();
    }

    inline std::string str(const std::vector<double>& vec)
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2);
        oss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            oss << vec[i];
            if (i < vec.size() - 1) oss << ", ";
        }
        oss << "]";
        return oss.str();
    }

    inline std::string str(const std::vector<int>& vec)
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2);
        oss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            oss << vec[i];
            if (i < vec.size() - 1) oss << ", ";
        }
        oss << "]";
        return oss.str();
    }

    inline std::string str(const std::vector<uint32_t>& vec)
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2);
        oss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            oss << vec[i];
            if (i < vec.size() - 1) oss << ", ";
        }
        oss << "]";
        return oss.str();
    }

    inline std::string str(const std::vector<uint16_t>& vec)
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2);
        oss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            oss << vec[i];
            if (i < vec.size() - 1) oss << ", ";
        }
        oss << "]";
        return oss.str();
    }
}



#define LOG(fmt, ...) std::cout << std::format(fmt, ##__VA_ARGS__) << '\n'
#define FMT(fmt, ...) std::format(fmt, ##__VA_ARGS__).c_str()
#define STR(x)   debug::str(x)
#define STRP(x, prec)   debug::str(x, prec)