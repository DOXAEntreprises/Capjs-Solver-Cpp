#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <cstdio>
#include <memory>
#include "../include/challenge_solver.h"

class CurlHTTPSClient {
private:
    std::string site_id;
    std::string base_url;

    // Execute curl command and get output
    std::string exec_curl(const std::string& cmd) {
        std::array<char, 128> buffer;
        std::string result;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
        
        if (!pipe) {
            throw std::runtime_error("Failed to execute curl command");
        }
        
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
        
        return result;
    }

    // Extract JSON value using simple string parsing
    std::string extract_json_value(const std::string& json, const std::string& key) {
        std::string search_pattern = "\"" + key + "\"";
        size_t key_pos = json.find(search_pattern);
        if (key_pos == std::string::npos) {
            return "";
        }

        size_t colon_pos = json.find(":", key_pos);
        if (colon_pos == std::string::npos) {
            return "";
        }

        size_t value_start = colon_pos + 1;
        
        // Skip whitespace
        while (value_start < json.length() && std::isspace(json[value_start])) {
            value_start++;
        }

        if (value_start >= json.length()) {
            return "";
        }

        // Check if value is quoted
        bool is_quoted = (json[value_start] == '"');
        if (is_quoted) {
            value_start++;
        }

        size_t value_end = value_start;
        if (is_quoted) {
            // Find closing quote
            while (value_end < json.length() && json[value_end] != '"') {
                if (json[value_end] == '\\') {
                    value_end += 2; // Skip escaped character
                } else {
                    value_end++;
                }
            }
        } else {
            // Find end of unquoted value
            while (value_end < json.length() && 
                   json[value_end] != ',' && 
                   json[value_end] != '}' && 
                   json[value_end] != ']' &&
                   !std::isspace(json[value_end])) {
                value_end++;
            }
        }

        return json.substr(value_start, value_end - value_start);
    }

public:
    CurlHTTPSClient(const std::string& site_id, const std::string& base_url) 
        : site_id(site_id), base_url(base_url) {}

    // Get challenge from CAP server
    std::map<std::string, std::string> get_challenge() {
        std::string url = base_url + "/" + site_id + "/challenge";
        std::string cmd = "curl -s -X POST -H 'Content-Type: application/json' -d '{}' '" + url + "'";
        
        try {
            std::string response = exec_curl(cmd);
            
            if (response.empty()) {
                return {};
            }

            std::map<std::string, std::string> challenge_data;
            challenge_data["token"] = extract_json_value(response, "token");
            
            // Extract challenge object
            std::string challenge_obj_start = "\"challenge\"";
            size_t challenge_pos = response.find(challenge_obj_start);
            if (challenge_pos != std::string::npos) {
                size_t obj_start = response.find("{", challenge_pos);
                size_t obj_end = response.find("}", obj_start);
                if (obj_start != std::string::npos && obj_end != std::string::npos) {
                    std::string challenge_obj = response.substr(obj_start, obj_end - obj_start + 1);
                    challenge_data["c"] = extract_json_value(challenge_obj, "c");
                    challenge_data["s"] = extract_json_value(challenge_obj, "s");
                    challenge_data["d"] = extract_json_value(challenge_obj, "d");
                }
            }

            if (challenge_data["token"].empty()) {
                return {};
            }

            return challenge_data;
            
        } catch (const std::exception& e) {
            return {};
        }
    }

    // Submit solution to CAP server
    bool submit_solution(const std::string& token, const std::vector<uint64_t>& solution) {
        // Build JSON manually
        std::ostringstream json_builder;
        json_builder << "{\"token\":\"" << token << "\",\"solutions\":[";
        for (size_t i = 0; i < solution.size(); ++i) {
            if (i > 0) json_builder << ",";
            json_builder << solution[i];
        }
        json_builder << "]}";
        
        std::string json_data = json_builder.str();
        
        std::string url = base_url + "/" + site_id + "/redeem";
        std::string cmd = "curl -s -X POST -H 'Content-Type: application/json' -d '" + json_data + "' '" + url + "'";
        
        try {
            std::string response = exec_curl(cmd);
            
            if (response.empty()) {
                return false;
            }

            std::string success = extract_json_value(response, "success");
            std::string result_token = extract_json_value(response, "token");
            
            if (success == "true") {
                std::cout << "token: " << result_token << std::endl;
                return true;
            } else {
                return false;
            }
            
        } catch (const std::exception& e) {
            return false;
        }
    }
};

// Progress callback function
void progress_callback(int progress, int current, int completed, uint64_t nonce) {
    std::cout << "challenge " << current << " nonce: " << nonce << std::endl;
}

int main() {
    try {
        CurlHTTPSClient client("ecdf80f9a5", "https://captcha.mefrp.com");

        // Step 1: Get challenge
        auto challenge_data = client.get_challenge();
        if (challenge_data.empty()) {
            return 1;
        }

        std::string token = challenge_data["token"];
        size_t c = std::stoull(challenge_data["c"]);
        size_t s = std::stoull(challenge_data["s"]);
        size_t d = std::stoull(challenge_data["d"]);

        std::cout << "challenge parameters:" << std::endl;
        std::cout << "token: " << token << std::endl;
        std::cout << "c: " << c << std::endl;
        std::cout << "s: " << s << std::endl;
        std::cout << "d: " << d << std::endl;

        // Step 2: Solve challenge using C++ solver
        cap::SolveConfig config;
        config.c = c;
        config.s = s;
        config.d = d;
        config.on_progress = progress_callback;

        std::vector<uint64_t> solution = cap::ChallengeSolver::solve(token, config);

        std::cout << "solution: [";
        for (size_t i = 0; i < solution.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << solution[i];
        }
        std::cout << "]" << std::endl;

        // Step 3: Submit solution
        bool success = client.submit_solution(token, solution);
        
        if (success) {
            return 0;
        } else {
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << std::endl;
        return 1;
    }
}
