// Copyright 2014 Sebastian A. Mueller
#include "catch.hpp"
#include "merlict/merlict.h"
namespace ml = merlict;


TEST_CASE("StringToolsTest: tokenize_text_several_delimiters", "[merlict]") {
    std::string text = "hans klaus\tpeter";
    std::vector<std::string> tokens =
        ml::txt::tokenize_text_using_either_one_of_delimiters(text, " \t");
    REQUIRE(tokens.size() == 3u);
    CHECK(tokens.at(0) == "hans");
    CHECK(tokens.at(1) == "klaus");
    CHECK(tokens.at(2) == "peter");
    tokens =
        ml::txt::tokenize_text_using_either_one_of_delimiters(text, " ");
    REQUIRE(tokens.size() == 2u);
    CHECK(tokens.at(0) == "hans");
    CHECK(tokens.at(1) == "klaus\tpeter");
    tokens =
        ml::txt::tokenize_text_using_either_one_of_delimiters(text, "\t");
    REQUIRE(tokens.size() == 2u);
    CHECK(tokens.at(0) == "hans klaus");
    CHECK(tokens.at(1) == "peter");
}

TEST_CASE("StringToolsTest: tokenize_epmty_text", "[merlict]") {
    std::string text = "";
    std::vector<std::string> tokens =
        ml::txt::tokenize_text_using_either_one_of_delimiters(text, " \t");
    CHECK(tokens.size() == 0u);
}

TEST_CASE("StringToolsTest: tokenize_text_delimiter_in_the_end", "[merlict]") {
    std::string text = "hans peter ";
    std::vector<std::string> tokens =
        ml::txt::tokenize_text_using_either_one_of_delimiters(text, " \t");
    REQUIRE(tokens.size() == 2u);
    CHECK(tokens.at(0) == "hans");
    CHECK(tokens.at(1) == "peter");
}

TEST_CASE("StringToolsTest: tokenize_text_several_same_delimiters", "[merlict]") {
    std::string text = "hans   peter";
    std::vector<std::string> tokens =
        ml::txt::tokenize_text_using_either_one_of_delimiters(text, " \t");
    REQUIRE(tokens.size() == 2u);
    CHECK(tokens.at(0) == "hans");
    CHECK(tokens.at(1) == "peter");
}

TEST_CASE("StringToolsTest: tokenize_text_several_different_delimiters", "[merlict]") {
    std::string text = "hans \t peter";
    std::vector<std::string> tokens =
        ml::txt::tokenize_text_using_either_one_of_delimiters(text, " \t");
    REQUIRE(tokens.size() == 2u);
    CHECK(tokens.at(0) == "hans");
    CHECK(tokens.at(1) == "peter");
}

TEST_CASE("StringToolsTest: is_ending", "[merlict]") {
    CHECK(ml::txt::is_ending("hans.xml", ".xml"));
    CHECK(!ml::txt::is_ending("hans.xml", ".XML"));
    CHECK(!ml::txt::is_ending("blabla_text_bla", ".xml"));
    CHECK(!ml::txt::is_ending("text.xmltext", ".xml"));
    CHECK(ml::txt::is_ending(".xml", ".xml"));
    CHECK(ml::txt::is_ending(".xml.xml", ".xml"));
    CHECK(ml::txt::is_ending("\t\n\r.xml", ".xml"));
    CHECK(ml::txt::is_ending("text .xml", ".xml"));
    CHECK(!ml::txt::is_ending("text.x ml", ".xml"));
    CHECK(ml::txt::is_ending("abc", "c"));
    CHECK(ml::txt::is_ending("abc\n", "\n"));
}

TEST_CASE("StringToolsTest: is_equal", "[merlict]") {
    CHECK(ml::txt::is_equal("", ""));
    CHECK(!ml::txt::is_equal("a", "b"));
    CHECK(ml::txt::is_equal("a", "a"));
    CHECK(!ml::txt::is_equal("a", "A"));
    CHECK(ml::txt::is_equal("Auto", "Auto"));
    CHECK(!ml::txt::is_equal("Auto", "auto"));
    CHECK(ml::txt::is_equal("Auto ", "Auto "));
    CHECK(!ml::txt::is_equal("Auto ", "Auto"));
    CHECK(ml::txt::is_equal("\n", "\n"));
    CHECK(!ml::txt::is_equal(" Auto", "Auto"));
}

