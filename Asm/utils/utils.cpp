
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string.h>
#include <string>

#include "utils.hpp"

namespace  ctUtils {
		
	bool stringToInt(const std::string& str, int32_t& outValue) {
		if (str.empty()) return false;
		
		std::stringstream ss(str);
		int32_t temp;
		ss >> temp;
		
		if (ss.fail() || !ss.eof()) {
			return false;
		}
		
		outValue = temp;
		return true;
	}

	bool stringToUInt(const std::string& str, uint32_t& outValue) {
		if (str.empty()) return false;
		
		std::stringstream ss(str);
		uint32_t temp;
		ss >> temp;
		
		if (ss.fail() || !ss.eof()) {
			return false;
		}
		
		outValue = temp;
		return true;
	}

	bool stringToFloat(const std::string& str, float& outValue) {
		if (str.empty()) return false;
		
		std::stringstream ss(str);
		float temp;
		ss >> temp;
		
		if (ss.fail() || !ss.eof()) {
			return false;
		}
		
		outValue = temp;
		return true;
	}


	bool isRegister(std::string& str, uint32_t& outValue) {

		if (str.size() < 2 || str[0] != 'r') return false;
		
		std::string numberPart = str.substr(1);
		
		if (ctUtils::stringToUInt(numberPart, outValue)) {
			return outValue < 256;
		}
		return false;
	}
	
	bool isSlot(std::string& str, uint32_t& outValue) {

		if (str.size() < 2 || str[0] != 's') return false;
		
		std::string numberPart = str.substr(1);

		if (ctUtils::stringToUInt(numberPart, outValue)) {
			return outValue < 256;
		}

		return stringToUInt(str.substr(1), outValue);
	}

	
	void insertInt32(int32_t inserted, std::vector<uint8_t>& vec) {
		size_t oldsize = vec.size();
		vec.resize(oldsize + 4);
		memcpy(vec.data() + oldsize, &inserted, sizeof(inserted));
	};

	void insertUInt32(uint32_t inserted, std::vector<uint8_t>& vec)  {
		size_t oldsize = vec.size();
		vec.resize(oldsize + sizeof(inserted));
		memcpy(vec.data() + oldsize, &inserted, sizeof(inserted));
	};

	void insertFloat32(float inserted, std::vector<uint8_t>& vec)  {
		size_t oldsize = vec.size();
		vec.resize(oldsize + sizeof(inserted));
		memcpy(vec.data() + oldsize, &inserted, sizeof(inserted));
	};
}
