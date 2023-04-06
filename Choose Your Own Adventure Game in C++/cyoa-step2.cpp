#include "cyoa.hpp"

int main(int argc, char ** argv) {
  // read input (argc is 2)
  if (argc != 2) {
    std::cerr << "Expected input: ./cyoa-step2 /Directory/to/story.txt" << std::endl;
    return EXIT_FAILURE;
  }
  story mystory;
  // read file and store it in a class called story
  mystory = readAStory(argv[1]);
  // play the story using the built-in method
  mystory.playStory();
  // STL would clean up everything for me
  return EXIT_SUCCESS;
}
