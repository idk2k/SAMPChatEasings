#ifndef EASINGS_HOLDER_H
#define EASINGS_HOLDER_H


class EasingsHolder {
public:
	static EasingsHolder& get_instance();
	double easeOutBounce(double x);
};

#endif // EASINGS_HOLDER_H