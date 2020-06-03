#include "robot_global.hpp"
#include "robot.hpp"
#include "utils/log.hpp"

Robot* global::pRobot = NULL;

int main() {
	Robot robot;
    global::pRobot = &robot;
    if (!global::pRobot->runServer()) {
        log_error("***************");
        return 0;
    }
}
