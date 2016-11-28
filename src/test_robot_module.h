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
#if MODULE_API_VERSION > 100
  const char *getUniqName();
  FunctionResult *executeFunction(int run_index, CommandMode mode, system_value command_index,
                                  void **args);
#else
  FunctionResult *executeFunction(CommandMode mode, system_value command_index,
                                  void **args);
#endif                                  
  void axisControl(system_value axis_index, variable_value value);
  ~TestRobot();

  void colorPrintf(ConsoleColor colors, const char *mask, ...);
};

class TestRobotModule : public RobotModule {
  std::vector<TestRobot *> aviable_connections;
  FunctionData **robot_functions;
  AxisData **robot_axis;
  colorPrintfModuleVA_t *colorPrintf_p;

#if MODULE_API_VERSION > 000
  ModuleInfo *mi;
#endif

 public:
  TestRobotModule();

#if MODULE_API_VERSION > 000
  const struct ModuleInfo &getModuleInfo();
#else
  const char *getUID();
#endif

  void prepare(colorPrintfModule_t *colorPrintf_p,
               colorPrintfModuleVA_t *colorPrintfVA_p);

  FunctionData **getFunctions(unsigned int *count_functions);
  AxisData **getAxis(unsigned int *count_axis);
  void *writePC(unsigned int *buffer_length);

  int init();
  void final();

#if MODULE_API_VERSION > 100
  int readPC(int pc_index, void *buffer, unsigned int buffer_length);
  int startProgram(int run_index, int pc_index);
  AviableRobotsResult *getAviableRobots(int run_index);
  Robot *robotRequire(int run_index, Robot *robot);
  void robotFree(int run_index, Robot *robot);
#else
  void readPC(void *buffer, unsigned int buffer_length){};
  int startProgram(int run_index);
  Robot *robotRequire();
  void robotFree(Robot *robot);
#endif
  
  int endProgram(int run_index);

  void destroy();
  ~TestRobotModule(){};

  void colorPrintf(ConsoleColor colors, const char *mask, ...);
};

#endif /* TEST_ROBOT_MODULE_H */