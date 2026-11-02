#ifndef SERVICE_TYPE_H
#define SERVICE_TYPE_H

#include <string>
#include <sstream>
#include <iomanip>

class ServiceType {
private:
    std::string id;                         // "ADM", "FEE", "EXM", "CRT"
    std::string name;                       // "Admission & Enrollment"
    std::string description;                // "Document verification, branch allocation..."
    std::string color;                      // Hex color accent
    double baseAvgDurationMinutes;          // Configured baseline (e.g. 8.0 mins)
    double currentAvgDurationMinutes;       // Dynamic moving average from actual history
    int totalCompleted;
    double totalServiceTimeSeconds;

public:
    ServiceType()
        : id(""), name(""), description(""), color("#2563eb"),
          baseAvgDurationMinutes(5.0), currentAvgDurationMinutes(5.0),
          totalCompleted(0), totalServiceTimeSeconds(0.0) {}

    ServiceType(const std::string& i, const std::string& n, const std::string& desc,
                const std::string& col, double baseMins)
        : id(i), name(n), description(desc), color(col),
          baseAvgDurationMinutes(baseMins), currentAvgDurationMinutes(baseMins),
          totalCompleted(0), totalServiceTimeSeconds(0.0) {}

    // Getters
    std::string getId() const { return id; }
    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
    std::string getColor() const { return color; }
    double getBaseAvgDurationMinutes() const { return baseAvgDurationMinutes; }
    double getCurrentAvgDurationMinutes() const { return currentAvgDurationMinutes; }
    int getTotalCompleted() const { return totalCompleted; }

    // Setters
    void setBaseAvgDurationMinutes(double m) {
        if (m > 0.0) baseAvgDurationMinutes = m;
    }

    // Dynamic Moving Average Update when a token is completed:
    // EMA with alpha = 0.3 for responsiveness to current counter pace
    void recordCompletedService(double actualMinutes) {
        if (actualMinutes <= 0.0) return;
        totalCompleted++;
        totalServiceTimeSeconds += (actualMinutes * 60.0);

        if (totalCompleted == 1) {
            currentAvgDurationMinutes = actualMinutes;
        } else {
            const double alpha = 0.30;
            currentAvgDurationMinutes = (alpha * actualMinutes) + ((1.0 - alpha) * currentAvgDurationMinutes);
        }
    }

    std::string toJson() const {
        std::ostringstream oss;
        oss << "{"
            << "\"id\":\"" << id << "\","
            << "\"name\":\"" << name << "\","
            << "\"description\":\"" << description << "\","
            << "\"color\":\"" << color << "\","
            << "\"baseAvgDurationMinutes\":" << std::fixed << std::setprecision(1) << baseAvgDurationMinutes << ","
            << "\"currentAvgDurationMinutes\":" << std::fixed << std::setprecision(1) << currentAvgDurationMinutes << ","
            << "\"totalCompleted\":" << totalCompleted
            << "}";
        return oss.str();
    }
};

#endif // SERVICE_TYPE_H