TEST_CASE("StringToolsTest: cut_leading_token", "[merlict]") {
    std::string names = "Hans,Peter,Klaus";
    std::string first_name =
    ml::txt::cut_leading_token_infront_of_delimiter(&names, ',');

    CHECK(first_name == "Hans");
    CHECK(names == "Peter,Klaus");

    names = ",Hans,Peter,Klaus";
    first_name = ml::txt::cut_leading_token_infront_of_delimiter(
        &names,
        ',');
    CHECK(first_name == "");
    CHECK(names == "Hans,Peter,Klaus");

    names = ",Hans/Peter/Klaus/";
    first_name = ml::txt::cut_leading_token_infront_of_delimiter(
        &names,
        '/');
    CHECK(first_name == ",Hans");
    CHECK(names == "Peter/Klaus/");

    names = "Hans ,Peter,Klaus";
    first_name = ml::txt::cut_leading_token_infront_of_delimiter(
        &names,
        ',');
    CHECK(first_name == "Hans ");
    CHECK(names == "Peter,Klaus");

    names =  " Hans,Peter,Klaus";
    first_name = ml::txt::cut_leading_token_infront_of_delimiter(
        &names,
        ',');
    CHECK(first_name == " Hans");
    CHECK(names == "Peter,Klaus");
}

TEST_CASE("StringToolsTest: contains_char", "[merlict]") {
    std::string text = "Hans Peter is an awesome engineer who build AMS!";
    CHECK(ml::txt::string_contains_char(text, 'H'));
    CHECK(ml::txt::string_contains_char(text, 'A'));
    CHECK(ml::txt::string_contains_char(text, 'w'));
    CHECK(ml::txt::string_contains_char(text, '!'));
    CHECK(!ml::txt::string_contains_char(text, '?'));
    CHECK(!ml::txt::string_contains_char(text, '*'));
}

TEST_CASE("StringToolsTest: place_infront_of_each_line", "[merlict]") {
    std::stringstream out;
    out << "Dear Customer,\n";
    out << "\n";
    out << "Please Shut the fuck up!\n";
    out << "\n";
    out << "Kind regards, your customer service\n";
    std::string result =
        ml::txt::place_first_infront_of_each_new_line_of_second(
            "->",
            out.str());
    std::stringstream expected_out;
    expected_out << "->Dear Customer,\n";
    expected_out << "->\n";
    expected_out << "->Please Shut the fuck up!\n";
    expected_out << "->\n";
    expected_out << "->Kind regards, your customer service\n";
    CHECK(ml::txt::is_equal(expected_out.str(), result));
}

TEST_CASE("StringToolsTest: fill_with_whitespaces_until_column", "[merlict]") {
    std::string hi = "hallo world";
    std::string ho =
        ml::txt::fill_up_text_with_whitespaces_until_column(hi, 80);
    CHECK(ho.length() == 80u);
    std::string empty = "";
    std::string empty_but_whitespaces =
        ml::txt::fill_up_text_with_whitespaces_until_column(empty, 1337);
    CHECK(empty_but_whitespaces.length() == 1337u);
    std::string str42 = "0123456789012345678901234567890123456789012";
    std::string strfull =
        ml::txt::fill_up_text_with_whitespaces_until_column(str42, 19);
    CHECK(strfull.length() == (19u*3u));
}

TEST_CASE("StringToolsTest: strip_leading_and_tailing_whitespaces", "[merlict]") {
    std::string no_whitespaces = "no_whitespaces";
    CHECK(ml::txt::is_equal(no_whitespaces, ml::txt::strip_whitespaces(no_whitespaces)));
    std::string whitespaces_in_between = "whitespaces in between";
    CHECK(ml::txt::is_equal(whitespaces_in_between, ml::txt::strip_whitespaces(whitespaces_in_between)));
    std::string leading_whitespaces = "    leading_whitespaces";
    CHECK(ml::txt::is_equal("leading_whitespaces", ml::txt::strip_whitespaces(leading_whitespaces)));
    std::string trailing_ws = "trailing_ws  ";
    CHECK(ml::txt::is_equal("trailing_ws", ml::txt::strip_whitespaces(trailing_ws)));
}

TEST_CASE("StringToolsTest: to_double", "[merlict]") {
    CHECK(ml::txt::to_double("42.1337") == 42.1337);
    CHECK(ml::txt::to_double("1337") == 1337);
    CHECK(ml::txt::to_double("1") == 1u);
    CHECK(ml::txt::to_double(".1") == .1);
    CHECK(ml::txt::to_double(".1442") == .1442);
    CHECK(ml::txt::to_double(" 1.337") == 1.337);
    CHECK(ml::txt::to_double(" .1") == .1);
    CHECK(ml::txt::to_double("1.337e42") == 1.337e42);
    CHECK(ml::txt::to_double("1.337E42") == 1.337E42);
    CHECK_THROWS_AS(ml::txt::to_double("42p1337"), std::invalid_argument);
    CHECK_THROWS_AS(ml::txt::to_double("1337 "), std::invalid_argument);
    CHECK_THROWS_AS(ml::txt::to_double(""), std::invalid_argument);
    CHECK_THROWS_AS(ml::txt::to_double(".1 442"), std::invalid_argument);
}

