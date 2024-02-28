#include <iostream>
#include <Windows.h>
#include <thread>
#include <math.h>
#include <iomanip>
#include <fstream>
#include <string>


double res;
int a;
CRITICAL_SECTION ocs;

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

int main()
{
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
    std::cout << "1st time: " << endTime1 - startTime1 << "ms" << "\n" << res << "\n";
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
    std::cout << "2nd time: " << endTime2 - startTime2 << "ms" << "\n" << res << "\n";
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
    std::cout << "3nd time: " << endTime3 - startTime3 << "ms" << "\n" << res << "\n";

    return 0;
}