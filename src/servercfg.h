#include <map>
#include <string>

class ServerCfg {
public:
	ServerCfg(const std::string &filename = "server.cfg");

	std::string GetOption(const std::string &name);
	long GetOptionAsInt(const std::string &name, int defaultValue = 0);
	double GetOptionAsDouble(const std::string &name, double defaultValue = 0.0);

private:
	std::map<std::string, std::string> options_;
};

