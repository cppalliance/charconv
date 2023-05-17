// Copyright 2023 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/charconv.hpp>
#include <boost/core/lightweight_test.hpp>
#include <iostream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cmath>

template <typename T>
void spot_value(const std::string& buffer, T expected_value, boost::charconv::chars_format fmt = boost::charconv::chars_format::general)
{
    T v = 0;
    auto r = boost::charconv::from_chars(buffer.c_str(), buffer.c_str() + std::strlen(buffer.c_str()), v, fmt);
    BOOST_TEST_EQ(r.ec, 0);
    if (!BOOST_TEST_EQ(v, expected_value))
    {
        std::cerr << "Test failure for: " << buffer << " got: " << v << std::endl;
    }
}

template <typename T>
inline void spot_check(T expected_value, const std::string& buffer, boost::charconv::chars_format fmt)
{
    spot_value(buffer, expected_value, fmt);
}

void fc (const std::string& s)
{
    char* str_end;
    const double expected_value = std::strtod(s.c_str(), &str_end);
    spot_value(s, expected_value);
}

template <typename T>
void simple_integer_test()
{
    const char* buffer1 = "12";
    T v1 = 0;
    auto r1 = boost::charconv::from_chars(buffer1, buffer1 + std::strlen(buffer1), v1);
    BOOST_TEST_EQ(r1.ec, 0);
    BOOST_TEST_EQ(v1, static_cast<T>(12));

    const char* buffer2 = "1200";
    T v2 = 0;
    auto r2 = boost::charconv::from_chars(buffer2, buffer2 + std::strlen(buffer2), v2);
    BOOST_TEST_EQ(r2.ec, 0);
    BOOST_TEST_EQ(v2, static_cast<T>(1200));
}

template <typename T>
void simple_hex_integer_test()
{
    const char* buffer1 = "-2a";
    T v1 = 0;
    auto r1 = boost::charconv::from_chars(buffer1, buffer1 + std::strlen(buffer1), v1, boost::charconv::chars_format::hex);
    BOOST_TEST_EQ(r1.ec, 0);
    BOOST_TEST_EQ(v1, static_cast<T>(-42));
}

template <typename T>
void simple_scientific_test()
{
    const char* buffer1 = "1e1";
    T v1 = 0;
    auto r1 = boost::charconv::from_chars(buffer1, buffer1 + std::strlen(buffer1), v1);
    BOOST_TEST_EQ(r1.ec, 0);
    BOOST_TEST_EQ(v1, static_cast<T>(1e1L));

    const char* buffer2 = "123456789e10";
    T v2 = 0;
    auto r2 = boost::charconv::from_chars(buffer2, buffer2 + std::strlen(buffer2), v2);
    BOOST_TEST_EQ(r2.ec, 0);
    BOOST_TEST_EQ(v2, static_cast<T>(123456789e10L));

    const char* buffer3 = "1.23456789e+10";
    T v3 = 0;
    auto r3 = boost::charconv::from_chars(buffer3, buffer3 + std::strlen(buffer3), v3);
    BOOST_TEST_EQ(r3.ec, 0);
    BOOST_TEST_EQ(v3, static_cast<T>(1.23456789e+10L));

    const char* buffer4 = "1234.56789e+10";
    T v4 = 0;
    auto r4 = boost::charconv::from_chars(buffer4, buffer4 + std::strlen(buffer4), v4);
    BOOST_TEST_EQ(r4.ec, 0);
    BOOST_TEST_EQ(v4, static_cast<T>(1234.56789e+10L));

    const char* buffer5 = "+1234.56789e+10";
    auto v5 = static_cast<T>(3.0L);
    auto r5 = boost::charconv::from_chars(buffer5, buffer5 + std::strlen(buffer5), v5);
    BOOST_TEST_EQ(r5.ec, EINVAL);
    BOOST_TEST_EQ(v5, static_cast<T>(3.0L));
}

template <typename T>
void simple_hex_scientific_test()
{
    const char* buffer1 = "1.3a2bp-10";
    T v1 = 0;
    auto r1 = boost::charconv::from_chars(buffer1, buffer1 + std::strlen(buffer1), v1, boost::charconv::chars_format::hex);
    BOOST_TEST_EQ(r1.ec, 0);
    BOOST_TEST_EQ(v1, static_cast<T>(80427e-14L));

    const char* buffer2 = "1.234p-10";
    T v2 = 0;
    auto r2 = boost::charconv::from_chars(buffer2, buffer2 + std::strlen(buffer2), v2, boost::charconv::chars_format::hex);
    BOOST_TEST_EQ(r2.ec, 0);
    BOOST_TEST_EQ(v2, static_cast<T>(4660e-13L));
}

template <typename T>
void dot_position_test()
{
    const char* buffer1 = "11.11111111";
    T v1 = 0;
    auto r1 = boost::charconv::from_chars(buffer1, buffer1 + std::strlen(buffer1), v1);
    BOOST_TEST_EQ(r1.ec, 0);
    BOOST_TEST_EQ(v1, static_cast<T>(11.11111111L));

    const char* buffer2 = "1111.111111";
    T v2 = 0;
    auto r2 = boost::charconv::from_chars(buffer2, buffer2 + std::strlen(buffer2), v2);
    BOOST_TEST_EQ(r2.ec, 0);
    BOOST_TEST_EQ(v2, static_cast<T>(1111.111111L));

    const char* buffer3 = "111111.1111";
    T v3 = 0;
    auto r3 = boost::charconv::from_chars(buffer3, buffer3 + std::strlen(buffer3), v3);
    BOOST_TEST_EQ(r3.ec, 0);
    BOOST_TEST_EQ(v3, static_cast<T>(111111.1111L));

    const char* buffer4 = "1111111111.";
    T v4 = 0;
    auto r4 = boost::charconv::from_chars(buffer4, buffer4 + std::strlen(buffer4), v4);
    BOOST_TEST_EQ(r4.ec, 0);
    BOOST_TEST_EQ(v4, static_cast<T>(1111111111.L));
}

template <typename T>
void odd_strings_test()
{
    const char* buffer1 = "00000000000000000000000000000000000000000005";
    T v1 = 0;
    auto r1 = boost::charconv::from_chars(buffer1, buffer1 + std::strlen(buffer1), v1);
    BOOST_TEST_EQ(r1.ec, 0);
    BOOST_TEST_EQ(v1, static_cast<T>(5));

    const char* buffer2 = "123456789123456789123456789";
    T v2 = 0;
    auto r2 = boost::charconv::from_chars(buffer2, buffer2 + std::strlen(buffer2), v2);
    BOOST_TEST_EQ(r2.ec, 0);
    BOOST_TEST_EQ(v2, static_cast<T>(1.23456789123456789123456789e26L));

    const char* buffer3 = "100000000000000000000000e5";
    T v3 = 0;
    auto r3 = boost::charconv::from_chars(buffer3, buffer3 + std::strlen(buffer3), v3);
    BOOST_TEST_EQ(r3.ec, 0);
    BOOST_TEST_EQ(v3, static_cast<T>(100000000000000000000000e5L));

    const char* buffer4 = "1.23456789123456789123456789123456789123456789e-5";
    T v4 = 0;
    auto r4 = boost::charconv::from_chars(buffer4, buffer4 + std::strlen(buffer4), v4);
    BOOST_TEST_EQ(r4.ec, 0);
    BOOST_TEST_EQ(v4, static_cast<T>(1.23456789123456789123456789123456789123456789e-5L));

    const char* buffer5 = "1.23456789123456789123456789123456789123456789e-00000000000000000005";
    T v5 = 0;
    auto r5 = boost::charconv::from_chars(buffer5, buffer5 + std::strlen(buffer5), v5);
    BOOST_TEST_EQ(r5.ec, 0);
    BOOST_TEST_EQ(v5, static_cast<T>(1.23456789123456789123456789123456789123456789e-5L));

    const char* buffer6 = "E01";
    T v6 = 0;
    auto r6 = boost::charconv::from_chars(buffer6, buffer6 + std::strlen(buffer6), v6);
    BOOST_TEST_EQ(r6.ec, EINVAL);
}

template <typename T>
void zero_test()
{
    const char* buffer1 = "0e0";
    T v1 = 0;
    auto r1 = boost::charconv::from_chars(buffer1, buffer1 + std::strlen(buffer1), v1);
    BOOST_TEST_EQ(r1.ec, 0);
    BOOST_TEST_EQ(v1, static_cast<T>(0));
    BOOST_TEST(!std::signbit(v1));

    const char* buffer2 = "-0e0";
    T v2 = 0;
    auto r2 = boost::charconv::from_chars(buffer2, buffer2 + std::strlen(buffer2), v2);
    BOOST_TEST_EQ(r2.ec, 0);
    BOOST_TEST_EQ(v2, static_cast<T>(-0));
    BOOST_TEST(std::signbit(v2));

    const char* buffer3 = "0.0";
    T v3 = 0;
    auto r3 = boost::charconv::from_chars(buffer3, buffer3 + std::strlen(buffer3), v3);
    BOOST_TEST_EQ(r3.ec, 0);
    BOOST_TEST_EQ(v3, static_cast<T>(0.0));
    BOOST_TEST(!std::signbit(v3));

    const char* buffer4 = "-0.0";
    T v4 = 0;
    auto r4 = boost::charconv::from_chars(buffer4, buffer4 + std::strlen(buffer4), v4);
    BOOST_TEST_EQ(r4.ec, 0);
    BOOST_TEST_EQ(v4, static_cast<T>(-0));
    BOOST_TEST(std::signbit(v4));

    const char* buffer5 = "0";
    T v5 = 0;
    auto r5 = boost::charconv::from_chars(buffer5, buffer5 + std::strlen(buffer5), v5);
    BOOST_TEST_EQ(r5.ec, 0);
    BOOST_TEST_EQ(v5, static_cast<T>(0));
    BOOST_TEST(!std::signbit(v5));

    const char* buffer6 = "-0";
    T v6 = 0;
    auto r6 = boost::charconv::from_chars(buffer6, buffer6 + std::strlen(buffer6), v6);
    BOOST_TEST_EQ(r6.ec, 0);
    BOOST_TEST_EQ(v6, static_cast<T>(-0));
    BOOST_TEST(std::signbit(v6));
}

