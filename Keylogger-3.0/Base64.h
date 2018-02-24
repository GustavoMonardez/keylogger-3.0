#pragma once
#ifndef BASE64_H
#define BASE64_H

#include <string>

namespace Base64 {
	const std::string& salt1{ "LM::TB::BB" };
	const std::string& salt2{ "_:/_77" };
	const std::string& salt3{ "line=wowC++" };
	const std::string &base64_codes = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	std::string encodeBase64(const std::string& source);

	std::string encryptBase64(std::string& source) {
		/*scramble*/
		source = salt1 + source + salt2 + salt3;

		/*encode and scramble*/
		source = encodeBase64(source);
		source.insert(7, salt3);
		source += salt1;
		source = encodeBase64(source);

		/*rescrambling/re-encode*/
		source = salt2 + salt3 + source + salt1;
		source = encodeBase64(source);
		source.insert(1, "L");
		source.insert(7, "M");

		return source;
	}
	std::string encodeBase64(const std::string& source) {
		std::string encoded{};
		int val{};
		int bits{ -6 };
		const unsigned int b63{ 0x3f };

		for (const auto& c : source) {
			val = (val << 8) + c;
			bits += 8;
			while (bits >= 0) {
				encoded.push_back(base64_codes[(val >> bits) & b63]);
				bits -= 6;
			}
		}

		if(bits > -6)
			encoded.push_back(base64_codes[((val << 8) >> (bits + 8)) & b63]);

		while (encoded.size() % 4)
			encoded.push_back('-');

		return encoded;
	}
}

#endif

