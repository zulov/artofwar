#pragma once
#include <regex>
#include <string>

namespace std {
	template <class BidirIt, class Traits, class CharT>
	std::basic_string<CharT> regex_replace(BidirIt first, BidirIt last,
	                                       const std::basic_regex<CharT, Traits>& re, vector<int>& values) {
		std::basic_string<CharT> s;

		typename std::match_results<BidirIt>::difference_type
			positionOfLastMatch = 0;
		auto endOfLastMatch = first;

		int i = 0;
		auto callback = [&](const std::match_results<BidirIt>& match)
		{
			auto positionOfThisMatch = match.position(0);
			auto diff = positionOfThisMatch - positionOfLastMatch;

			auto startOfThisMatch = endOfLastMatch;
			std::advance(startOfThisMatch, diff);

			s.append(endOfLastMatch, startOfThisMatch);
			s.append(std::to_string(values[i++]));

			auto lengthOfMatch = match.length(0);

			positionOfLastMatch = positionOfThisMatch + lengthOfMatch;

			endOfLastMatch = startOfThisMatch;
			std::advance(endOfLastMatch, lengthOfMatch);
		};

		std::regex_iterator<BidirIt> begin(first, last, re), end;
		std::for_each(begin, end, callback);

		s.append(endOfLastMatch, last);

		return s;
	}

	template <class Traits, class CharT>
	std::string regex_replace(const std::string& s,
	                          const basic_regex<CharT, Traits>& re, vector<int>& values) {
		return regex_replace(s.cbegin(), s.cend(), re, values);
	}

}
