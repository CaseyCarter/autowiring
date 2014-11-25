// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include <exception>
#include <string>

class autowiring_error:
  public std::exception
{
public:
  autowiring_error(const std::string& what);
  virtual ~autowiring_error(void);

private:
  const std::string m_what;

public:
  const char* what(void) const throw() override {return m_what.c_str();}
};

