#include <stdio.h>
#include <stdlib.h>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <stdint.h>
#include <unistd.h>
#include <cstdarg>
#include <cstddef>
#endif

#include "module.h"
#include "robot_module.h"
#include "test_robot_module.h"

/* GLOBALS CONFIG */

#define IID "RCT.Test_robot_module_v107"

const unsigned int COUNT_ROBOTS = 99;
const unsigned int COUNT_FUNCTIONS = 7;
const unsigned int COUNT_AXIS = 3;

#define ADD_ROBOT_AXIS(AXIS_NAME, UPPER_VALUE, LOWER_VALUE) \
  robot_axis[axis_id] = new AxisData;                       \
  robot_axis[axis_id]->axis_index = axis_id + 1;            \
  robot_axis[axis_id]->upper_value = UPPER_VALUE;           \
  robot_axis[axis_id]->lower_value = LOWER_VALUE;           \
  robot_axis[axis_id]->name = AXIS_NAME;                    \
  ++axis_id;

TestRobotModule::TestRobotModule() {
#if MODULE_API_VERSION > 000
  mi = new ModuleInfo;
  mi->uid = IID;
  mi->mode = ModuleInfo::Modes::PROD;
  mi->version = BUILD_NUMBER;
  mi->digest = NULL;
#endif

  {
    robot_functions = new FunctionData *[COUNT_FUNCTIONS];
    system_value function_id = 0;
    FunctionData::ParamTypes *pt;

    robot_functions[function_id] =
        new FunctionData(function_id + 1, 0, NULL, "none");
    function_id++;

    pt = new FunctionData::ParamTypes[1];
    pt[0] = FunctionData::ParamTypes::FLOAT;
    robot_functions[function_id] =
        new FunctionData(function_id + 1, 1, pt, "do_something");
    function_id++;

    pt = new FunctionData::ParamTypes[1];
    pt[0] = FunctionData::ParamTypes::FLOAT;
    robot_functions[function_id] =
        new FunctionData(function_id + 1, 1, pt, "get_some_value");
    function_id++;

    robot_functions[function_id] =
        new FunctionData(function_id + 1, 0, NULL, "throw_exception");
    function_id++;

    pt = new FunctionData::ParamTypes[2];
    pt[0] = FunctionData::ParamTypes::STRING;
    pt[1] = FunctionData::ParamTypes::FLOAT;
    robot_functions[function_id] =
        new FunctionData(function_id + 1, 2, pt, "print");
    function_id++;

    pt = new FunctionData::ParamTypes[1];
    pt[0] = FunctionData::ParamTypes::FLOAT;
    robot_functions[function_id] =
        new FunctionData(function_id + 1, 1, pt, "throw_value");
    function_id++;

    pt = new FunctionData::ParamTypes[1];
    pt[0] = FunctionData::ParamTypes::STRING;
    robot_functions[function_id] =
        new FunctionData(function_id + 1, 1, pt, "debug");
    function_id++;

  }
  {
    robot_axis = new AxisData *[COUNT_AXIS];
    system_value axis_id = 0;
    ADD_ROBOT_AXIS("X", 100, -100)
    ADD_ROBOT_AXIS("Y", 1, 0)
    ADD_ROBOT_AXIS("Z", 100, 0)
  }
}

#if MODULE_API_VERSION > 000
const struct ModuleInfo &TestRobotModule::getModuleInfo() { return *mi; }
#else
const char *TestRobotModule::getUID() { return IID; }
#endif

void TestRobotModule::prepare(colorPrintfModule_t *colorPrintf_p,
                              colorPrintfModuleVA_t *colorPrintfVA_p) {
  this->colorPrintf_p = colorPrintfVA_p;
}

FunctionData **TestRobotModule::getFunctions(unsigned int *count_functions) {
  (*count_functions) = COUNT_FUNCTIONS;
  return robot_functions;
}

AxisData **TestRobotModule::getAxis(unsigned int *count_axis) {
  (*count_axis) = COUNT_AXIS;
  return robot_axis;
}

void *TestRobotModule::writePC(unsigned int *buffer_length) {
  (*buffer_length) = 0;
  return NULL;
}

int TestRobotModule::init() {
  for (unsigned int i = 0; i < COUNT_ROBOTS; ++i) {
    aviable_connections.push_back(new TestRobot(i + 1));
  }
  return 0;
}

#if MODULE_API_VERSION > 100
int TestRobotModule::readPC(int pc_index, void *buffer, unsigned int buffer_length) {
  return 0;
}

int TestRobotModule::startProgram(int run_index, int pc_index) {
  return 0;
}

AviableRobotsResult *TestRobotModule::getAviableRobots(int run_index) {
  std::vector<TestRobot*> aviable_robots;
  for (auto i = aviable_connections.begin();
       i != aviable_connections.end(); ++i) {
    if ((*i)->isAviable) {
      aviable_robots.push_back(*i);
    }
  }

  unsigned int count_robots = aviable_robots.size();
  if (!count_robots) {
    return NULL;
  }
  Robot **robots = new Robot*[count_robots];
  for (unsigned int i = 0; i < count_robots; ++i) {
    robots[i] = aviable_robots[i];
  }

  return new AviableRobotsResult(robots, count_robots);
}
Robot *TestRobotModule::robotRequire(int run_index, Robot *robot) {
  for (auto i = aviable_connections.begin();
       i != aviable_connections.end(); ++i) {
    if ((*i)->isAviable) {
      if ((!robot) || (robot == (*i))) {
        (*i)->isAviable = false;
        return (*i);
      }
    }
  }
  return NULL;
}
#else
int TestRobotModule::startProgram(int run_index) { return 0; }

