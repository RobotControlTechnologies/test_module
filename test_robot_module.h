#ifndef TEST_ROBOT_MODULE_H
#define TEST_ROBOT_MODULE_H

class TestRobotModule;

class TestRobot : public Robot {
  char *uniq_name;
  colorPrintfRobotVA_t *colorPrintf_p;

 public:
  bool isAviable;
  TestRobot(unsigned int uniq_index);
  void prepare(colorPrintfRobot_t *colorPrintf_p,
               colorPrintfRobotVA_t *colorPrintfVA_p);
  FunctionResult *executeFunction(CommandMode mode, system_value command_index,
                                  void **args);
  void axisControl(system_value axis_index, variable_value value);
  ~TestRobot();

  void colorPrintf(ConsoleColor colors, const char *mask, ...);
};
typedef std::map<int, TestRobot *> m_connections;

class TestRobotModule : public RobotModule {
  m_connections aviable_connections;
  FunctionData **robot_functions;
  AxisData **robot_axis;
  colorPrintfModuleVA_t *colorPrintf_p;

#ifndef ROBOT_MODULE_H_000
  ModuleInfo *mi;
#endif

 public:
  TestRobotModule();

#ifdef ROBOT_MODULE_H_000
  const char *getUID();
#else
  const struct ModuleInfo &getModuleInfo();
#endif
  void prepare(colorPrintfModule_t *colorPrintf_p,
               colorPrintfModuleVA_t *colorPrintfVA_p);

  FunctionData **getFunctions(unsigned int *count_functions);
  AxisData **getAxis(unsigned int *count_axis);
  void *writePC(unsigned int *buffer_length);

  int init();
  Robot *robotRequire();
  void robotFree(Robot *robot);
  void final();

  void readPC(void *buffer, unsigned int buffer_length){};

  int startProgram(int uniq_index);
  int endProgram(int uniq_index);

  void destroy();
  ~TestRobotModule(){};

  void colorPrintf(ConsoleColor colors, const char *mask, ...);
};

#endif /* TEST_ROBOT_MODULE_H */