#ifndef EASINGS_HOLDER_H
#define EASINGS_HOLDER_H

#define _USE_MATH_DEFINES
#include <cmath>

class EasingsHolder {
public:
    static EasingsHolder& get_instance();

    [[nodiscard]] double easeInSine(double x) noexcept;
    [[nodiscard]] double easeOutSine(double x) noexcept;
    [[nodiscard]] double easeInOutSine(double x) noexcept;
    [[nodiscard]] double easeInQuad(double x) noexcept;
    [[nodiscard]] double easeOutQuad(double x) noexcept;
    [[nodiscard]] double easeInOutQuad(double x) noexcept;

    [[nodiscard]] double easeInCubic(double x) noexcept;
    [[nodiscard]] double easeOutCubic(double x) noexcept;
    [[nodiscard]] double easeInOutCubic(double x) noexcept;
    [[nodiscard]] double easeInQuart(double x) noexcept;
    [[nodiscard]] double easeOutQuart(double x) noexcept;
    [[nodiscard]] double easeInOutQuart(double x) noexcept;

    [[nodiscard]] double easeInQuint(double x) noexcept;
    [[nodiscard]] double easeOutQuint(double x) noexcept;
    [[nodiscard]] double easeInOutQuint(double x) noexcept;
    [[nodiscard]] double easeInExpo(double x) noexcept;
    [[nodiscard]] double easeOutExpo(double x) noexcept;
    [[nodiscard]] double easeInOutExpo(double x) noexcept;

    [[nodiscard]] double easeInCirc(double x) noexcept;
    [[nodiscard]] double easeOutCirc(double x) noexcept;
    [[nodiscard]] double easeInOutCirc(double x) noexcept;
    [[nodiscard]] double easeInBack(double x) noexcept;
    [[nodiscard]] double easeOutBack(double x) noexcept;
    [[nodiscard]] double easeInOutBack(double x) noexcept;

    [[nodiscard]] double easeInElastic(double x) noexcept;
    [[nodiscard]] double easeOutElastic(double x) noexcept;
    [[nodiscard]] double easeInOutElastic(double x) noexcept;
    [[nodiscard]] double easeInBounce(double x) noexcept;
    [[nodiscard]] double easeOutBounce(double x) noexcept;  // perfectly implemented, and usage tested
    [[nodiscard]] double easeInOutBounce(double x) noexcept;
};

#endif  // EASINGS_HOLDER_H