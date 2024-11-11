#include <limits.h>
/*won't work if x is bigger than int*/
int mx_sqrt(int x) {
	if (x <= 0)
		return 0;
	int i = 1;

	while (i * i < x) {
		i += 1;
		if (INT_MAX / i < i)
			return 0;
	}
	if (i * i == x)
		return i;
	return 0;
}