Robot *TestRobotModule::robotRequire() {
  for (auto i = aviable_connections.begin();
       i != aviable_connections.end(); ++i) {
    if ((*i)->isAviable) {
      (*i)->isAviable = false;
      return (*i);
    }
  }
  return NULL;
}
#endif

#if MODULE_API_VERSION > 100
void TestRobotModule::robotFree(int run_index, Robot *robot) {
#else
void TestRobotModule::robotFree(Robot *robot) {
#endif
  TestRobot *test_robot = reinterpret_cast<TestRobot *>(robot);
  for (auto i = aviable_connections.begin();
       i != aviable_connections.end(); ++i) {
    if ((*i) == test_robot) {
      test_robot->isAviable = true;
      return;
    }
  }
}

void TestRobotModule::final() {
  for (auto i = aviable_connections.begin();
       i != aviable_connections.end(); ++i) {
    delete (*i);
  }
  aviable_connections.clear();
}



int TestRobotModule::endProgram(int run_index) { return 0; }

void TestRobotModule::destroy() {
#if MODULE_API_VERSION > 000
  delete mi;
#endif

  for (unsigned int j = 0; j < COUNT_FUNCTIONS; ++j) {
    if (robot_functions[j]->count_params) {
      delete[] robot_functions[j]->params;
    }
    delete robot_functions[j];
  }
  for (unsigned int j = 0; j < COUNT_AXIS; ++j) {
    delete robot_axis[j];
  }
  delete[] robot_functions;
  delete[] robot_axis;
  delete this;
}

void TestRobotModule::colorPrintf(ConsoleColor colors, const char *mask, ...) {
  va_list args;
  va_start(args, mask);
  (*colorPrintf_p)(this, colors, mask, args);
  va_end(args);
}

TestRobot::TestRobot(unsigned int uniq_index) : isAviable(true) {
  uniq_name = new char[40];
  sprintf(uniq_name, "robot-%u", uniq_index);
}

void TestRobot::prepare(colorPrintfRobot_t *colorPrintf_p,
                        colorPrintfRobotVA_t *colorPrintfVA_p) {
  this->colorPrintf_p = colorPrintfVA_p;
}

#if MODULE_API_VERSION > 100
const char *TestRobot::getUniqName() {
  return uniq_name;
}

FunctionResult *TestRobot::executeFunction(int run_index, CommandMode mode,
                                           system_value command_index,
                                           void **args) {
#else
FunctionResult *TestRobot::executeFunction(CommandMode mode,
                                           system_value command_index,
                                           void **args) {
#endif

  FunctionResult *fr = NULL;

  switch (command_index) {
    case 1: {  // none
      break;
    }
    case 2: {  // do_something
      variable_value *vv = (variable_value *)args[0];
#ifdef _WIN32
      Sleep((DWORD)*vv);
#else
      usleep(((uint32_t)*vv) * 1000);
#endif
      break;
    }
    case 3: {  // get_some_value
      variable_value *vv = (variable_value *)args[0];
#if MODULE_API_VERSION > 000
      fr = new FunctionResult(FunctionResult::Types::VALUE, *vv);
#else
      fr = new FunctionResult(1, *vv);
#endif
      break;
    }
    case 4: {  // throw_exception
#if MODULE_API_VERSION > 000
      fr = new FunctionResult(FunctionResult::Types::EXCEPTION);
#else
      fr = new FunctionResult(0);
#endif

      break;
    }
    case 5: {  // print
      variable_value *vv = (variable_value *)args[1];
#ifdef _WIN32
      Sleep((DWORD)*vv);
#else
      usleep(((uint32_t)*vv) * 1000);
#endif
      const char *tmp = (const char *)args[0];
      colorPrintf(ConsoleColor(ConsoleColor::white), "%s", tmp);
      break;
    }
    case 6:{ // throw_value
            variable_value *vv = (variable_value *)args[0];
#if MODULE_API_VERSION > 000
      fr = new FunctionResult(FunctionResult::Types::EXCEPTION, *vv);
#else
      fr = new FunctionResult(0, *vv);
#endif
    }
    case 7: {  // debug
      const char *tmp = (const char *)args[0];
      colorPrintf(ConsoleColor(ConsoleColor::white), "%s", tmp);
      break;
    }
    default:
      break;
  }

  return fr;
}

void TestRobot::axisControl(system_value axis_index, variable_value value) {
  const char *name;
  switch (axis_index) {
    case 1: {
      name = "X";
      break;
    }
    case 2: {
      name = "Y";
      break;
    }
    case 3: {
      name = "Z";
      break;
    }
    default: {
      name = "O_o";
      break;
    };
  }
  colorPrintf(ConsoleColor(ConsoleColor::green), "change axis value: %s = %f\n",
              name, value);
}

TestRobot::~TestRobot() { delete[] uniq_name; }

void TestRobot::colorPrintf(ConsoleColor colors, const char *mask, ...) {
  va_list args;
  va_start(args, mask);
#if MODULE_API_VERSION > 100
  (*colorPrintf_p)(this, colors, mask, args);
#else
  (*colorPrintf_p)(this, uniq_name, colors, mask, args);
#endif
  va_end(args);
}

#if MODULE_API_VERSION > 000
PREFIX_FUNC_DLL unsigned short getRobotModuleApiVersion() {
  return MODULE_API_VERSION;
};
#endif

PREFIX_FUNC_DLL RobotModule *getRobotModuleObject() {
  return new TestRobotModule();
}