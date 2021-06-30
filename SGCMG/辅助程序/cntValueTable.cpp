/*
自动生成步进电机加减速所需的查找表
*/
#include <iostream>
#include <string>
#include <vector>

#define F_tim                       1000000             //定时器计数频率
#define Motor_fullStep              15                  //整步步进角，单位\degree/s
#define Motor_reducer               14                  //减速比
#define MicroStep                   16                  //微步细分
#define PI                          (double)3.14159     //PI

using namespace std;
class Solution {
private:
    
    
public:
    /* 
        功能：根据起始速度和终点速度，打印出计数器装填表
        输入：起始速度和终点速度，单位\degree/s；加速度，单位rad/s^2
        输出：计数器装填表
    */
    
    void createTable(int startVal, int endVal, int accel) {
        vector<int> cntVal(2, 0);
        cntVal[0] = calCNTVal(startVal);
        
        cntVal[1] = calCNTVal(endVal);
        
        int speed_aver = (startVal + endVal) / 2;
        double f_aver = calPulseFreq(speed_aver);
        
        int pulseNum = (int)(f_aver * (endVal - startVal)/ accel / (180 / PI));
        pulseNum *= 2;
        
        int cntInc = (cntVal[1] - cntVal[0]) / pulseNum;
        
        int curcnt = cntVal[0];
        cout << "//" << accel << "rad/s^2: " << pulseNum + 1 << " {";
        for (int i = 0; i < pulseNum; i++) {
            cout << curcnt << ", ";
            curcnt = curcnt + cntInc;
        }
        cout << cntVal[1]  << "}" << endl;
    }
    /* 
        功能：根据转速计算计数器应该装载的值
        输入：速度，单位\degree/s
        输出：计数器应该装填的值
    */
    int calCNTVal(int speed) {
        int ans;
        double f_p = calPulseFreq(speed);
        ans = (int)(F_tim / (f_p * 2));
        return ans;
    }
    /* 
        功能：根据转速计算控制脉冲频率
        输入：速度，单位\degree/s
        输出：控制脉冲频率
    */
    double calPulseFreq(int speed) {
        double f_p = (double)speed * (360 / Motor_fullStep) * MicroStep * Motor_reducer / 360;
        return f_p;
    }

};
int main() {
    Solution solve;
    int startVal;
    int endVal;
    int accel;
    // cin >> startVal >> endVal >> accel;
    // solve.createTable(startVal, endVal, accel);
    for (int i = 0; i < 3; ++i) {
        cin >> accel;
        solve.createTable(20, 120, accel);
    }

    return 0;
}