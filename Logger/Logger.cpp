// Logger.cpp : Defines the exported functions for the DLL application.
//
#include <stdio.h>
#include <time.h>
#include <sstream>
#include <iomanip>
#include "stdafx.h"
#include "Logger.h"
#include <ctime>
#include <cstdarg>

namespace Logger
{
  static int priority_level = Info::level | Warning::level | Error::level;
  static FILE * log_file = stdout;

  int Log_print(int priority, const char * logId, const char * format, va_list args)
  {
    int written = 0;
    std::stringstream formatTime;

    if (priority & priority_level)
    {
      time_t ltime;       // Time

      ltime = time(NULL);

      formatTime << std::put_time(localtime(&ltime), "%H:%M:%S");

      // Print timestamp and level info
      fprintf(log_file, "[%s] [%s] : ", formatTime.str().c_str(), logId);

      // Print formated message
      written = vfprintf(log_file, format, args);

      // Print newline
      fprintf(log_file, "\n");
    }

    return written;
  }

  int Log(int priority, const char * logId, const char * format, ...)
  {
    va_list args;
    va_start(args, format);

    int written = Log_print(priority, logId, format, args);

    va_end(args);

    return written;
  }

  int Set_Priority(int priority)
  {
    return priority_level = priority;
  }

  void Set_Logfile(const char * name)
  {
    fclose(log_file);

    log_file = fopen(name, "w");

  }
}
