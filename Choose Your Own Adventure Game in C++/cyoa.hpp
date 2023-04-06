#ifndef __CYOA_HPP__
#define __CYOA_HPP__

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include <algorithm>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <vector>

//using namespace std;
enum pageTypes { Normal = 'N', Win = 'W', Lose = 'L' };

class choice {
  size_t currentpgNum;
  std::pair<std::string, long int> gadget;  // added for Step-4
  size_t nextpgNum;
  std::string textMessage;

 public:
  choice();
  choice(size_t _currentpgNum,
         std::pair<std::string, long int> _gadget,
         size_t _nextpgNum,
         std::string _textMessage);
  choice(const choice & rhs);
  choice & operator=(const choice & rhs);
  size_t getcurrentpgNum() const;
  size_t getnextpgNum() const;
  std::pair<std::string, long int> getGadget() const;
  void printChoice(std::map<std::string, long int> & gadgets_story);
  ~choice();
};

class page {
  size_t pageNum;
  std::map<std::string, long int> gadgets_pg;  // for Step-4
  pageTypes pageType;
  std::string pageFile;  // just the file name
  size_t Num_choices;
  std::vector<choice> choices;

 public:
  page();
  page(size_t _pageNum,
       std::map<std::string, long int> _gadgets_pg,
       pageTypes _pageType,
       std::string _pageFile,
       size_t _Num_choices,
       std::vector<choice> _choices);
  page(const page & rhs);
  page & operator=(const page & rhs);
  void addChoice(const choice & rhs);
  void addGadget(const std::pair<std::string, long int> gadgets_pg_pair);
  size_t getpageNum() const;
  size_t getNumChoices() const;
  pageTypes getpageType() const;
  std::vector<choice> getChoices() const;
  std::map<std::string, long int> getGadgePG() const;
  void printPage(bool just_print_everything,
                 std::map<std::string, long int> & gadgets_story);
  ~page();
};

class story {
  std::map<std::string, long int> gadgets_story;
  size_t Num_pages;
  std::vector<page> pages;

 public:
  story();
  story(std::map<std::string, long int> _gadgets_story,
        size_t _Num_pages,
        std::vector<page> _pages);
  story(const story & rhs);
  story & operator=(const story & rhs);
  void printStory(bool just_print_everything);
  void playStory();
  void printWinPath() const;
  ~story();
};

// Step-1:
story readAStory(const char * directory);
page readAPage(std::string oneline, const char * directory);
choice readAChoice(std::string oneline);
size_t numstr(std::string num_str);
void printTxt(std::string filedirectory);
// Step-2: mainly implemented the void story::playstory() const;
bool valid_choiceNum_str(std::string & user_input_choice_str,
                         size_t & current_num_choices);
// Step-3: using DFS finding the path to win
// Step-4: add funcs for gadgets
std::pair<std::string, long int> readGadget_pg(std::string oneline,
                                               size_t & curnt_pg_num);
bool is_ggt_match(std::map<std::string, long int> & gadgets_story,
                  std::pair<std::string, long int> & gadget);
bool hit_unavilable(std::string & user_input_choice_str,
                    std::map<std::string, long int> & gadgets_story,
                    std::pair<std::string, long int> & gadget_local,
                    std::vector<choice> & choices_local);
#endif
