
#ifndef UTILS_UTILS_HPP
#define UTILS_UTILS_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace ctUtils {

	class ctErrorCollector {
		bool has_error = false;
		std::string msg;

		public:

		ctErrorCollector() = default;
		void accumulate(std::string _msg) {msg.append(std::move(_msg)); msg.push_back('\n'); has_error = true;}
		bool hasError() {return has_error;}
		const std::string getErrors() {return msg;}
	};

	bool stringToInt(const std::string& str, int32_t& outValue);
	bool stringToUInt(const std::string& str, uint32_t& outValue);
	bool stringToFloat(const std::string& str, float& outValue);

	bool isRegister(std::string& str, uint32_t& outValue);
	bool isSlot(std::string& str, uint32_t& outValue);

	void insertInt32(int32_t inserted, std::vector<uint8_t>& vec);
	void insertUInt32(uint32_t inserted, std::vector<uint8_t>& vec);
	void insertFloat32(float inserted, std::vector<uint8_t>& vec);
}

#endif // UTILS_UTILS_HPP