TEST_CASE("StringToolsTest: to_int", "[merlict]") {
    CHECK(ml::txt::to_int("1") == 1);
    CHECK(ml::txt::to_int("12") == 12);
    CHECK(ml::txt::to_int("1337") == 1337);
    CHECK(ml::txt::to_int(" 42") == 42);
    CHECK(ml::txt::to_int("123456789") == 123456789);
    CHECK_THROWS_AS(ml::txt::to_int("1 "), std::invalid_argument);
    CHECK_THROWS_AS(ml::txt::to_int(""), std::invalid_argument);
    CHECK_THROWS_AS(ml::txt::to_int("123 456"), std::invalid_argument);
    CHECK_THROWS_AS(ml::txt::to_int("12.344"), std::invalid_argument);
    CHECK_THROWS_AS(ml::txt::to_int("no"), std::invalid_argument);
    CHECK_THROWS_AS(ml::txt::to_int("6p6"), std::invalid_argument);
}

TEST_CASE("StringToolsTest: to_Vec3", "[merlict]") {
    CHECK(ml::txt::to_Vec3("[1,2,3]") == ml::Vec3(1, 2, 3));
    CHECK(ml::txt::to_Vec3("[1.2,3.4,5.6]") == ml::Vec3(1.2, 3.4, 5.6));
    CHECK(ml::txt::to_Vec3("[ 1.2,3.4,5.6]") == ml::Vec3(1.2, 3.4, 5.6));
    CHECK(ml::txt::to_Vec3("[1.2 ,3.4,5.6]") == ml::Vec3(1.2, 3.4, 5.6));
    CHECK(ml::txt::to_Vec3("[1.2, 3.4,5.6]") == ml::Vec3(1.2, 3.4, 5.6));
    CHECK(ml::txt::to_Vec3("[1.2,3.4 ,5.6]") == ml::Vec3(1.2, 3.4, 5.6));
    CHECK(ml::txt::to_Vec3("[1.2,3.4, 5.6]") == ml::Vec3(1.2, 3.4, 5.6));
    CHECK(ml::txt::to_Vec3("[1.2,3.4,5.6 ]") == ml::Vec3(1.2, 3.4, 5.6));
    CHECK(ml::txt::to_Vec3("[ 1.2 ,3.4,5.6]") == ml::Vec3(1.2, 3.4, 5.6));
    CHECK(ml::txt::to_Vec3("[1.2 , 3.4,5.6]") == ml::Vec3(1.2, 3.4, 5.6));
    CHECK(ml::txt::to_Vec3("[1.2, 3.4 ,5.6]") == ml::Vec3(1.2, 3.4, 5.6));
    CHECK(ml::txt::to_Vec3("[1.2,3.4 , 5.6]") == ml::Vec3(1.2, 3.4, 5.6));
    CHECK(ml::txt::to_Vec3("[1.2,3.4 , 5.6]") == ml::Vec3(1.2, 3.4, 5.6));
    CHECK(ml::txt::to_Vec3("[1.2,3.4, 5.6 ]") == ml::Vec3(1.2, 3.4, 5.6));
    CHECK(ml::txt::to_Vec3("[1.2,3.4 , 5.6]") == ml::Vec3(1.2, 3.4, 5.6));
    CHECK(ml::txt::to_Vec3("[ 1.2 , 3.4 , 5.6 ]") == ml::Vec3(1.2, 3.4, 5.6));
    CHECK_THROWS_AS(ml::txt::to_Vec3("1,2,3]"), std::invalid_argument);
    CHECK_THROWS_AS(ml::txt::to_Vec3("[1,2,3"), std::invalid_argument);
    CHECK_THROWS_AS(ml::txt::to_Vec3("1,2,3"), std::invalid_argument);
    CHECK_THROWS_AS(ml::txt::to_Vec3("[1,2]"), std::invalid_argument);
    CHECK_THROWS_AS(ml::txt::to_Vec3("[1 2,3]"), std::invalid_argument);
    CHECK_THROWS_AS(ml::txt::to_Vec3("[1 2 3]"), std::invalid_argument);
    CHECK_THROWS_AS(ml::txt::to_Vec3("[1,2 3]"), std::invalid_argument);
    CHECK_THROWS_AS(ml::txt::to_Vec3("[1,,3]"), std::invalid_argument);
    CHECK_THROWS_AS(ml::txt::to_Vec3("[,,]"), std::invalid_argument);
    CHECK_THROWS_AS(ml::txt::to_Vec3("[1,,]"), std::invalid_argument);
    CHECK_THROWS_AS(ml::txt::to_Vec3("[,2,]"), std::invalid_argument);
}
