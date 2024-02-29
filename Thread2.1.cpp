#include <iostream>
#include <Windows.h>
#include <thread>
#include <math.h>
#include <iomanip>
#include <fstream>
#include <string>
#include <mutex>
#include <atomic>

double res;
int a;
CRITICAL_SECTION ocs;
std::mutex count_mutex;
std::atomic<long long> result(0.0);

void th1() {
    for (int i = 0; i < 10000000; i += 2) {
        for (int j = 0; j < 20; j++) {
            EnterCriticalSection(&ocs);
            res -= sqrt(i + a);
            LeaveCriticalSection(&ocs);
        }
    }
}

void th2() {
    for (int i = 0; i < 10000000; i += 2) {
        for (int j = 0; j < 20; j++) {
            res -= sqrt(i + a);
        }
    }
}

void th3() {
    for (int i = 0; i < 10000000; i += 2) {
        for (int j = 0; j < 20; j++) {
            count_mutex.lock();
            res -= sqrt(i + a);
            count_mutex.unlock();
        }
    }
}

void th4() {
    for (int i = 0; i < 10000000; i += 2) {
        for (int j = 0; j < 20; j++) {
            result.fetch_sub(sqrt(i + a), std::memory_order_relaxed);
        }
    }
}

int main()
{
    setlocale(LC_ALL, "RU");
    std::cin >> a;
    InitializeCriticalSection(&ocs);
    int startTime1 = clock();
    for (int i = 0; i < 10000000; i++) {
        for (int j = 0; j < 20; j++) {
            if (i & 1) res += sqrt(i + a);
            else res -= sqrt(i + a);
        }
    }
    int endTime1 = clock();
    std::cout << "1st time Один поток : " << endTime1 - startTime1 << "ms" << "\n" << res << "\n";
    //outData << "1st time: " << endTime - startTime << "Результат: " << res;
    res = 0;

    auto thf1 = CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(th1), NULL, CREATE_SUSPENDED, LPDWORD());
    int startTime2 = clock();
    
     ResumeThread(thf1);
     for (int i = 1; i < 10000000; i += 2) {
         for (int j = 0; j < 20; j++) {
             EnterCriticalSection(&ocs);
             res += sqrt(i + a);
             LeaveCriticalSection(&ocs);
         }
     }
    WaitForSingleObject(thf1, INFINITE);
    DeleteCriticalSection(&ocs);
    int endTime2 = clock();;
    std::cout << "2nd time Win.h-CritSec: " << endTime2 - startTime2 << "ms" << "\n" << res << "\n";
    res = 0;
    auto thf2 = CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(th2), NULL, CREATE_SUSPENDED, LPDWORD());
    int startTime3 = clock();
    ResumeThread(thf2);
    for (int i = 1; i < 10000000; i += 2) {
        for (int j = 0; j < 20; j++) {
            res += sqrt(i + a);
        }
    }
    WaitForSingleObject(thf2, INFINITE);
    int endTime3 = clock();;
    std::cout << "3nd time Win.h no-critSec: " << endTime3 - startTime3 << "ms" << "\n" << res << "\n";


    res = 0;
    int startTime4 = clock();
    std::thread thf3(th3);
    for (int i = 1; i < 10000000; i += 2) {
        for (int j = 0; j < 20; j++) {
            count_mutex.lock();
            res += sqrt(i + a);
            count_mutex.unlock();
        }
    }
    thf3.join();
    int endTime4 = clock();;
    std::cout << "4nd time threads+mutex: " << endTime4 - startTime4 << "ms" << "\n" << res << "\n";
    
    res = 0;
    int startTime5 = clock();
    std::thread thf4(th4);
    for (int i = 1; i < 10000000; i += 2) {
        for (int j = 0; j < 20; j++) {
            result.fetch_add(sqrt(i + a), std::memory_order_relaxed);
        }
    }
    thf4.join();
    int endTime5 = clock();;
    std::cout << "4nd time thread-atomic: " << endTime5 - startTime5 << "ms" << "\n" << result << "\n";
    return 0;
}