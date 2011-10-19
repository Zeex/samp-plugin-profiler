#include <map>
#include <sstream>
#include <string>

class ConfigReader {
public:
	ConfigReader(const std::string &filename);

	template<typename T>
	T GetOption(const std::string &name, const T &defaultValue) {
		std::map<std::string, std::string>::const_iterator it = options_.find(name);
		if (it == options_.end()) {
			return defaultValue;
		}
		std::stringstream sstream(it->second);
		T value;
		sstream >> value;
		if (!sstream) {
			return defaultValue;
		}
		return value;
	}

	bool IsLoaded() const { return loaded_; }

private:
	ConfigReader();

	bool loaded_;
	std::map<std::string, std::string> options_;
};

