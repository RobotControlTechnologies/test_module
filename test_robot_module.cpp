#include <stdio.h>
#include <stdlib.h>
#include <map>

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

#define UID "Test_robot_module_v107"

const unsigned int COUNT_ROBOTS = 99;
const unsigned int COUNT_FUNCTIONS = 6;
const unsigned int COUNT_AXIS = 3;

#define ADD_ROBOT_AXIS(AXIS_NAME, UPPER_VALUE, LOWER_VALUE) \
  robot_axis[axis_id] = new AxisData;                       \
  robot_axis[axis_id]->axis_index = axis_id + 1;            \
  robot_axis[axis_id]->upper_value = UPPER_VALUE;           \
  robot_axis[axis_id]->lower_value = LOWER_VALUE;           \
  robot_axis[axis_id]->name = AXIS_NAME;                    \
  ++axis_id;

TestRobotModule::TestRobotModule() {
#ifndef ROBOT_MODULE_H_000
  mi = new ModuleInfo;
  mi->uid = UID;
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

  }
  {
    robot_axis = new AxisData *[COUNT_AXIS];
    system_value axis_id = 0;
    ADD_ROBOT_AXIS("X", 100, -100)
    ADD_ROBOT_AXIS("Y", 1, 0)
    ADD_ROBOT_AXIS("Z", 100, 0)
  }
}

#ifdef ROBOT_MODULE_H_000
const char *TestRobotModule::getUID() { return UID; }
#else
const struct ModuleInfo &TestRobotModule::getModuleInfo() { return *mi; }
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
    TestRobot *test_robot = new TestRobot(i);
    aviable_connections[i] = test_robot;
  }
  return 0;
}

Robot *TestRobotModule::robotRequire() {
#ifdef IS_DEBUG
  colorPrintf(ConsoleColor(), "new robot require\n");
#endif

  for (m_connections::iterator i = aviable_connections.begin();
       i != aviable_connections.end(); ++i) {
    if (i->second->isAviable) {
#ifdef IS_DEBUG
      colorPrintf(ConsoleColor(ConsoleColor::green), "finded free robot\n");
#endif

      TestRobot *tr = i->second;
      tr->isAviable = false;

      return tr;
    }
  }
  return NULL;
}

void TestRobotModule::robotFree(Robot *robot) {
  TestRobot *test_robot = reinterpret_cast<TestRobot *>(robot);

  for (m_connections::iterator i = aviable_connections.begin();
       i != aviable_connections.end(); ++i) {
    if (i->second == test_robot) {
#ifdef IS_DEBUG
      colorPrintf(ConsoleColor(), "free robot\n");
#endif
      test_robot->isAviable = true;
      return;
    }
  }
}

void TestRobotModule::final() {
  for (m_connections::iterator i = aviable_connections.begin();
       i != aviable_connections.end(); ++i) {
    delete i->second;
  }
  aviable_connections.clear();
}

int TestRobotModule::startProgram(int uniq_index) { return 0; }

int TestRobotModule::endProgram(int uniq_index) { return 0; }

void TestRobotModule::destroy() {
#ifndef ROBOT_MODULE_H_000
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

FunctionResult *TestRobot::executeFunction(CommandMode mode,
                                           system_value command_index,
                                           void **args) {
  FunctionResult *fr = NULL;
#ifdef IS_DEBUG
  colorPrintf(ConsoleColor(ConsoleColor::green), "execute function - %d\n",
              command_index);
#endif

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
#ifdef ROBOT_MODULE_H_000
      fr = new FunctionResult(1, *vv);
#else
      fr = new FunctionResult(FunctionResult::Types::VALUE, *vv);
#endif
      break;
    }
    case 4: {  // throw_exception
#ifdef ROBOT_MODULE_H_000
      fr = new FunctionResult(0);
#else
      fr = new FunctionResult(FunctionResult::Types::EXCEPTION);
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
      puts(tmp);
      break;
    }
    case 6:{ // throw_value
            variable_value *vv = (variable_value *)args[0];
#ifdef ROBOT_MODULE_H_000
      fr = new FunctionResult(0, *vv);
#else
      fr = new FunctionResult(FunctionResult::Types::EXCEPTION, *vv);
#endif
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
  (*colorPrintf_p)(this, uniq_name, colors, mask, args);
  va_end(args);
}

#ifndef ROBOT_MODULE_H_000
PREFIX_FUNC_DLL unsigned short getRobotModuleApiVersion() {
  return ROBOT_MODULE_API_VERSION;
};
#endif

PREFIX_FUNC_DLL RobotModule *getRobotModuleObject() {
  return new TestRobotModule();
}