// See: https://github.com/boostorg/json/blob/develop/test/double.cpp#L243
template <typename T>
void boost_json_test()
{
    spot_value("-1.010", -1.01);
    spot_value("-0.010", -0.01);
    spot_value("-0.0", -0.0);
    spot_value("-0e0", -0.0);
    spot_value( "18.4",  18.4);
    spot_value("-18.4", -18.4);
    spot_value( "18446744073709551616",  1.8446744073709552e+19);
    spot_value("-18446744073709551616", -1.8446744073709552e+19);
    spot_value( "18446744073709551616.0",  1.8446744073709552e+19);
    spot_value( "18446744073709551616.00009",  1.8446744073709552e+19);
    spot_value( "1844674407370955161600000",  1.8446744073709552e+24);
    spot_value("-1844674407370955161600000", -1.8446744073709552e+24);
    spot_value( "1844674407370955161600000.0",  1.8446744073709552e+24);
    spot_value( "1844674407370955161600000.00009",  1.8446744073709552e+24);
    spot_value( "19700720435664.186294290058937593e13",  1.9700720435664185e+26);

    spot_value( "1.0", 1.0);
    spot_value( "1.1", 1.1);
    spot_value( "1.11", 1.11);
    spot_value( "1.11111", 1.11111);
    spot_value( "11.1111", 11.1111);
    spot_value( "111.111", 111.111);

    fc("-0.9999999999999999999999");
    fc("-0.9999999999999999");
    fc("-0.9007199254740991");
    fc("-0.999999999999999");
    fc("-0.99999999999999");
    fc("-0.9999999999999");
    fc("-0.999999999999");
    fc("-0.99999999999");
    fc("-0.9999999999");
    fc("-0.999999999");
    fc("-0.99999999");
    fc("-0.9999999");
    fc("-0.999999");
    fc("-0.99999");
    fc("-0.9999");
    fc("-0.8125");
    fc("-0.999");
    fc("-0.99");
    fc("-1.0");
    fc("-0.9");
    fc("-0.0");
    fc("0.0");
    fc("0.9");
    fc("0.99");
    fc("0.999");
    fc("0.8125");
    fc("0.9999");
    fc("0.99999");
    fc("0.999999");
    fc("0.9999999");
    fc("0.99999999");
    fc("0.999999999");
    fc("0.9999999999");
    fc("0.99999999999");
    fc("0.999999999999");
    fc("0.9999999999999");
    fc("0.99999999999999");
    fc("0.999999999999999");
    fc("0.9007199254740991");
    fc("0.9999999999999999");
    fc("0.9999999999999999999999");
    fc("0.999999999999999999999999999");

    fc("-1e308");
    fc("-1e-308");
    fc("-9999e300");
    fc("-999e100");
    fc("-99e10");
    fc("-9e1");
    fc("9e1");
    fc("99e10");
    fc("999e100");
    fc("9999e300");
    fc("999999999999999999.0");
    fc("999999999999999999999.0");
    fc("999999999999999999999e5");
    fc("999999999999999999999.0e5");

    fc("0.00000000000000001");

    fc("-1e-1");
    fc("-1e0");
    fc("-1e1");
    fc("0e0");
    fc("1e0");
    fc("1e10");

    fc("0."
       "00000000000000000000000000000000000000000000000000" // 50 zeroes
       "1e50");
    fc("-0."
       "00000000000000000000000000000000000000000000000000" // 50 zeroes
       "1e50");

    fc("0."
       "00000000000000000000000000000000000000000000000000"
       "00000000000000000000000000000000000000000000000000"
       "00000000000000000000000000000000000000000000000000"
       "00000000000000000000000000000000000000000000000000"
       "00000000000000000000000000000000000000000000000000"
       "00000000000000000000000000000000000000000000000000"
       "00000000000000000000000000000000000000000000000000"
       "00000000000000000000000000000000000000000000000000"
       "00000000000000000000000000000000000000000000000000"
       "00000000000000000000000000000000000000000000000000" // 500 zeroes
       "1e600");
    fc("-0."
       "00000000000000000000000000000000000000000000000000"
       "00000000000000000000000000000000000000000000000000"
       "00000000000000000000000000000000000000000000000000"
       "00000000000000000000000000000000000000000000000000"
       "00000000000000000000000000000000000000000000000000"
       "00000000000000000000000000000000000000000000000000"
       "00000000000000000000000000000000000000000000000000"
       "00000000000000000000000000000000000000000000000000"
       "00000000000000000000000000000000000000000000000000"
       "00000000000000000000000000000000000000000000000000" // 500 zeroes
       "1e600");

    fc("0e"
       "00000000000000000000000000000000000000000000000000"
       "00000000000000000000000000000000000000000000000000"
       "00000000000000000000000000000000000000000000000000"
       "00000000000000000000000000000000000000000000000000"
       "00000000000000000000000000000000000000000000000000"
       "00000000000000000000000000000000000000000000000000"
       "00000000000000000000000000000000000000000000000000"
       "00000000000000000000000000000000000000000000000000"
       "00000000000000000000000000000000000000000000000000"
       "00000000000000000000000000000000000000000000000000" // 500 zeroes
    );
}

