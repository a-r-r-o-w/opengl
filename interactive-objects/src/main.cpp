#include <iostream>

#include "types.hpp"
#include "application.hpp"
#include "object.hpp"

int main () {
  {
    gl::application* application = new gl::application(800, 600, 1000, "Interactive Objects");
    
    application->initialise_demo();
    application->run();

    delete application;
  }

  return 0;
}
