#include <random>

// Random number generator
std::random_device rd;
std::mt19937 gen(rd());

// Function to generate a random integer within a specified range
int getRandomValue(int minValue, int maxValue) {
    std::uniform_int_distribution<> dis(minValue, maxValue);
    return dis(gen);
}

// Function to generate a random float within a specified range
float getRandomValue(float minValue, float maxValue) {
    std::uniform_real_distribution<> dis(minValue, maxValue);
    return dis(gen);
}
