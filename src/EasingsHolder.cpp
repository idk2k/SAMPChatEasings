#include "EasingsHolder.h"

EasingsHolder& EasingsHolder::get_instance() {
    static EasingsHolder easings_holder_inst{};
    return easings_holder_inst;
}

[[nodiscard]] double EasingsHolder::easeInSine(double x) noexcept {
    double arg = x * M_PI;
    return 1 - std::cos((arg) / 2);
}

[[nodiscard]] double EasingsHolder::easeOutSine(double x) noexcept {
    double arg = x * M_PI;
    return std::sin((arg) / 2);
}

[[nodiscard]] double EasingsHolder::easeInOutSine(double x) noexcept {
    double arg = x * M_PI;
    return -(std::cos(arg) - 1) / 2;
}

[[nodiscard]] double EasingsHolder::easeInQuad(double x) noexcept {
    double result = x * x;
    return result;
}

[[nodiscard]] double EasingsHolder::easeOutQuad(double x) noexcept {
    double result = 1 - (1 - x) * (1 - x);
    return result;
}

[[nodiscard]] double EasingsHolder::easeInOutQuad(double x) noexcept {
    double result = x < 0.5 ? 2 * x * x : 1 - std::pow(-2 * x + 2, 2) / 2;
    return result;
}

[[nodiscard]] double EasingsHolder::easeInCubic(double x) noexcept {
    double result = x * x * x;
    return result;
}

[[nodiscard]] double EasingsHolder::easeOutCubic(double x) noexcept {
    double result = 1 - std::pow(1 - x, 3);
    return result;
}

[[nodiscard]] double EasingsHolder::easeInOutCubic(double x) noexcept {
    double result = x < 0.5 ? 4 * x * x * x : 1 - std::pow(-2 * x + 2, 3) / 2;
    return result;
}

[[nodiscard]] double EasingsHolder::easeInQuart(double x) noexcept {
    double result = x * x * x * x;
    return result;
}

[[nodiscard]] double EasingsHolder::easeOutQuart(double x) noexcept {
    double result = 1 - std::pow(1 - x, 4);
    return result;
}

[[nodiscard]] double EasingsHolder::easeInOutQuart(double x) noexcept {
    double result = x < 0.5 ? 8 * x * x * x * x : 1 - std::pow(-2 * x + 2, 4) / 2;
    return result;
}

[[nodiscard]] double EasingsHolder::easeInQuint(double x) noexcept {
    double result = x * x * x * x * x;
    return result;
}

[[nodiscard]] double EasingsHolder::easeOutQuint(double x) noexcept {
    double result = 1 - std::pow(1 - x, 5);
    return result;
}

[[nodiscard]] double EasingsHolder::easeInOutQuint(double x) noexcept {
    double result = x < 0.5 ? 16 * x * x * x * x * x : 1 - std::pow(-2 * x + 2, 5) / 2;
    return result;
}

[[nodiscard]] double EasingsHolder::easeInExpo(double x) noexcept {
    double result = x == 0 ? 0 : std::pow(2, 10 * x - 10);
    return result;
}

[[nodiscard]] double EasingsHolder::easeOutExpo(double x) noexcept {
    double result = x == 1 ? 1 : 1 - std::pow(2, -10 * x);
    return result;
}

[[nodiscard]] double EasingsHolder::easeInOutExpo(double x) noexcept {
    return x == 0 ? 0 : x == 1 ? 1 : x < 0.5 ? std::pow(2, 20 * x - 10) / 2 : (2 - std::pow(2, -20 * x + 10)) / 2;
}

[[nodiscard]] double EasingsHolder::easeInCirc(double x) noexcept {
    double result = 1 - std::sqrt(1 - std::pow(x, 2));
    return result;
}

[[nodiscard]] double EasingsHolder::easeOutCirc(double x) noexcept {
    double result = std::sqrt(1 - std::pow(x - 1, 2));
    return result;
}

[[nodiscard]] double EasingsHolder::easeInOutCirc(double x) noexcept {
    return x < 0.5 ? (1 - std::sqrt(1 - std::pow(2 * x, 2))) / 2 : (std::sqrt(1 - std::pow(-2 * x + 2, 2)) + 1) / 2;
}

[[nodiscard]] double EasingsHolder::easeInBack(double x) noexcept {
    const double c1 = 1.70158;
    const double c3 = c1 + 1;

    return c3 * x * x * x - c1 * x * x;
}

[[nodiscard]] double EasingsHolder::easeOutBack(double x) noexcept {
    const double c1 = 1.70158;
    const double c3 = c1 + 1;

    return 1 + c3 * std::pow(x - 1, 3) + c1 * std::pow(x - 1, 2);
}

[[nodiscard]] double EasingsHolder::easeInOutBack(double x) noexcept {
    const double c1 = 1.70158;
    const double c2 = c1 * 1.525;

    return x < 0.5 ? (std::pow(2 * x, 2) * ((c2 + 1) * 2 * x - c2)) / 2
                   : (std::pow(2 * x - 2, 2) * ((c2 + 1) * (x * 2 - 2) + c2) + 2) / 2;
}

[[nodiscard]] double EasingsHolder::easeInElastic(double x) noexcept {
    const double c4 = (2 * M_PI) / 3;

    return x == 0 ? 0 : x == 1 ? 1 : -std::pow(2, 10 * x - 10) * std::sin((x * 10 - 10.75) * c4);
}

[[nodiscard]] double EasingsHolder::easeOutElastic(double x) noexcept {
    const double c4 = (2 * M_PI) / 3;

    return x == 0 ? 0 : x == 1 ? 1 : std::pow(2, -10 * x) * std::sin((x * 10 - 0.75) * c4) + 1;
}

[[nodiscard]] double EasingsHolder::easeInOutElastic(double x) noexcept {
    const double c5 = (2 * M_PI) / 4.5;

    return x == 0    ? 0
           : x == 1  ? 1
           : x < 0.5 ? -(std::pow(2, 20 * x - 10) * std::sin((20 * x - 11.125) * c5)) / 2
                     : (std::pow(2, -20 * x + 10) * std::sin((20 * x - 11.125) * c5)) / 2 + 1;
}

[[nodiscard]] double EasingsHolder::easeInBounce(double x) noexcept {
    double result = 1 - this->easeOutBounce(1 - x);
    return result;
}

[[nodiscard]] double EasingsHolder::easeOutBounce(double x) noexcept {
    double n1 = 7.5625;
    double d1 = 2.75;

    if (x < 1 / d1) {
        return n1 * x * x;
    } else if (x < 2 / d1) {
        return n1 * (x -= 1.5 / d1) * x + 0.75;
    } else if (x < 2.5 / d1) {
        return n1 * (x -= 2.25 / d1) * x + 0.9375;
    } else {
        return n1 * (x -= 2.625 / d1) * x + 0.984375;
    }
}

[[nodiscard]] double EasingsHolder::easeInOutBounce(double x) noexcept {
    return x < 0.5 ? (1 - this->easeOutBounce(1 - 2 * x)) / 2 : (1 + this->easeOutBounce(2 * x - 1)) / 2;
}