int main()
{
    simple_integer_test<float>();
    simple_integer_test<double>();
    
    simple_hex_integer_test<float>();
    simple_hex_integer_test<double>();

    simple_scientific_test<float>();
    simple_scientific_test<double>();

    simple_hex_scientific_test<float>();
    simple_hex_scientific_test<double>();

    dot_position_test<float>();
    dot_position_test<double>();

    odd_strings_test<float>();
    odd_strings_test<double>();

    simple_integer_test<long double>();
    simple_hex_integer_test<long double>();
    simple_scientific_test<long double>();
    simple_hex_scientific_test<long double>();

    zero_test<float>();
    zero_test<double>();

    boost_json_test<double>();

    // Every power
    spot_check(1.7e+308, "1.7e+308", boost::charconv::chars_format::scientific);
    spot_check(1.7e+307, "1.7e+307", boost::charconv::chars_format::scientific);
    spot_check(1.7e+306, "1.7e+306", boost::charconv::chars_format::scientific);
    spot_check(1.7e+305, "1.7e+305", boost::charconv::chars_format::scientific);
    spot_check(1.7e+304, "1.7e+304", boost::charconv::chars_format::scientific);
    spot_check(1.7e+303, "1.7e+303", boost::charconv::chars_format::scientific);
    spot_check(1.7e+302, "1.7e+302", boost::charconv::chars_format::scientific);
    spot_check(1.7e+301, "1.7e+301", boost::charconv::chars_format::scientific);
    spot_check(1.7e+300, "1.7e+300", boost::charconv::chars_format::scientific);
    spot_check(1.7e+299, "1.7e+299", boost::charconv::chars_format::scientific);
    spot_check(1.7e+298, "1.7e+298", boost::charconv::chars_format::scientific);
    spot_check(1.7e+297, "1.7e+297", boost::charconv::chars_format::scientific);
    spot_check(1.7e+296, "1.7e+296", boost::charconv::chars_format::scientific);
    spot_check(1.7e+295, "1.7e+295", boost::charconv::chars_format::scientific);
    spot_check(1.7e+294, "1.7e+294", boost::charconv::chars_format::scientific);
    spot_check(1.7e+293, "1.7e+293", boost::charconv::chars_format::scientific);
    spot_check(1.7e+292, "1.7e+292", boost::charconv::chars_format::scientific);
    spot_check(1.7e+291, "1.7e+291", boost::charconv::chars_format::scientific);
    spot_check(1.7e+290, "1.7e+290", boost::charconv::chars_format::scientific);
    spot_check(1.7e+289, "1.7e+289", boost::charconv::chars_format::scientific);
    spot_check(1.7e+288, "1.7e+288", boost::charconv::chars_format::scientific);
    spot_check(1.7e+287, "1.7e+287", boost::charconv::chars_format::scientific);
    spot_check(1.7e+286, "1.7e+286", boost::charconv::chars_format::scientific);
    spot_check(1.7e+285, "1.7e+285", boost::charconv::chars_format::scientific);
    spot_check(1.7e+284, "1.7e+284", boost::charconv::chars_format::scientific);
    spot_check(1.7e+283, "1.7e+283", boost::charconv::chars_format::scientific);
    spot_check(1.7e+282, "1.7e+282", boost::charconv::chars_format::scientific);
    spot_check(1.7e+281, "1.7e+281", boost::charconv::chars_format::scientific);
    spot_check(1.7e+280, "1.7e+280", boost::charconv::chars_format::scientific);
    spot_check(1.7e+279, "1.7e+279", boost::charconv::chars_format::scientific);
    spot_check(1.7e+278, "1.7e+278", boost::charconv::chars_format::scientific);
    spot_check(1.7e+277, "1.7e+277", boost::charconv::chars_format::scientific);
    spot_check(1.7e+276, "1.7e+276", boost::charconv::chars_format::scientific);
    spot_check(1.7e+275, "1.7e+275", boost::charconv::chars_format::scientific);
    spot_check(1.7e+274, "1.7e+274", boost::charconv::chars_format::scientific);
    spot_check(1.7e+273, "1.7e+273", boost::charconv::chars_format::scientific);
    spot_check(1.7e+272, "1.7e+272", boost::charconv::chars_format::scientific);
    spot_check(1.7e+271, "1.7e+271", boost::charconv::chars_format::scientific);
    spot_check(1.7e+270, "1.7e+270", boost::charconv::chars_format::scientific);
    spot_check(1.7e+269, "1.7e+269", boost::charconv::chars_format::scientific);
    spot_check(1.7e+268, "1.7e+268", boost::charconv::chars_format::scientific);
    spot_check(1.7e+267, "1.7e+267", boost::charconv::chars_format::scientific);
    spot_check(1.7e+266, "1.7e+266", boost::charconv::chars_format::scientific);
    spot_check(1.7e+265, "1.7e+265", boost::charconv::chars_format::scientific);
    spot_check(1.7e+264, "1.7e+264", boost::charconv::chars_format::scientific);
    spot_check(1.7e+263, "1.7e+263", boost::charconv::chars_format::scientific);
    spot_check(1.7e+262, "1.7e+262", boost::charconv::chars_format::scientific);
    spot_check(1.7e+261, "1.7e+261", boost::charconv::chars_format::scientific);
    spot_check(1.7e+260, "1.7e+260", boost::charconv::chars_format::scientific);
    spot_check(1.7e+259, "1.7e+259", boost::charconv::chars_format::scientific);
    spot_check(1.7e+258, "1.7e+258", boost::charconv::chars_format::scientific);
    spot_check(1.7e+257, "1.7e+257", boost::charconv::chars_format::scientific);
    spot_check(1.7e+256, "1.7e+256", boost::charconv::chars_format::scientific);
    spot_check(1.7e+255, "1.7e+255", boost::charconv::chars_format::scientific);
    spot_check(1.7e+254, "1.7e+254", boost::charconv::chars_format::scientific);
    spot_check(1.7e+253, "1.7e+253", boost::charconv::chars_format::scientific);
    spot_check(1.7e+252, "1.7e+252", boost::charconv::chars_format::scientific);
    spot_check(1.7e+251, "1.7e+251", boost::charconv::chars_format::scientific);
    spot_check(1.7e+250, "1.7e+250", boost::charconv::chars_format::scientific);
    spot_check(1.7e+249, "1.7e+249", boost::charconv::chars_format::scientific);
    spot_check(1.7e+248, "1.7e+248", boost::charconv::chars_format::scientific);
    spot_check(1.7e+247, "1.7e+247", boost::charconv::chars_format::scientific);
    spot_check(1.7e+246, "1.7e+246", boost::charconv::chars_format::scientific);
    spot_check(1.7e+245, "1.7e+245", boost::charconv::chars_format::scientific);
    spot_check(1.7e+244, "1.7e+244", boost::charconv::chars_format::scientific);
    spot_check(1.7e+243, "1.7e+243", boost::charconv::chars_format::scientific);
    spot_check(1.7e+242, "1.7e+242", boost::charconv::chars_format::scientific);
    spot_check(1.7e+241, "1.7e+241", boost::charconv::chars_format::scientific);
    spot_check(1.7e+240, "1.7e+240", boost::charconv::chars_format::scientific);
    spot_check(1.7e+239, "1.7e+239", boost::charconv::chars_format::scientific);
    spot_check(1.7e+238, "1.7e+238", boost::charconv::chars_format::scientific);
    spot_check(1.7e+237, "1.7e+237", boost::charconv::chars_format::scientific);
    spot_check(1.7e+236, "1.7e+236", boost::charconv::chars_format::scientific);
    spot_check(1.7e+235, "1.7e+235", boost::charconv::chars_format::scientific);
    spot_check(1.7e+234, "1.7e+234", boost::charconv::chars_format::scientific);
    spot_check(1.7e+233, "1.7e+233", boost::charconv::chars_format::scientific);
    spot_check(1.7e+232, "1.7e+232", boost::charconv::chars_format::scientific);
    spot_check(1.7e+231, "1.7e+231", boost::charconv::chars_format::scientific);
    spot_check(1.7e+230, "1.7e+230", boost::charconv::chars_format::scientific);
    spot_check(1.7e+229, "1.7e+229", boost::charconv::chars_format::scientific);
    spot_check(1.7e+228, "1.7e+228", boost::charconv::chars_format::scientific);
    spot_check(1.7e+227, "1.7e+227", boost::charconv::chars_format::scientific);
    spot_check(1.7e+226, "1.7e+226", boost::charconv::chars_format::scientific);
    spot_check(1.7e+225, "1.7e+225", boost::charconv::chars_format::scientific);
    spot_check(1.7e+224, "1.7e+224", boost::charconv::chars_format::scientific);
    spot_check(1.7e+223, "1.7e+223", boost::charconv::chars_format::scientific);
    spot_check(1.7e+222, "1.7e+222", boost::charconv::chars_format::scientific);
    spot_check(1.7e+221, "1.7e+221", boost::charconv::chars_format::scientific);
    spot_check(1.7e+220, "1.7e+220", boost::charconv::chars_format::scientific);
    spot_check(1.7e+219, "1.7e+219", boost::charconv::chars_format::scientific);
    spot_check(1.7e+218, "1.7e+218", boost::charconv::chars_format::scientific);
    spot_check(1.7e+217, "1.7e+217", boost::charconv::chars_format::scientific);
    spot_check(1.7e+216, "1.7e+216", boost::charconv::chars_format::scientific);
    spot_check(1.7e+215, "1.7e+215", boost::charconv::chars_format::scientific);
    spot_check(1.7e+214, "1.7e+214", boost::charconv::chars_format::scientific);
    spot_check(1.7e+213, "1.7e+213", boost::charconv::chars_format::scientific);
    spot_check(1.7e+212, "1.7e+212", boost::charconv::chars_format::scientific);
    spot_check(1.7e+211, "1.7e+211", boost::charconv::chars_format::scientific);
    spot_check(1.7e+210, "1.7e+210", boost::charconv::chars_format::scientific);
    spot_check(1.7e+209, "1.7e+209", boost::charconv::chars_format::scientific);
    spot_check(1.7e+208, "1.7e+208", boost::charconv::chars_format::scientific);
    spot_check(1.7e+207, "1.7e+207", boost::charconv::chars_format::scientific);
    spot_check(1.7e+206, "1.7e+206", boost::charconv::chars_format::scientific);
    spot_check(1.7e+205, "1.7e+205", boost::charconv::chars_format::scientific);
    spot_check(1.7e+204, "1.7e+204", boost::charconv::chars_format::scientific);
    spot_check(1.7e+203, "1.7e+203", boost::charconv::chars_format::scientific);
    spot_check(1.7e+202, "1.7e+202", boost::charconv::chars_format::scientific);
    spot_check(1.7e+201, "1.7e+201", boost::charconv::chars_format::scientific);
    spot_check(1.7e+200, "1.7e+200", boost::charconv::chars_format::scientific);

    spot_check(1.7e+199, "1.7e+199", boost::charconv::chars_format::scientific);
    spot_check(1.7e+198, "1.7e+198", boost::charconv::chars_format::scientific);
    spot_check(1.7e+197, "1.7e+197", boost::charconv::chars_format::scientific);
    spot_check(1.7e+196, "1.7e+196", boost::charconv::chars_format::scientific);
    spot_check(1.7e+195, "1.7e+195", boost::charconv::chars_format::scientific);
    spot_check(1.7e+194, "1.7e+194", boost::charconv::chars_format::scientific);
    spot_check(1.7e+193, "1.7e+193", boost::charconv::chars_format::scientific);
    spot_check(1.7e+192, "1.7e+192", boost::charconv::chars_format::scientific);
    spot_check(1.7e+191, "1.7e+191", boost::charconv::chars_format::scientific);
    spot_check(1.7e+190, "1.7e+190", boost::charconv::chars_format::scientific);
    spot_check(1.7e+189, "1.7e+189", boost::charconv::chars_format::scientific);
    spot_check(1.7e+188, "1.7e+188", boost::charconv::chars_format::scientific);
    spot_check(1.7e+187, "1.7e+187", boost::charconv::chars_format::scientific);
    spot_check(1.7e+186, "1.7e+186", boost::charconv::chars_format::scientific);
    spot_check(1.7e+185, "1.7e+185", boost::charconv::chars_format::scientific);
    spot_check(1.7e+184, "1.7e+184", boost::charconv::chars_format::scientific);
    spot_check(1.7e+183, "1.7e+183", boost::charconv::chars_format::scientific);
    spot_check(1.7e+182, "1.7e+182", boost::charconv::chars_format::scientific);
    spot_check(1.7e+181, "1.7e+181", boost::charconv::chars_format::scientific);
    spot_check(1.7e+180, "1.7e+180", boost::charconv::chars_format::scientific);
    spot_check(1.7e+179, "1.7e+179", boost::charconv::chars_format::scientific);
    spot_check(1.7e+178, "1.7e+178", boost::charconv::chars_format::scientific);
    spot_check(1.7e+177, "1.7e+177", boost::charconv::chars_format::scientific);
    spot_check(1.7e+176, "1.7e+176", boost::charconv::chars_format::scientific);
    spot_check(1.7e+175, "1.7e+175", boost::charconv::chars_format::scientific);
    spot_check(1.7e+174, "1.7e+174", boost::charconv::chars_format::scientific);
    spot_check(1.7e+173, "1.7e+173", boost::charconv::chars_format::scientific);
    spot_check(1.7e+172, "1.7e+172", boost::charconv::chars_format::scientific);
    spot_check(1.7e+171, "1.7e+171", boost::charconv::chars_format::scientific);
    spot_check(1.7e+170, "1.7e+170", boost::charconv::chars_format::scientific);
    spot_check(1.7e+169, "1.7e+169", boost::charconv::chars_format::scientific);
    spot_check(1.7e+168, "1.7e+168", boost::charconv::chars_format::scientific);
    spot_check(1.7e+167, "1.7e+167", boost::charconv::chars_format::scientific);
    spot_check(1.7e+166, "1.7e+166", boost::charconv::chars_format::scientific);
    spot_check(1.7e+165, "1.7e+165", boost::charconv::chars_format::scientific);
    spot_check(1.7e+164, "1.7e+164", boost::charconv::chars_format::scientific);
    spot_check(1.7e+163, "1.7e+163", boost::charconv::chars_format::scientific);
    spot_check(1.7e+162, "1.7e+162", boost::charconv::chars_format::scientific);
    spot_check(1.7e+161, "1.7e+161", boost::charconv::chars_format::scientific);
    spot_check(1.7e+160, "1.7e+160", boost::charconv::chars_format::scientific);
    spot_check(1.7e+159, "1.7e+159", boost::charconv::chars_format::scientific);
    spot_check(1.7e+158, "1.7e+158", boost::charconv::chars_format::scientific);
    spot_check(1.7e+157, "1.7e+157", boost::charconv::chars_format::scientific);
    spot_check(1.7e+156, "1.7e+156", boost::charconv::chars_format::scientific);
    spot_check(1.7e+155, "1.7e+155", boost::charconv::chars_format::scientific);
    spot_check(1.7e+154, "1.7e+154", boost::charconv::chars_format::scientific);
    spot_check(1.7e+153, "1.7e+153", boost::charconv::chars_format::scientific);
    spot_check(1.7e+152, "1.7e+152", boost::charconv::chars_format::scientific);
    spot_check(1.7e+151, "1.7e+151", boost::charconv::chars_format::scientific);
    spot_check(1.7e+150, "1.7e+150", boost::charconv::chars_format::scientific);
    spot_check(1.7e+149, "1.7e+149", boost::charconv::chars_format::scientific);
    spot_check(1.7e+148, "1.7e+148", boost::charconv::chars_format::scientific);
    spot_check(1.7e+147, "1.7e+147", boost::charconv::chars_format::scientific);
    spot_check(1.7e+146, "1.7e+146", boost::charconv::chars_format::scientific);
    spot_check(1.7e+145, "1.7e+145", boost::charconv::chars_format::scientific);
    spot_check(1.7e+144, "1.7e+144", boost::charconv::chars_format::scientific);
    spot_check(1.7e+143, "1.7e+143", boost::charconv::chars_format::scientific);
    spot_check(1.7e+142, "1.7e+142", boost::charconv::chars_format::scientific);
    spot_check(1.7e+141, "1.7e+141", boost::charconv::chars_format::scientific);
    spot_check(1.7e+140, "1.7e+140", boost::charconv::chars_format::scientific);
    spot_check(1.7e+139, "1.7e+139", boost::charconv::chars_format::scientific);
    spot_check(1.7e+138, "1.7e+138", boost::charconv::chars_format::scientific);
    spot_check(1.7e+137, "1.7e+137", boost::charconv::chars_format::scientific);
    spot_check(1.7e+136, "1.7e+136", boost::charconv::chars_format::scientific);
    spot_check(1.7e+135, "1.7e+135", boost::charconv::chars_format::scientific);
    spot_check(1.7e+134, "1.7e+134", boost::charconv::chars_format::scientific);
    spot_check(1.7e+133, "1.7e+133", boost::charconv::chars_format::scientific);
    spot_check(1.7e+132, "1.7e+132", boost::charconv::chars_format::scientific);
    spot_check(1.7e+131, "1.7e+131", boost::charconv::chars_format::scientific);
    spot_check(1.7e+130, "1.7e+130", boost::charconv::chars_format::scientific);
    spot_check(1.7e+129, "1.7e+129", boost::charconv::chars_format::scientific);
    spot_check(1.7e+128, "1.7e+128", boost::charconv::chars_format::scientific);
    spot_check(1.7e+127, "1.7e+127", boost::charconv::chars_format::scientific);
    spot_check(1.7e+126, "1.7e+126", boost::charconv::chars_format::scientific);
    spot_check(1.7e+125, "1.7e+125", boost::charconv::chars_format::scientific);
    spot_check(1.7e+124, "1.7e+124", boost::charconv::chars_format::scientific);
    spot_check(1.7e+123, "1.7e+123", boost::charconv::chars_format::scientific);
    spot_check(1.7e+122, "1.7e+122", boost::charconv::chars_format::scientific);
    spot_check(1.7e+121, "1.7e+121", boost::charconv::chars_format::scientific);
    spot_check(1.7e+120, "1.7e+120", boost::charconv::chars_format::scientific);
    spot_check(1.7e+119, "1.7e+119", boost::charconv::chars_format::scientific);
    spot_check(1.7e+118, "1.7e+118", boost::charconv::chars_format::scientific);
    spot_check(1.7e+117, "1.7e+117", boost::charconv::chars_format::scientific);
    spot_check(1.7e+116, "1.7e+116", boost::charconv::chars_format::scientific);
    spot_check(1.7e+115, "1.7e+115", boost::charconv::chars_format::scientific);
    spot_check(1.7e+114, "1.7e+114", boost::charconv::chars_format::scientific);
    spot_check(1.7e+113, "1.7e+113", boost::charconv::chars_format::scientific);
    spot_check(1.7e+112, "1.7e+112", boost::charconv::chars_format::scientific);
    spot_check(1.7e+111, "1.7e+111", boost::charconv::chars_format::scientific);
    spot_check(1.7e+110, "1.7e+110", boost::charconv::chars_format::scientific);
    spot_check(1.7e+109, "1.7e+109", boost::charconv::chars_format::scientific);
    spot_check(1.7e+108, "1.7e+108", boost::charconv::chars_format::scientific);
    spot_check(1.7e+107, "1.7e+107", boost::charconv::chars_format::scientific);
    spot_check(1.7e+106, "1.7e+106", boost::charconv::chars_format::scientific);
    spot_check(1.7e+105, "1.7e+105", boost::charconv::chars_format::scientific);
    spot_check(1.7e+104, "1.7e+104", boost::charconv::chars_format::scientific);
    spot_check(1.7e+103, "1.7e+103", boost::charconv::chars_format::scientific);
    spot_check(1.7e+102, "1.7e+102", boost::charconv::chars_format::scientific);
    spot_check(1.7e+101, "1.7e+101", boost::charconv::chars_format::scientific);
    spot_check(1.7e+100, "1.7e+100", boost::charconv::chars_format::scientific);

    spot_check(1.7e+99, "1.7e+99", boost::charconv::chars_format::scientific);
    spot_check(1.7e+98, "1.7e+98", boost::charconv::chars_format::scientific);
    spot_check(1.7e+97, "1.7e+97", boost::charconv::chars_format::scientific);
    spot_check(1.7e+96, "1.7e+96", boost::charconv::chars_format::scientific);
    spot_check(1.7e+95, "1.7e+95", boost::charconv::chars_format::scientific);
    spot_check(1.7e+94, "1.7e+94", boost::charconv::chars_format::scientific);
    spot_check(1.7e+93, "1.7e+93", boost::charconv::chars_format::scientific);
    spot_check(1.7e+92, "1.7e+92", boost::charconv::chars_format::scientific);
    spot_check(1.7e+91, "1.7e+91", boost::charconv::chars_format::scientific);
    spot_check(1.7e+90, "1.7e+90", boost::charconv::chars_format::scientific);
    spot_check(1.7e+89, "1.7e+89", boost::charconv::chars_format::scientific);
    spot_check(1.7e+88, "1.7e+88", boost::charconv::chars_format::scientific);
    spot_check(1.7e+87, "1.7e+87", boost::charconv::chars_format::scientific);
    spot_check(1.7e+86, "1.7e+86", boost::charconv::chars_format::scientific);
    spot_check(1.7e+85, "1.7e+85", boost::charconv::chars_format::scientific);
    spot_check(1.7e+84, "1.7e+84", boost::charconv::chars_format::scientific);
    spot_check(1.7e+83, "1.7e+83", boost::charconv::chars_format::scientific);
    spot_check(1.7e+82, "1.7e+82", boost::charconv::chars_format::scientific);
    spot_check(1.7e+81, "1.7e+81", boost::charconv::chars_format::scientific);
    spot_check(1.7e+80, "1.7e+80", boost::charconv::chars_format::scientific);
    spot_check(1.7e+79, "1.7e+79", boost::charconv::chars_format::scientific);
    spot_check(1.7e+78, "1.7e+78", boost::charconv::chars_format::scientific);
    spot_check(1.7e+77, "1.7e+77", boost::charconv::chars_format::scientific);
    spot_check(1.7e+76, "1.7e+76", boost::charconv::chars_format::scientific);
    spot_check(1.7e+75, "1.7e+75", boost::charconv::chars_format::scientific);
    spot_check(1.7e+74, "1.7e+74", boost::charconv::chars_format::scientific);
    spot_check(1.7e+73, "1.7e+73", boost::charconv::chars_format::scientific);
    spot_check(1.7e+72, "1.7e+72", boost::charconv::chars_format::scientific);
    spot_check(1.7e+71, "1.7e+71", boost::charconv::chars_format::scientific);
    spot_check(1.7e+70, "1.7e+70", boost::charconv::chars_format::scientific);
    spot_check(1.7e+69, "1.7e+69", boost::charconv::chars_format::scientific);
    spot_check(1.7e+68, "1.7e+68", boost::charconv::chars_format::scientific);
    spot_check(1.7e+67, "1.7e+67", boost::charconv::chars_format::scientific);
    spot_check(1.7e+66, "1.7e+66", boost::charconv::chars_format::scientific);
    spot_check(1.7e+65, "1.7e+65", boost::charconv::chars_format::scientific);
    spot_check(1.7e+64, "1.7e+64", boost::charconv::chars_format::scientific);
    spot_check(1.7e+63, "1.7e+63", boost::charconv::chars_format::scientific);
    spot_check(1.7e+62, "1.7e+62", boost::charconv::chars_format::scientific);
    spot_check(1.7e+61, "1.7e+61", boost::charconv::chars_format::scientific);
    spot_check(1.7e+60, "1.7e+60", boost::charconv::chars_format::scientific);
    spot_check(1.7e+59, "1.7e+59", boost::charconv::chars_format::scientific);
    spot_check(1.7e+58, "1.7e+58", boost::charconv::chars_format::scientific);
    spot_check(1.7e+57, "1.7e+57", boost::charconv::chars_format::scientific);
    spot_check(1.7e+56, "1.7e+56", boost::charconv::chars_format::scientific);
    spot_check(1.7e+55, "1.7e+55", boost::charconv::chars_format::scientific);
    spot_check(1.7e+54, "1.7e+54", boost::charconv::chars_format::scientific);
    spot_check(1.7e+53, "1.7e+53", boost::charconv::chars_format::scientific);
    spot_check(1.7e+52, "1.7e+52", boost::charconv::chars_format::scientific);
    spot_check(1.7e+51, "1.7e+51", boost::charconv::chars_format::scientific);
    spot_check(1.7e+50, "1.7e+50", boost::charconv::chars_format::scientific);
    spot_check(1.7e+49, "1.7e+49", boost::charconv::chars_format::scientific);
    spot_check(1.7e+48, "1.7e+48", boost::charconv::chars_format::scientific);
    spot_check(1.7e+47, "1.7e+47", boost::charconv::chars_format::scientific);
    spot_check(1.7e+46, "1.7e+46", boost::charconv::chars_format::scientific);
    spot_check(1.7e+45, "1.7e+45", boost::charconv::chars_format::scientific);
    spot_check(1.7e+44, "1.7e+44", boost::charconv::chars_format::scientific);
    spot_check(1.7e+43, "1.7e+43", boost::charconv::chars_format::scientific);
    spot_check(1.7e+42, "1.7e+42", boost::charconv::chars_format::scientific);
    spot_check(1.7e+41, "1.7e+41", boost::charconv::chars_format::scientific);
    spot_check(1.7e+40, "1.7e+40", boost::charconv::chars_format::scientific);
    spot_check(1.7e+39, "1.7e+39", boost::charconv::chars_format::scientific);
    spot_check(1.7e+38, "1.7e+38", boost::charconv::chars_format::scientific);
    spot_check(1.7e+37, "1.7e+37", boost::charconv::chars_format::scientific);
    spot_check(1.7e+36, "1.7e+36", boost::charconv::chars_format::scientific);
    spot_check(1.7e+35, "1.7e+35", boost::charconv::chars_format::scientific);
    spot_check(1.7e+34, "1.7e+34", boost::charconv::chars_format::scientific);
    spot_check(1.7e+33, "1.7e+33", boost::charconv::chars_format::scientific);
    spot_check(1.7e+32, "1.7e+32", boost::charconv::chars_format::scientific);
    spot_check(1.7e+31, "1.7e+31", boost::charconv::chars_format::scientific);
    spot_check(1.7e+30, "1.7e+30", boost::charconv::chars_format::scientific);
    spot_check(1.7e+29, "1.7e+29", boost::charconv::chars_format::scientific);
    spot_check(1.7e+28, "1.7e+28", boost::charconv::chars_format::scientific);
    spot_check(1.7e+27, "1.7e+27", boost::charconv::chars_format::scientific);
    spot_check(1.7e+26, "1.7e+26", boost::charconv::chars_format::scientific);
    spot_check(1.7e+25, "1.7e+25", boost::charconv::chars_format::scientific);
    spot_check(1.7e+24, "1.7e+24", boost::charconv::chars_format::scientific);
    spot_check(1.7e+23, "1.7e+23", boost::charconv::chars_format::scientific);
    spot_check(1.7e+22, "1.7e+22", boost::charconv::chars_format::scientific);
    spot_check(1.7e+21, "1.7e+21", boost::charconv::chars_format::scientific);
    spot_check(1.7e+20, "1.7e+20", boost::charconv::chars_format::scientific);
    spot_check(1.7e+19, "1.7e+19", boost::charconv::chars_format::scientific);
    spot_check(1.7e+18, "1.7e+18", boost::charconv::chars_format::scientific);
    spot_check(1.7e+17, "1.7e+17", boost::charconv::chars_format::scientific);
    spot_check(1.7e+16, "1.7e+16", boost::charconv::chars_format::scientific);
    spot_check(1.7e+15, "1.7e+15", boost::charconv::chars_format::scientific);
    spot_check(1.7e+14, "1.7e+14", boost::charconv::chars_format::scientific);
    spot_check(1.7e+13, "1.7e+13", boost::charconv::chars_format::scientific);
    spot_check(1.7e+12, "1.7e+12", boost::charconv::chars_format::scientific);
    spot_check(1.7e+11, "1.7e+11", boost::charconv::chars_format::scientific);
    spot_check(1.7e+10, "1.7e+10", boost::charconv::chars_format::scientific);
    spot_check(1.7e+09, "1.7e+09", boost::charconv::chars_format::scientific);
    spot_check(1.7e+08, "1.7e+08", boost::charconv::chars_format::scientific);
    spot_check(1.7e+07, "1.7e+07", boost::charconv::chars_format::scientific);
    spot_check(1.7e+06, "1.7e+06", boost::charconv::chars_format::scientific);
    spot_check(1.7e+05, "1.7e+05", boost::charconv::chars_format::scientific);
    spot_check(1.7e+04, "1.7e+04", boost::charconv::chars_format::scientific);
    spot_check(1.7e+03, "1.7e+03", boost::charconv::chars_format::scientific);
    spot_check(1.7e+02, "1.7e+02", boost::charconv::chars_format::scientific);
    spot_check(1.7e+01, "1.7e+01", boost::charconv::chars_format::scientific);
    spot_check(1.7e+00, "1.7e+00", boost::charconv::chars_format::scientific);

    spot_check(1.7e-308, "1.7e-308", boost::charconv::chars_format::scientific);
    spot_check(1.7e-307, "1.7e-307", boost::charconv::chars_format::scientific);
    spot_check(1.7e-306, "1.7e-306", boost::charconv::chars_format::scientific);
    spot_check(1.7e-305, "1.7e-305", boost::charconv::chars_format::scientific);
    spot_check(1.7e-304, "1.7e-304", boost::charconv::chars_format::scientific);
    spot_check(1.7e-303, "1.7e-303", boost::charconv::chars_format::scientific);
    spot_check(1.7e-302, "1.7e-302", boost::charconv::chars_format::scientific);
    spot_check(1.7e-301, "1.7e-301", boost::charconv::chars_format::scientific);
    spot_check(1.7e-300, "1.7e-300", boost::charconv::chars_format::scientific);
    spot_check(1.7e-299, "1.7e-299", boost::charconv::chars_format::scientific);
    spot_check(1.7e-298, "1.7e-298", boost::charconv::chars_format::scientific);
    spot_check(1.7e-297, "1.7e-297", boost::charconv::chars_format::scientific);
    spot_check(1.7e-296, "1.7e-296", boost::charconv::chars_format::scientific);
    spot_check(1.7e-295, "1.7e-295", boost::charconv::chars_format::scientific);
    spot_check(1.7e-294, "1.7e-294", boost::charconv::chars_format::scientific);
    spot_check(1.7e-293, "1.7e-293", boost::charconv::chars_format::scientific);
    spot_check(1.7e-292, "1.7e-292", boost::charconv::chars_format::scientific);
    spot_check(1.7e-291, "1.7e-291", boost::charconv::chars_format::scientific);
    spot_check(1.7e-290, "1.7e-290", boost::charconv::chars_format::scientific);
    spot_check(1.7e-289, "1.7e-289", boost::charconv::chars_format::scientific);
    spot_check(1.7e-288, "1.7e-288", boost::charconv::chars_format::scientific);
    spot_check(1.7e-287, "1.7e-287", boost::charconv::chars_format::scientific);
    spot_check(1.7e-286, "1.7e-286", boost::charconv::chars_format::scientific);
    spot_check(1.7e-285, "1.7e-285", boost::charconv::chars_format::scientific);
    spot_check(1.7e-284, "1.7e-284", boost::charconv::chars_format::scientific);
    spot_check(1.7e-283, "1.7e-283", boost::charconv::chars_format::scientific);
    spot_check(1.7e-282, "1.7e-282", boost::charconv::chars_format::scientific);
    spot_check(1.7e-281, "1.7e-281", boost::charconv::chars_format::scientific);
    spot_check(1.7e-280, "1.7e-280", boost::charconv::chars_format::scientific);
    spot_check(1.7e-279, "1.7e-279", boost::charconv::chars_format::scientific);
    spot_check(1.7e-278, "1.7e-278", boost::charconv::chars_format::scientific);
    spot_check(1.7e-277, "1.7e-277", boost::charconv::chars_format::scientific);
    spot_check(1.7e-276, "1.7e-276", boost::charconv::chars_format::scientific);
    spot_check(1.7e-275, "1.7e-275", boost::charconv::chars_format::scientific);
    spot_check(1.7e-274, "1.7e-274", boost::charconv::chars_format::scientific);
    spot_check(1.7e-273, "1.7e-273", boost::charconv::chars_format::scientific);
    spot_check(1.7e-272, "1.7e-272", boost::charconv::chars_format::scientific);
    spot_check(1.7e-271, "1.7e-271", boost::charconv::chars_format::scientific);
    spot_check(1.7e-270, "1.7e-270", boost::charconv::chars_format::scientific);
    spot_check(1.7e-269, "1.7e-269", boost::charconv::chars_format::scientific);
    spot_check(1.7e-268, "1.7e-268", boost::charconv::chars_format::scientific);
    spot_check(1.7e-267, "1.7e-267", boost::charconv::chars_format::scientific);
    spot_check(1.7e-266, "1.7e-266", boost::charconv::chars_format::scientific);
    spot_check(1.7e-265, "1.7e-265", boost::charconv::chars_format::scientific);
    spot_check(1.7e-264, "1.7e-264", boost::charconv::chars_format::scientific);
    spot_check(1.7e-263, "1.7e-263", boost::charconv::chars_format::scientific);
    spot_check(1.7e-262, "1.7e-262", boost::charconv::chars_format::scientific);
    spot_check(1.7e-261, "1.7e-261", boost::charconv::chars_format::scientific);
    spot_check(1.7e-260, "1.7e-260", boost::charconv::chars_format::scientific);
    spot_check(1.7e-259, "1.7e-259", boost::charconv::chars_format::scientific);
    spot_check(1.7e-258, "1.7e-258", boost::charconv::chars_format::scientific);
    spot_check(1.7e-257, "1.7e-257", boost::charconv::chars_format::scientific);
    spot_check(1.7e-256, "1.7e-256", boost::charconv::chars_format::scientific);
    spot_check(1.7e-255, "1.7e-255", boost::charconv::chars_format::scientific);
    spot_check(1.7e-254, "1.7e-254", boost::charconv::chars_format::scientific);
    spot_check(1.7e-253, "1.7e-253", boost::charconv::chars_format::scientific);
    spot_check(1.7e-252, "1.7e-252", boost::charconv::chars_format::scientific);
    spot_check(1.7e-251, "1.7e-251", boost::charconv::chars_format::scientific);
    spot_check(1.7e-250, "1.7e-250", boost::charconv::chars_format::scientific);
    spot_check(1.7e-249, "1.7e-249", boost::charconv::chars_format::scientific);
    spot_check(1.7e-248, "1.7e-248", boost::charconv::chars_format::scientific);
    spot_check(1.7e-247, "1.7e-247", boost::charconv::chars_format::scientific);
    spot_check(1.7e-246, "1.7e-246", boost::charconv::chars_format::scientific);
    spot_check(1.7e-245, "1.7e-245", boost::charconv::chars_format::scientific);
    spot_check(1.7e-244, "1.7e-244", boost::charconv::chars_format::scientific);
    spot_check(1.7e-243, "1.7e-243", boost::charconv::chars_format::scientific);
    spot_check(1.7e-242, "1.7e-242", boost::charconv::chars_format::scientific);
    spot_check(1.7e-241, "1.7e-241", boost::charconv::chars_format::scientific);
    spot_check(1.7e-240, "1.7e-240", boost::charconv::chars_format::scientific);
    spot_check(1.7e-239, "1.7e-239", boost::charconv::chars_format::scientific);
    spot_check(1.7e-238, "1.7e-238", boost::charconv::chars_format::scientific);
    spot_check(1.7e-237, "1.7e-237", boost::charconv::chars_format::scientific);
    spot_check(1.7e-236, "1.7e-236", boost::charconv::chars_format::scientific);
    spot_check(1.7e-235, "1.7e-235", boost::charconv::chars_format::scientific);
    spot_check(1.7e-234, "1.7e-234", boost::charconv::chars_format::scientific);
    spot_check(1.7e-233, "1.7e-233", boost::charconv::chars_format::scientific);
    spot_check(1.7e-232, "1.7e-232", boost::charconv::chars_format::scientific);
    spot_check(1.7e-231, "1.7e-231", boost::charconv::chars_format::scientific);
    spot_check(1.7e-230, "1.7e-230", boost::charconv::chars_format::scientific);
    spot_check(1.7e-229, "1.7e-229", boost::charconv::chars_format::scientific);
    spot_check(1.7e-228, "1.7e-228", boost::charconv::chars_format::scientific);
    spot_check(1.7e-227, "1.7e-227", boost::charconv::chars_format::scientific);
    spot_check(1.7e-226, "1.7e-226", boost::charconv::chars_format::scientific);
    spot_check(1.7e-225, "1.7e-225", boost::charconv::chars_format::scientific);
    spot_check(1.7e-224, "1.7e-224", boost::charconv::chars_format::scientific);
    spot_check(1.7e-223, "1.7e-223", boost::charconv::chars_format::scientific);
    spot_check(1.7e-222, "1.7e-222", boost::charconv::chars_format::scientific);
    spot_check(1.7e-221, "1.7e-221", boost::charconv::chars_format::scientific);
    spot_check(1.7e-220, "1.7e-220", boost::charconv::chars_format::scientific);
    spot_check(1.7e-219, "1.7e-219", boost::charconv::chars_format::scientific);
    spot_check(1.7e-218, "1.7e-218", boost::charconv::chars_format::scientific);
    spot_check(1.7e-217, "1.7e-217", boost::charconv::chars_format::scientific);
    spot_check(1.7e-216, "1.7e-216", boost::charconv::chars_format::scientific);
    spot_check(1.7e-215, "1.7e-215", boost::charconv::chars_format::scientific);
    spot_check(1.7e-214, "1.7e-214", boost::charconv::chars_format::scientific);
    spot_check(1.7e-213, "1.7e-213", boost::charconv::chars_format::scientific);
    spot_check(1.7e-212, "1.7e-212", boost::charconv::chars_format::scientific);
    spot_check(1.7e-211, "1.7e-211", boost::charconv::chars_format::scientific);
    spot_check(1.7e-210, "1.7e-210", boost::charconv::chars_format::scientific);
    spot_check(1.7e-209, "1.7e-209", boost::charconv::chars_format::scientific);
    spot_check(1.7e-208, "1.7e-208", boost::charconv::chars_format::scientific);
    spot_check(1.7e-207, "1.7e-207", boost::charconv::chars_format::scientific);
    spot_check(1.7e-206, "1.7e-206", boost::charconv::chars_format::scientific);
    spot_check(1.7e-205, "1.7e-205", boost::charconv::chars_format::scientific);
    spot_check(1.7e-204, "1.7e-204", boost::charconv::chars_format::scientific);
    spot_check(1.7e-203, "1.7e-203", boost::charconv::chars_format::scientific);
    spot_check(1.7e-202, "1.7e-202", boost::charconv::chars_format::scientific);
    spot_check(1.7e-201, "1.7e-201", boost::charconv::chars_format::scientific);
    spot_check(1.7e-200, "1.7e-200", boost::charconv::chars_format::scientific);

    spot_check(1.7e-199, "1.7e-199", boost::charconv::chars_format::scientific);
    spot_check(1.7e-198, "1.7e-198", boost::charconv::chars_format::scientific);
    spot_check(1.7e-197, "1.7e-197", boost::charconv::chars_format::scientific);
    spot_check(1.7e-196, "1.7e-196", boost::charconv::chars_format::scientific);
    spot_check(1.7e-195, "1.7e-195", boost::charconv::chars_format::scientific);
    spot_check(1.7e-194, "1.7e-194", boost::charconv::chars_format::scientific);
    spot_check(1.7e-193, "1.7e-193", boost::charconv::chars_format::scientific);
    spot_check(1.7e-192, "1.7e-192", boost::charconv::chars_format::scientific);
    spot_check(1.7e-191, "1.7e-191", boost::charconv::chars_format::scientific);
    spot_check(1.7e-190, "1.7e-190", boost::charconv::chars_format::scientific);
    spot_check(1.7e-189, "1.7e-189", boost::charconv::chars_format::scientific);
    spot_check(1.7e-188, "1.7e-188", boost::charconv::chars_format::scientific);
    spot_check(1.7e-187, "1.7e-187", boost::charconv::chars_format::scientific);
    spot_check(1.7e-186, "1.7e-186", boost::charconv::chars_format::scientific);
    spot_check(1.7e-185, "1.7e-185", boost::charconv::chars_format::scientific);
    spot_check(1.7e-184, "1.7e-184", boost::charconv::chars_format::scientific);
    spot_check(1.7e-183, "1.7e-183", boost::charconv::chars_format::scientific);
    spot_check(1.7e-182, "1.7e-182", boost::charconv::chars_format::scientific);
    spot_check(1.7e-181, "1.7e-181", boost::charconv::chars_format::scientific);
    spot_check(1.7e-180, "1.7e-180", boost::charconv::chars_format::scientific);
    spot_check(1.7e-179, "1.7e-179", boost::charconv::chars_format::scientific);
    spot_check(1.7e-178, "1.7e-178", boost::charconv::chars_format::scientific);
    spot_check(1.7e-177, "1.7e-177", boost::charconv::chars_format::scientific);
    spot_check(1.7e-176, "1.7e-176", boost::charconv::chars_format::scientific);
    spot_check(1.7e-175, "1.7e-175", boost::charconv::chars_format::scientific);
    spot_check(1.7e-174, "1.7e-174", boost::charconv::chars_format::scientific);
    spot_check(1.7e-173, "1.7e-173", boost::charconv::chars_format::scientific);
    spot_check(1.7e-172, "1.7e-172", boost::charconv::chars_format::scientific);
    spot_check(1.7e-171, "1.7e-171", boost::charconv::chars_format::scientific);
    spot_check(1.7e-170, "1.7e-170", boost::charconv::chars_format::scientific);
    spot_check(1.7e-169, "1.7e-169", boost::charconv::chars_format::scientific);
    spot_check(1.7e-168, "1.7e-168", boost::charconv::chars_format::scientific);
    spot_check(1.7e-167, "1.7e-167", boost::charconv::chars_format::scientific);
    spot_check(1.7e-166, "1.7e-166", boost::charconv::chars_format::scientific);
    spot_check(1.7e-165, "1.7e-165", boost::charconv::chars_format::scientific);
    spot_check(1.7e-164, "1.7e-164", boost::charconv::chars_format::scientific);
    spot_check(1.7e-163, "1.7e-163", boost::charconv::chars_format::scientific);
    spot_check(1.7e-162, "1.7e-162", boost::charconv::chars_format::scientific);
    spot_check(1.7e-161, "1.7e-161", boost::charconv::chars_format::scientific);
    spot_check(1.7e-160, "1.7e-160", boost::charconv::chars_format::scientific);
    spot_check(1.7e-159, "1.7e-159", boost::charconv::chars_format::scientific);
    spot_check(1.7e-158, "1.7e-158", boost::charconv::chars_format::scientific);
    spot_check(1.7e-157, "1.7e-157", boost::charconv::chars_format::scientific);
    spot_check(1.7e-156, "1.7e-156", boost::charconv::chars_format::scientific);
    spot_check(1.7e-155, "1.7e-155", boost::charconv::chars_format::scientific);
    spot_check(1.7e-154, "1.7e-154", boost::charconv::chars_format::scientific);
    spot_check(1.7e-153, "1.7e-153", boost::charconv::chars_format::scientific);
    spot_check(1.7e-152, "1.7e-152", boost::charconv::chars_format::scientific);
    spot_check(1.7e-151, "1.7e-151", boost::charconv::chars_format::scientific);
    spot_check(1.7e-150, "1.7e-150", boost::charconv::chars_format::scientific);
    spot_check(1.7e-149, "1.7e-149", boost::charconv::chars_format::scientific);
    spot_check(1.7e-148, "1.7e-148", boost::charconv::chars_format::scientific);
    spot_check(1.7e-147, "1.7e-147", boost::charconv::chars_format::scientific);
    spot_check(1.7e-146, "1.7e-146", boost::charconv::chars_format::scientific);
    spot_check(1.7e-145, "1.7e-145", boost::charconv::chars_format::scientific);
    spot_check(1.7e-144, "1.7e-144", boost::charconv::chars_format::scientific);
    spot_check(1.7e-143, "1.7e-143", boost::charconv::chars_format::scientific);
    spot_check(1.7e-142, "1.7e-142", boost::charconv::chars_format::scientific);
    spot_check(1.7e-141, "1.7e-141", boost::charconv::chars_format::scientific);
    spot_check(1.7e-140, "1.7e-140", boost::charconv::chars_format::scientific);
    spot_check(1.7e-139, "1.7e-139", boost::charconv::chars_format::scientific);
    spot_check(1.7e-138, "1.7e-138", boost::charconv::chars_format::scientific);
    spot_check(1.7e-137, "1.7e-137", boost::charconv::chars_format::scientific);
    spot_check(1.7e-136, "1.7e-136", boost::charconv::chars_format::scientific);
    spot_check(1.7e-135, "1.7e-135", boost::charconv::chars_format::scientific);
    spot_check(1.7e-134, "1.7e-134", boost::charconv::chars_format::scientific);
    spot_check(1.7e-133, "1.7e-133", boost::charconv::chars_format::scientific);
    spot_check(1.7e-132, "1.7e-132", boost::charconv::chars_format::scientific);
    spot_check(1.7e-131, "1.7e-131", boost::charconv::chars_format::scientific);
    spot_check(1.7e-130, "1.7e-130", boost::charconv::chars_format::scientific);
    spot_check(1.7e-129, "1.7e-129", boost::charconv::chars_format::scientific);
    spot_check(1.7e-128, "1.7e-128", boost::charconv::chars_format::scientific);
    spot_check(1.7e-127, "1.7e-127", boost::charconv::chars_format::scientific);
    spot_check(1.7e-126, "1.7e-126", boost::charconv::chars_format::scientific);
    spot_check(1.7e-125, "1.7e-125", boost::charconv::chars_format::scientific);
    spot_check(1.7e-124, "1.7e-124", boost::charconv::chars_format::scientific);
    spot_check(1.7e-123, "1.7e-123", boost::charconv::chars_format::scientific);
    spot_check(1.7e-122, "1.7e-122", boost::charconv::chars_format::scientific);
    spot_check(1.7e-121, "1.7e-121", boost::charconv::chars_format::scientific);
    spot_check(1.7e-120, "1.7e-120", boost::charconv::chars_format::scientific);
    spot_check(1.7e-119, "1.7e-119", boost::charconv::chars_format::scientific);
    spot_check(1.7e-118, "1.7e-118", boost::charconv::chars_format::scientific);
    spot_check(1.7e-117, "1.7e-117", boost::charconv::chars_format::scientific);
    spot_check(1.7e-116, "1.7e-116", boost::charconv::chars_format::scientific);
    spot_check(1.7e-115, "1.7e-115", boost::charconv::chars_format::scientific);
    spot_check(1.7e-114, "1.7e-114", boost::charconv::chars_format::scientific);
    spot_check(1.7e-113, "1.7e-113", boost::charconv::chars_format::scientific);
    spot_check(1.7e-112, "1.7e-112", boost::charconv::chars_format::scientific);
    spot_check(1.7e-111, "1.7e-111", boost::charconv::chars_format::scientific);
    spot_check(1.7e-110, "1.7e-110", boost::charconv::chars_format::scientific);
    spot_check(1.7e-109, "1.7e-109", boost::charconv::chars_format::scientific);
    spot_check(1.7e-108, "1.7e-108", boost::charconv::chars_format::scientific);
    spot_check(1.7e-107, "1.7e-107", boost::charconv::chars_format::scientific);
    spot_check(1.7e-106, "1.7e-106", boost::charconv::chars_format::scientific);
    spot_check(1.7e-105, "1.7e-105", boost::charconv::chars_format::scientific);
    spot_check(1.7e-104, "1.7e-104", boost::charconv::chars_format::scientific);
    spot_check(1.7e-103, "1.7e-103", boost::charconv::chars_format::scientific);
    spot_check(1.7e-102, "1.7e-102", boost::charconv::chars_format::scientific);
    spot_check(1.7e-101, "1.7e-101", boost::charconv::chars_format::scientific);
    spot_check(1.7e-100, "1.7e-100", boost::charconv::chars_format::scientific);

    spot_check(1.7e-99, "1.7e-99", boost::charconv::chars_format::scientific);
    spot_check(1.7e-98, "1.7e-98", boost::charconv::chars_format::scientific);
    spot_check(1.7e-97, "1.7e-97", boost::charconv::chars_format::scientific);
    spot_check(1.7e-96, "1.7e-96", boost::charconv::chars_format::scientific);
    spot_check(1.7e-95, "1.7e-95", boost::charconv::chars_format::scientific);
    spot_check(1.7e-94, "1.7e-94", boost::charconv::chars_format::scientific);
    spot_check(1.7e-93, "1.7e-93", boost::charconv::chars_format::scientific);
    spot_check(1.7e-92, "1.7e-92", boost::charconv::chars_format::scientific);
    spot_check(1.7e-91, "1.7e-91", boost::charconv::chars_format::scientific);
    spot_check(1.7e-90, "1.7e-90", boost::charconv::chars_format::scientific);
    spot_check(1.7e-89, "1.7e-89", boost::charconv::chars_format::scientific);
    spot_check(1.7e-88, "1.7e-88", boost::charconv::chars_format::scientific);
    spot_check(1.7e-87, "1.7e-87", boost::charconv::chars_format::scientific);
    spot_check(1.7e-86, "1.7e-86", boost::charconv::chars_format::scientific);
    spot_check(1.7e-85, "1.7e-85", boost::charconv::chars_format::scientific);
    spot_check(1.7e-84, "1.7e-84", boost::charconv::chars_format::scientific);
    spot_check(1.7e-83, "1.7e-83", boost::charconv::chars_format::scientific);
    spot_check(1.7e-82, "1.7e-82", boost::charconv::chars_format::scientific);
    spot_check(1.7e-81, "1.7e-81", boost::charconv::chars_format::scientific);
    spot_check(1.7e-80, "1.7e-80", boost::charconv::chars_format::scientific);
    spot_check(1.7e-79, "1.7e-79", boost::charconv::chars_format::scientific);
    spot_check(1.7e-78, "1.7e-78", boost::charconv::chars_format::scientific);
    spot_check(1.7e-77, "1.7e-77", boost::charconv::chars_format::scientific);
    spot_check(1.7e-76, "1.7e-76", boost::charconv::chars_format::scientific);
    spot_check(1.7e-75, "1.7e-75", boost::charconv::chars_format::scientific);
    spot_check(1.7e-74, "1.7e-74", boost::charconv::chars_format::scientific);
    spot_check(1.7e-73, "1.7e-73", boost::charconv::chars_format::scientific);
    spot_check(1.7e-72, "1.7e-72", boost::charconv::chars_format::scientific);
    spot_check(1.7e-71, "1.7e-71", boost::charconv::chars_format::scientific);
    spot_check(1.7e-70, "1.7e-70", boost::charconv::chars_format::scientific);
    spot_check(1.7e-69, "1.7e-69", boost::charconv::chars_format::scientific);
    spot_check(1.7e-68, "1.7e-68", boost::charconv::chars_format::scientific);
    spot_check(1.7e-67, "1.7e-67", boost::charconv::chars_format::scientific);
    spot_check(1.7e-66, "1.7e-66", boost::charconv::chars_format::scientific);
    spot_check(1.7e-65, "1.7e-65", boost::charconv::chars_format::scientific);
    spot_check(1.7e-64, "1.7e-64", boost::charconv::chars_format::scientific);
    spot_check(1.7e-63, "1.7e-63", boost::charconv::chars_format::scientific);
    spot_check(1.7e-62, "1.7e-62", boost::charconv::chars_format::scientific);
    spot_check(1.7e-61, "1.7e-61", boost::charconv::chars_format::scientific);
    spot_check(1.7e-60, "1.7e-60", boost::charconv::chars_format::scientific);
    spot_check(1.7e-59, "1.7e-59", boost::charconv::chars_format::scientific);
    spot_check(1.7e-58, "1.7e-58", boost::charconv::chars_format::scientific);
    spot_check(1.7e-57, "1.7e-57", boost::charconv::chars_format::scientific);
    spot_check(1.7e-56, "1.7e-56", boost::charconv::chars_format::scientific);
    spot_check(1.7e-55, "1.7e-55", boost::charconv::chars_format::scientific);
    spot_check(1.7e-54, "1.7e-54", boost::charconv::chars_format::scientific);
    spot_check(1.7e-53, "1.7e-53", boost::charconv::chars_format::scientific);
    spot_check(1.7e-52, "1.7e-52", boost::charconv::chars_format::scientific);
    spot_check(1.7e-51, "1.7e-51", boost::charconv::chars_format::scientific);
    spot_check(1.7e-50, "1.7e-50", boost::charconv::chars_format::scientific);
    spot_check(1.7e-49, "1.7e-49", boost::charconv::chars_format::scientific);
    spot_check(1.7e-48, "1.7e-48", boost::charconv::chars_format::scientific);
    spot_check(1.7e-47, "1.7e-47", boost::charconv::chars_format::scientific);
    spot_check(1.7e-46, "1.7e-46", boost::charconv::chars_format::scientific);
    spot_check(1.7e-45, "1.7e-45", boost::charconv::chars_format::scientific);
    spot_check(1.7e-44, "1.7e-44", boost::charconv::chars_format::scientific);
    spot_check(1.7e-43, "1.7e-43", boost::charconv::chars_format::scientific);
    spot_check(1.7e-42, "1.7e-42", boost::charconv::chars_format::scientific);
    spot_check(1.7e-41, "1.7e-41", boost::charconv::chars_format::scientific);
    spot_check(1.7e-40, "1.7e-40", boost::charconv::chars_format::scientific);
    spot_check(1.7e-39, "1.7e-39", boost::charconv::chars_format::scientific);
    spot_check(1.7e-38, "1.7e-38", boost::charconv::chars_format::scientific);
    spot_check(1.7e-37, "1.7e-37", boost::charconv::chars_format::scientific);
    spot_check(1.7e-36, "1.7e-36", boost::charconv::chars_format::scientific);
    spot_check(1.7e-35, "1.7e-35", boost::charconv::chars_format::scientific);
    spot_check(1.7e-34, "1.7e-34", boost::charconv::chars_format::scientific);
    spot_check(1.7e-33, "1.7e-33", boost::charconv::chars_format::scientific);
    spot_check(1.7e-32, "1.7e-32", boost::charconv::chars_format::scientific);
    spot_check(1.7e-31, "1.7e-31", boost::charconv::chars_format::scientific);
    spot_check(1.7e-30, "1.7e-30", boost::charconv::chars_format::scientific);
    spot_check(1.7e-29, "1.7e-29", boost::charconv::chars_format::scientific);
    spot_check(1.7e-28, "1.7e-28", boost::charconv::chars_format::scientific);
    spot_check(1.7e-27, "1.7e-27", boost::charconv::chars_format::scientific);
    spot_check(1.7e-26, "1.7e-26", boost::charconv::chars_format::scientific);
    spot_check(1.7e-25, "1.7e-25", boost::charconv::chars_format::scientific);
    spot_check(1.7e-24, "1.7e-24", boost::charconv::chars_format::scientific);
    spot_check(1.7e-23, "1.7e-23", boost::charconv::chars_format::scientific);
    spot_check(1.7e-22, "1.7e-22", boost::charconv::chars_format::scientific);
    spot_check(1.7e-21, "1.7e-21", boost::charconv::chars_format::scientific);
    spot_check(1.7e-20, "1.7e-20", boost::charconv::chars_format::scientific);
    spot_check(1.7e-19, "1.7e-19", boost::charconv::chars_format::scientific);
    spot_check(1.7e-18, "1.7e-18", boost::charconv::chars_format::scientific);
    spot_check(1.7e-17, "1.7e-17", boost::charconv::chars_format::scientific);
    spot_check(1.7e-16, "1.7e-16", boost::charconv::chars_format::scientific);
    spot_check(1.7e-15, "1.7e-15", boost::charconv::chars_format::scientific);
    spot_check(1.7e-14, "1.7e-14", boost::charconv::chars_format::scientific);
    spot_check(1.7e-13, "1.7e-13", boost::charconv::chars_format::scientific);
    spot_check(1.7e-12, "1.7e-12", boost::charconv::chars_format::scientific);
    spot_check(1.7e-11, "1.7e-11", boost::charconv::chars_format::scientific);
    spot_check(1.7e-10, "1.7e-10", boost::charconv::chars_format::scientific);
    spot_check(1.7e-09, "1.7e-09", boost::charconv::chars_format::scientific);
    spot_check(1.7e-08, "1.7e-08", boost::charconv::chars_format::scientific);
    spot_check(1.7e-07, "1.7e-07", boost::charconv::chars_format::scientific);
    spot_check(1.7e-06, "1.7e-06", boost::charconv::chars_format::scientific);
    spot_check(1.7e-05, "1.7e-05", boost::charconv::chars_format::scientific);
    spot_check(1.7e-04, "1.7e-04", boost::charconv::chars_format::scientific);
    spot_check(1.7e-03, "1.7e-03", boost::charconv::chars_format::scientific);
    spot_check(1.7e-02, "1.7e-02", boost::charconv::chars_format::scientific);
    spot_check(1.7e-01, "1.7e-01", boost::charconv::chars_format::scientific);
    spot_check(1.7e-00, "1.7e-00", boost::charconv::chars_format::scientific);

    // 0 exponent in general format
    spot_check(1.7e-00, "1.7e-00", boost::charconv::chars_format::general);
    spot_check(1.7e-00, "1.7e+00", boost::charconv::chars_format::general);
    spot_check(17e-00, "17e+00", boost::charconv::chars_format::general);
    spot_check(17.0e-00, "17.0e+00", boost::charconv::chars_format::general);
    spot_check(170e-00, "170e+00", boost::charconv::chars_format::general);
    spot_check(170.0e-00, "170.0e+00", boost::charconv::chars_format::general);
    spot_check(170.0000e-00, "170.0000e+00", boost::charconv::chars_format::general);

    return boost::report_errors();
}
