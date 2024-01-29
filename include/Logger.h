// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------

/* 
To link to this, set your include dirrectorys to look for 'Logger.lib',
which should be in the projects libs/ folder; running it requires the
'Logger.dll' dll, which can be found in the projects dlls/ folder.

If you are missing the library or DLL file, or if they don't work on your
machine, you can build it with the project solution found in the Logger/ folder
*/
#pragma once
#include <stdio.h>
#include <cstdarg>

#ifdef LOGGER_EXPORTS
#define LOGGER_API __declspec(dllexport)
#else
#define LOGGER_API __declspec(dllimport)
#endif

/******************************************************************************/
/*!
  \brief 
    API for logging debugging info and setting logging levels. 
    
    Logs take a priority level and a format string to print with. When setting
    the priority level, you can bitwise-or together multiple pre-defined 
    levels, or pass in a custom level.

    Alternately, you can use one of the pre-defined loggers {Info, Warn, Error}
    to log messages. 

    Messages will only be logged if their logging level is set with
    Set_Priority. If Set_Priority is set to 0, no messages will be logged
*/
/******************************************************************************/
namespace Logger
{
  enum LogPriority {
    LOG_INFO = 0x001,
    LOG_WARNING = 0x002,
    LOG_ERROR = 0x004,
    LOG_OFF = 0,
    LOG_ALL = ~0
  };

  /****************************************************************************/
  /*!
    \brief 
      Base class for log level trait class. Only member is an enum that
      is used by the templated Log function for determingin logging priority

    \param Priority
      Enum defining the priority of the logging level
  */
  /****************************************************************************/
  struct LogLevel
  {
    enum Prioity {level = LOG_OFF};

    template<typename T>

    static const char * levelId() { return "OTHER"; }

    virtual ~LogLevel() = 0 {}
  };

  struct Info : public LogLevel
  {
    enum Priority {level = LOG_INFO};

    static const char * levelId() { return "INFO"; }
  };

  struct Warning : public Info
  {
    enum Priority {level = Info::level | LOG_WARNING};

    static const char * levelId() { return "WARNING"; }
  };

  struct Error : public Warning
  {
    enum Priority {level = Warning::level | LOG_ERROR};

    static const char * levelId() { return "ERROR"; };
  };


  /****************************************************************************/
  /*!
    \brief 
      Templated function for logging with pre-set logging priorities. Takes 
      a format string and format specifiers as arguments. The specialized type
      will be used to determine priority, and allows for custom priority info
      to be passed see above presets (Info, Warning, Error) for examples. 
      Custom logging levels should inherit from either the base LogLevel class or
      one of the other above classes.

    \param format
      Format string that will be used for the log message

    \param ...
      Format specifiers for the log message

    \return
      Number of characters that were logged
  */
  /****************************************************************************/
  template <typename T> 
  int Log(const char * format, ...)
  {
    va_list args;
    //const char * format = message.c_str();
    va_start(args, format);

    int written = Log_print(T::level, T::levelId(), format, args);

    va_end(args);

    return written;
  }

  /****************************************************************************/
  /*!
    \brief 
      Function used to log messages to the log file

    \param priority
      Priority to log the message at

    \param format
      Format string to log the message with

    \param ...
      Format specifiers for the string

    \return
      Number of characters printed into the log
  */
  /****************************************************************************/
  LOGGER_API int Log_print(int priority, const char * logId, const char * format, va_list args);

  LOGGER_API int Log(int priority, const char * format, ...);

  /****************************************************************************/
  /*!
    \brief 
      Sets the current priority level of the logger Defaults to
      { LOG_INFO | LOG_WARNING | LOG_ERROR }

    \return
      The new priority level of the logger
  */
  /****************************************************************************/
  LOGGER_API int Set_Priority(int priority);

  /****************************************************************************/
  /*!
    \brief 
      Sets the current logger output to the specified file. Defaults to stdout
  */
  /****************************************************************************/
  LOGGER_API void Set_Logfile(const char * logFile);
}
