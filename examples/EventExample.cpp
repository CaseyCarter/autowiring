// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include <autowiring/Autowired.h>
#include <iostream>
#include <memory>

class MyEventReceiver:
  public EventReceiver
{
  
};

int main(){
  std::cout << "Hello World" << std::endl;
}