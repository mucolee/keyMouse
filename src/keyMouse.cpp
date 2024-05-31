//
// Created by 李海铭 on 2024/5/31.
//
// 定义钩子过程函数
#include <Windows.h>
#include <fstream>
#include <iostream>

bool isOperation = true;
int config[20] = {};

void readConfig() {
    std::ifstream fin;
    fin.open("config", std::ios::in);
    if (!fin.is_open()) {
        return;
    }
    int i=0;
    char buf[16] = { 0 };
    while (fin.getline(buf,sizeof buf)){
        config[i]=std::stoi(buf);
        ++i;
    }
    fin.close();
}

/*
     * 有参鼠标左击函数mouseEvent_leftClick()
     * @int posX 鼠标X坐标
     * @int posY 鼠标Y坐标
     * #int 返回值类型
     * */
int mouseLeftClick(int posX, int posY) {
    mouse_event(MOUSEEVENTF_LEFTDOWN, posX, posY, 0, 0);
    mouse_event(MOUSEEVENTF_LEFTUP, posX, posY, 0, 0);
    return 1;
}

/*
 * 有参鼠标右击击函数mouseEvent_rightClick
 * @int posX 鼠标X坐标
 * @int posY 鼠标Y坐标
 * #int 返回值类型
 * */
int mouseRightClick(int posX, int posY) {
    mouse_event(MOUSEEVENTF_RIGHTDOWN, posX, posY, 0, 0);
    mouse_event(MOUSEEVENTF_RIGHTUP, posX, posY, 0, 0);
    return 1;
}

/*
     * 鼠标滚轮事件
     * @int scrollingDirection 滚动步频，负数向下滚反之向上
     * #int 返回值
     * */
int mouseWheelEvent(int scrollingDirection) {
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_WHEEL;
    input.mi.mouseData = scrollingDirection;
    SendInput(1, &input, sizeof(INPUT));
    return 1;
}

/*
     * 鼠标移动
     * int keyCode 键盘码
     *  int posX 鼠标坐标X位置
     *  int posY 鼠标坐标Y位置
     *  int mouseSpeed = 5 鼠标移动步长
     * */
int mouseEvent(int keyCode, int posX, int posY, int mouseSpeed = config[13]) {
    if (keyCode == config[1]) {//上
        SetCursorPos(posX, posY - mouseSpeed);
        return 1;
    } else if (keyCode == config[2]) {//下
        SetCursorPos(posX, posY + mouseSpeed);
        return 1;
    } else if (keyCode == config[3]) {//左
        SetCursorPos(posX - mouseSpeed, posY);
        return 1;
    } else if (keyCode == config[4]) {//右
        SetCursorPos(posX + mouseSpeed, posY);
        return 1;
    } else if (keyCode == config[5]) {//左上
        SetCursorPos(posX - mouseSpeed, posY - mouseSpeed);
        return 1;
    } else if (keyCode == config[6]) {//右上
        SetCursorPos(posX + mouseSpeed, posY - mouseSpeed);
        return 1;
    } else if (keyCode == config[7]) {//左下
        SetCursorPos(posX - mouseSpeed, posY + mouseSpeed);
        return 1;
    } else if (keyCode == config[8]) {//右下
        SetCursorPos(posX + mouseSpeed, posY + mouseSpeed);
        return 1;
    } else if (keyCode == config[9]) {//鼠标左键
        return mouseLeftClick(posX, posY);
    } else if (keyCode == config[10]) {//鼠标右键
        return mouseRightClick(posX, posY);
    } else if (keyCode == config[11]) {//鼠标上滚
        return mouseWheelEvent(config[14]);
    } else if (keyCode == config[12]) {//鼠标下滚
        return mouseWheelEvent(-config[14]);
    } else return 0;
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        if (wParam == WM_KEYDOWN) {
            KBDLLHOOKSTRUCT *pkb = (KBDLLHOOKSTRUCT *) lParam;
            // 按键码
            int keyCode = pkb->vkCode;
            //布尔开关
            if (keyCode == VK_ESCAPE) {
                isOperation = !isOperation;
            }
            if (!isOperation) {
                return 0;
            }

            // 检测组合键 Ctrl + Q 的按下事件，用于退出程序
            if (GetAsyncKeyState(VK_CONTROL) & 0x8000 && pkb->vkCode == 0x51) {
                // 如果按下 Ctrl + Q，则退出程序
                exit(0);
            }

            // 检测组合键 Ctrl + A 的按下事件
//            if (GetAsyncKeyState(VK_CONTROL) & 0x8000 &&  pkb->vkCode == 0x41 ) {
//                std::cout << "Ctrl +  A pressd!ed!" << std::endl;
//
//            }

            // 输出按下的按键码
            printf("Key down: %d\n", keyCode);
            POINT cursorPos;
            // 获取鼠标位置
            if (GetCursorPos(&cursorPos)) {
                // 输出鼠标位置
                std::cout << "Mouse position: X = " << cursorPos.x << ", Y = " << cursorPos.y << std::endl;
            } else {
                std::cerr << "Failed to get mouse position!" << std::endl;
            }

            //鼠标移动
            int mouseReturn = mouseEvent(keyCode, cursorPos.x, cursorPos.y, 30);
            return mouseReturn;
        }
    }
    // 继续传递消息给下一个钩子或默认过程
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void mountingHook() {
    // 安装键盘钩子
    HHOOK hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    if (hHook == NULL) {
        printf("Failed to install hook!\n");
        return;
    }

    // 消息循环，等待键盘事件
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) != 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 卸载钩子
    UnhookWindowsHookEx(hHook);
}


int main() {
    readConfig();
    mountingHook();
    return 0;
}