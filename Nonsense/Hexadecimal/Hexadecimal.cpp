//TODO: Add cross-platform support
#include <cstdio>
#include <map>
#include <random>
#include <utility>
#include <Windows.h>

using namespace std;

template<typename T>
class RandomHolder {
private:
	default_random_engine engine;
	map<pair<T, T>, uniform_int_distribution<T>> dists;
public:
	T get(T min, T max) {
		pair<T, T> range(min, max);
		if (dists.find(range) == dists.end()) {
			dists[range] = uniform_int_distribution<T>(range.first, range.second);
		}
		return dists[range](engine);
	}
};

int main() {
	system("color 0a");
	default_random_engine engine;
	uniform_int_distribution<int> forDisplay(0, 100000000);
	uniform_int_distribution<int> forDelay(0, 60);
	uniform_int_distribution<int> forPage(0, 300);
	uniform_int_distribution<int> forDecision1(0, 3);
	uniform_int_distribution<int> forDecision250(0, 5);
	//RandomHolder<int> holder;
	printf("--STARTING KERNEL MEMORY DUMP--\n");
	for (int i = 0; i < 5000; i++) {
		if (i % 250 == 0) {
		sectionStart:
			int pages = forPage(engine);
			printf("\nSECTION 0x%x, %d VIRTUAL PAGES\n", forDisplay(engine), pages);
			if (!forDecision250(engine)) {
				printf("!!PAGE CORRUPTION DETECTED. SENDING SIGSEGV TO ALL %d PROCESSES!!\n", pages);
				for (int j = 0; j < pages * 10; j++) {
					printf("%x", forDisplay(engine));
				}
				printf("\n");
				goto sectionStart;
			}
		}
		printf("%x", forDisplay(engine));
		if (!forDecision1(engine))
			Sleep(forDelay(engine));
		if (!forDecision1(engine))
			printf(" ");
	}
	printf("\n--PROCESS COMPLETE--\n");
	return 0;
}